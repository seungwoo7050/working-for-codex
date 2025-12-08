/**
 * [FILE]
 * - 목적: 비디오 메타데이터 분석기
 * - 주요 역할: 비디오/오디오 스트림 정보, 코덱, 해상도, FPS 등 추출
 * - 관련 클론 가이드 단계: [CG-v2.2.0] 메타데이터 분석
 * - 권장 읽는 순서: thumbnail_extractor.cpp 이후
 *
 * [LEARN] C 개발자를 위한 FFmpeg 메타데이터:
 * - AVFormatContext: 컨테이너(MP4, MKV 등) 정보
 * - AVStream: 개별 스트림(비디오, 오디오, 자막)
 * - AVCodecParameters: 코덱 파라미터(코덱 ID, 해상도, 샘플레이트 등)
 * - AVDictionary: 키-값 쌍의 메타데이터 저장소
 *
 * [Reader Notes]
 * - avformat_open_input: 파일을 열고 포맷 감지
 * - avformat_find_stream_info: 스트림 정보 분석
 * - AVCodecParameters: 디코딩 없이 코덱 정보만 읽기
 */

#include "metadata_analyzer.h"
#include <stdexcept>
#include <cstring>

// [LEARN] FFmpeg C 라이브러리 extern "C" 래핑
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
}

namespace videoeditor {

// [LEARN] 메타데이터 추출 메인 함수
// - 파일을 열어 모든 스트림의 정보를 분석한다.
MetadataAnalyzer::VideoMetadata MetadataAnalyzer::extract_metadata(const std::string& video_path) {
  VideoMetadata metadata;

  // [LEARN] Step 1: 파일 열기
  // Open input file
  AVFormatContext* fmt_ctx_raw = nullptr;
  if (avformat_open_input(&fmt_ctx_raw, video_path.c_str(), nullptr, nullptr) < 0) {
    throw std::runtime_error("Failed to open video file: " + video_path);
  }
  AVFormatContextPtr fmt_ctx(fmt_ctx_raw);

  // [LEARN] Step 2: 스트림 정보 찾기
  // Retrieve stream information
  if (avformat_find_stream_info(fmt_ctx.get(), nullptr) < 0) {
    throw std::runtime_error("Failed to find stream information");
  }

  // [LEARN] Step 3: 포맷(컨테이너) 정보 추출
  // Extract format information
  metadata.format.format_name = fmt_ctx->iformat->name ? fmt_ctx->iformat->name : "";
  metadata.format.format_long_name = fmt_ctx->iformat->long_name ? fmt_ctx->iformat->long_name : "";
  metadata.format.duration_sec = fmt_ctx->duration > 0 ? fmt_ctx->duration / static_cast<double>(AV_TIME_BASE) : 0.0;
  metadata.format.size_bytes = fmt_ctx->pb ? avio_size(fmt_ctx->pb) : 0;
  metadata.format.bitrate = fmt_ctx->bit_rate;
  metadata.format.nb_streams = fmt_ctx->nb_streams;

  // [LEARN] Step 4: 컨테이너 메타데이터 추출
  // - av_dict_get으로 딕셔너리를 순회한다.
  // Extract container metadata
  AVDictionaryEntry* tag = nullptr;
  while ((tag = av_dict_get(fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX))) {
    metadata.format.metadata[tag->key] = tag->value;
  }

  // [LEARN] Step 5: 각 스트림 분석
  // Process each stream
  for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
    AVStream* stream = fmt_ctx->streams[i];
    AVCodecParameters* codecpar = stream->codecpar;

    if (codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      // [LEARN] 비디오 스트림 정보 추출
      // Video stream
      VideoStreamInfo video_info;
      video_info.codec_name = get_codec_name(codecpar->codec_id);

      const AVCodecDescriptor* desc = avcodec_descriptor_get(codecpar->codec_id);
      video_info.codec_long_name = desc ? desc->long_name : "";

      video_info.width = codecpar->width;
      video_info.height = codecpar->height;
      video_info.bitrate = codecpar->bit_rate;
      video_info.pixel_format = get_pixel_format_name(static_cast<AVPixelFormat>(codecpar->format));

      // [LEARN] FPS 계산
      // - avg_frame_rate 또는 r_frame_rate에서 FPS를 얻는다.
      // Calculate FPS
      if (stream->avg_frame_rate.den > 0) {
        video_info.fps = av_q2d(stream->avg_frame_rate);
      } else if (stream->r_frame_rate.den > 0) {
        video_info.fps = av_q2d(stream->r_frame_rate);
      } else {
        video_info.fps = 0.0;
      }

      video_info.nb_frames = stream->nb_frames;

      metadata.video_streams.push_back(video_info);

    } else if (codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      // [LEARN] 오디오 스트림 정보 추출
      // Audio stream
      AudioStreamInfo audio_info;
      audio_info.codec_name = get_codec_name(codecpar->codec_id);

      const AVCodecDescriptor* desc = avcodec_descriptor_get(codecpar->codec_id);
      audio_info.codec_long_name = desc ? desc->long_name : "";

      audio_info.sample_rate = codecpar->sample_rate;
      audio_info.bitrate = codecpar->bit_rate;

      // [LEARN] 채널 정보 추출 (FFmpeg 버전 호환)
      // - FFmpeg 5.0+에서는 ch_layout 사용
      // - FFmpeg 4.x에서는 channels와 channel_layout 사용
      // Get channel info - compatible with both old and new FFmpeg API
      char layout_name[256];
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT(57, 24, 100)
      // FFmpeg 5.0+ with new channel layout API
      audio_info.channels = codecpar->ch_layout.nb_channels;
      av_channel_layout_describe(&codecpar->ch_layout, layout_name, sizeof(layout_name));
#else
      // FFmpeg 4.x with old channel layout API
      audio_info.channels = codecpar->channels;
      av_get_channel_layout_string(layout_name, sizeof(layout_name),
                                     codecpar->channels, codecpar->channel_layout);
#endif
      audio_info.channel_layout = layout_name;

      metadata.audio_streams.push_back(audio_info);
    }
  }

  return metadata;
}

bool MetadataAnalyzer::is_codec_supported(const std::string& codec_name) {
  const AVCodec* codec = avcodec_find_decoder_by_name(codec_name.c_str());
  return codec != nullptr;
}

std::string MetadataAnalyzer::get_codec_name(enum AVCodecID codec_id) {
  const AVCodecDescriptor* desc = avcodec_descriptor_get(codec_id);
  return desc ? desc->name : "unknown";
}

std::string MetadataAnalyzer::get_pixel_format_name(enum AVPixelFormat pix_fmt) {
  const char* name = av_get_pix_fmt_name(pix_fmt);
  return name ? name : "unknown";
}

} // namespace videoeditor
