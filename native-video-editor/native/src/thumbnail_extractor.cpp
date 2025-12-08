/**
 * [FILE]
 * - 목적: 비디오에서 썸네일(프레임) 추출
 * - 주요 역할: FFmpeg C API를 사용하여 특정 시간의 프레임을 이미지로 변환
 * - 관련 클론 가이드 단계: [CG-v2.1.0] 썸네일 추출
 * - 권장 읽는 순서: video_processor.cpp 이후
 *
 * [LEARN] C 개발자를 위한 FFmpeg C API:
 * - FFmpeg은 비디오/오디오 처리를 위한 C 라이브러리이다.
 * - avformat: 컨테이너(MP4, MKV 등) 처리
 * - avcodec: 코덱(H.264, HEVC 등) 인코딩/디코딩
 * - swscale: 픽셀 포맷/크기 변환
 * - 이 코드는 C 스타일 FFmpeg API를 C++ RAII 래퍼로 감싸서 사용한다.
 *
 * [Reader Notes]
 * - AVFormatContext: 미디어 파일 컨텍스트 (스트림 정보 포함)
 * - AVCodecContext: 코덱 컨텍스트 (디코딩 설정)
 * - AVFrame: 디코딩된 프레임 (원시 픽셀 데이터)
 * - AVPacket: 압축된 데이터 패킷
 */

#include "thumbnail_extractor.h"
#include <stdexcept>
#include <chrono>
#include <cstring>

// [LEARN] FFmpeg은 C 라이브러리이므로 extern "C"로 감싼다
// - C++에서 C 함수를 호출할 때 네임 맹글링을 방지한다.
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace videoeditor {

/**
 * [LEARN] 생성자 초기화 리스트
 * - C++11 스타일로 멤버 변수를 초기화한다.
 * - frame_pool_: 메모리 풀로 AVFrame 재사용 (성능 최적화)
 */
ThumbnailExtractor::ThumbnailExtractor()
  : frame_pool_(std::make_unique<AVFramePool>(10)),  // 최대 10개 프레임 풀
    total_extractions_(0),
    total_duration_ms_(0.0),
    cache_hits_(0),
    cache_misses_(0) {
}

/**
 * [LEARN] 비디오 파일 열기
 * - FFmpeg의 표준 패턴: open -> find_stream_info -> find_decoder -> open_codec
 */
ThumbnailExtractor::VideoContext ThumbnailExtractor::open_video(const std::string& video_path) {
  VideoContext ctx;

  // [LEARN] Step 1: 파일 열기
  // - avformat_open_input()으로 미디어 파일을 연다.
  // - 실패 시 음수 에러 코드를 반환한다.
  // Open input file
  AVFormatContext* fmt_ctx_raw = nullptr;
  if (avformat_open_input(&fmt_ctx_raw, video_path.c_str(), nullptr, nullptr) < 0) {
    throw std::runtime_error("Failed to open video file: " + video_path);
  }
  // [LEARN] RAII 래퍼로 감싸서 자동 해제
  ctx.format_ctx = AVFormatContextPtr(fmt_ctx_raw);

  // [LEARN] Step 2: 스트림 정보 찾기
  // - 파일 헤더에서 오디오/비디오 스트림 정보를 읽는다.
  // Retrieve stream information
  if (avformat_find_stream_info(ctx.format_ctx.get(), nullptr) < 0) {
    throw std::runtime_error("Failed to find stream information");
  }

  // [LEARN] Step 3: 비디오 스트림 찾기
  // - 파일에 여러 스트림(비디오, 오디오, 자막 등)이 있을 수 있다.
  // - AVMEDIA_TYPE_VIDEO 타입의 스트림을 찾는다.
  // Find video stream
  ctx.video_stream_index = -1;
  for (unsigned int i = 0; i < ctx.format_ctx->nb_streams; i++) {
    if (ctx.format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      ctx.video_stream_index = i;
      break;
    }
  }

  if (ctx.video_stream_index == -1) {
    throw std::runtime_error("No video stream found");
  }

  // [LEARN] Step 4: 코덱 파라미터 가져오기
  // - 비디오 스트림에서 코덱 정보(코덱 ID, 해상도 등)를 읽는다.
  // Get codec parameters
  AVCodecParameters* codecpar = ctx.format_ctx->streams[ctx.video_stream_index]->codecpar;

  // [LEARN] Step 5: 디코더 찾기
  // - 코덱 ID에 해당하는 디코더를 찾는다 (예: H.264 디코더).
  // Find decoder
  const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
  if (!codec) {
    throw std::runtime_error("Unsupported codec");
  }

  // [LEARN] Step 6: 코덱 컨텍스트 할당 및 설정
  // Allocate codec context
  ctx.codec_ctx = make_codec_context(codec);
  if (!ctx.codec_ctx) {
    throw std::runtime_error("Failed to allocate codec context");
  }

  // Copy codec parameters to codec context
  if (avcodec_parameters_to_context(ctx.codec_ctx.get(), codecpar) < 0) {
    throw std::runtime_error("Failed to copy codec parameters");
  }

  // [LEARN] Step 7: 코덱 열기
  // - 이제 이 컨텍스트로 프레임을 디코딩할 수 있다.
  // Open codec
  if (avcodec_open2(ctx.codec_ctx.get(), codec, nullptr) < 0) {
    throw std::runtime_error("Failed to open codec");
  }

  // [LEARN] time_base: 타임스탬프 단위
  // - 예: 1/1000이면 밀리초 단위, 1/90000이면 90kHz 단위
  // Store time base
  ctx.time_base = ctx.format_ctx->streams[ctx.video_stream_index]->time_base;

  return ctx;
}

AVFramePtr ThumbnailExtractor::seek_and_decode(VideoContext& ctx, double timestamp_sec) {
  // Convert timestamp to stream time base
  int64_t seek_target = static_cast<int64_t>(timestamp_sec / av_q2d(ctx.time_base));

  // Seek to timestamp
  if (av_seek_frame(ctx.format_ctx.get(), ctx.video_stream_index, seek_target, AVSEEK_FLAG_BACKWARD) < 0) {
    throw std::runtime_error("Failed to seek to timestamp");
  }

  // Flush codec buffers
  avcodec_flush_buffers(ctx.codec_ctx.get());

  // Allocate packet
  AVPacketPtr packet = make_av_packet();
  if (!packet) {
    throw std::runtime_error("Failed to allocate packet");
  }

  // Acquire frame from pool
  AVFramePtr frame = frame_pool_->acquire();
  if (!frame) {
    throw std::runtime_error("Failed to allocate frame");
  }

  // Read frames until we find one at or after the target timestamp
  bool found_frame = false;
  while (av_read_frame(ctx.format_ctx.get(), packet.get()) >= 0) {
    if (packet->stream_index == ctx.video_stream_index) {
      // Send packet to decoder
      int ret = avcodec_send_packet(ctx.codec_ctx.get(), packet.get());
      if (ret < 0) {
        av_packet_unref(packet.get());
        continue;
      }

      // Receive decoded frame
      ret = avcodec_receive_frame(ctx.codec_ctx.get(), frame.get());
      if (ret == 0) {
        // Successfully decoded frame
        found_frame = true;
        av_packet_unref(packet.get());
        break;
      }
    }
    av_packet_unref(packet.get());
  }

  if (!found_frame) {
    throw std::runtime_error("Failed to decode frame at timestamp");
  }

  return frame;
}

std::vector<uint8_t> ThumbnailExtractor::frame_to_jpeg(AVFrame* frame, int target_width, int target_height) {
  // Determine output dimensions
  int out_width = target_width > 0 ? target_width : frame->width;
  int out_height = target_height > 0 ? target_height : frame->height;

  // Create scaling context
  SwsContextPtr sws_ctx(sws_getContext(
    frame->width, frame->height, static_cast<AVPixelFormat>(frame->format),
    out_width, out_height, AV_PIX_FMT_RGB24,
    SWS_BILINEAR, nullptr, nullptr, nullptr
  ));

  if (!sws_ctx) {
    throw std::runtime_error("Failed to create scaling context");
  }

  // Allocate RGB frame
  AVFramePtr rgb_frame = make_av_frame();
  if (!rgb_frame) {
    throw std::runtime_error("Failed to allocate RGB frame");
  }

  rgb_frame->format = AV_PIX_FMT_RGB24;
  rgb_frame->width = out_width;
  rgb_frame->height = out_height;

  if (av_frame_get_buffer(rgb_frame.get(), 32) < 0) {
    throw std::runtime_error("Failed to allocate RGB frame buffer");
  }

  // Convert to RGB
  sws_scale(
    sws_ctx.get(),
    frame->data, frame->linesize, 0, frame->height,
    rgb_frame->data, rgb_frame->linesize
  );

  // For simplicity, return raw RGB data
  // In production, you would encode to JPEG here using libjpeg or similar
  size_t data_size = rgb_frame->linesize[0] * out_height;
  std::vector<uint8_t> result(data_size);
  std::memcpy(result.data(), rgb_frame->data[0], data_size);

  return result;
}

std::vector<uint8_t> ThumbnailExtractor::extract_thumbnail(
  const std::string& video_path,
  double timestamp_sec,
  int width,
  int height
) {
  auto start_time = std::chrono::high_resolution_clock::now();

  try {
    // Open video
    VideoContext ctx = open_video(video_path);

    // Seek and decode frame
    AVFramePtr frame = seek_and_decode(ctx, timestamp_sec);

    // Convert to JPEG
    std::vector<uint8_t> jpeg_data = frame_to_jpeg(frame.get(), width, height);

    // Release frame back to pool
    frame_pool_->release(std::move(frame));

    // Update statistics
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    {
      std::lock_guard<std::mutex> lock(stats_mutex_);
      total_extractions_++;
      total_duration_ms_ += duration_ms;
      cache_misses_++; // In this version, we don't use cache yet
    }

    return jpeg_data;
  } catch (const std::exception& e) {
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    {
      std::lock_guard<std::mutex> lock(stats_mutex_);
      total_extractions_++;
      total_duration_ms_ += duration_ms;
    }

    throw;
  }
}

ThumbnailExtractor::Stats ThumbnailExtractor::get_stats() const {
  std::lock_guard<std::mutex> lock(stats_mutex_);

  Stats stats;
  stats.total_extractions = total_extractions_;
  stats.avg_duration_ms = total_extractions_ > 0 ? total_duration_ms_ / total_extractions_ : 0.0;
  stats.cache_hits = cache_hits_;
  stats.cache_misses = cache_misses_;

  return stats;
}

} // namespace videoeditor
