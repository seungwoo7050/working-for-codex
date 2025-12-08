/**
 * [FILE]
 * - 목적: Node.js ↔ C++ 바인딩 (N-API 래퍼)
 * - 주요 역할: C++ 클래스를 JavaScript에서 사용할 수 있게 래핑
 * - 관련 클론 가이드 단계: [CG-v2.0.0] Native Addon 기본 구조
 * - 권장 읽는 순서: Order 1 (ThumbnailExtractorWrapper) → Order 2 (MetadataAnalyzerWrapper)
 *
 * [LEARN] C 개발자를 위한 N-API:
 * - N-API (Node-API)는 Node.js와 C/C++ 코드를 연결하는 ABI 안정적 API이다.
 * - V8 엔진의 세부 사항을 추상화하여 Node.js 버전 간 호환성을 보장한다.
 * - JavaScript 객체를 C++ 객체로 래핑하고, 메서드를 노출한다.
 *
 * [Reader Notes]
 * - Napi::ObjectWrap: C++ 클래스를 JavaScript 객체로 래핑하는 헬퍼
 * - DefineClass: JavaScript 클래스 정의 (생성자 + 메서드)
 * - InstanceMethod: 인스턴스 메서드 등록
 * - 이 파일은 thumbnail_extractor.cpp와 metadata_analyzer.cpp를 래핑한다.
 */

#include <napi.h>
#include "thumbnail_extractor.h"
#include "metadata_analyzer.h"
#include <memory>

using namespace videoeditor;

/**
 * [Order 1] ThumbnailExtractor N-API 래퍼
 * - C++의 ThumbnailExtractor 클래스를 JavaScript에서 사용할 수 있게 한다.
 * - new ThumbnailExtractor()로 인스턴스를 생성할 수 있다.
 *
 * N-API wrapper for ThumbnailExtractor
 */
class ThumbnailExtractorWrapper : public Napi::ObjectWrap<ThumbnailExtractorWrapper> {
public:
  /**
   * [LEARN] 모듈 초기화
   * - DefineClass로 JavaScript 클래스를 정의한다.
   * - InstanceMethod로 extractThumbnail, getStats 메서드를 등록한다.
   * - exports.Set으로 모듈의 export 객체에 클래스를 추가한다.
   */
  static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "ThumbnailExtractor", {
      InstanceMethod("extractThumbnail", &ThumbnailExtractorWrapper::ExtractThumbnail),
      InstanceMethod("getStats", &ThumbnailExtractorWrapper::GetStats),
    });

    // [LEARN] Persistent 참조
    // - JavaScript 가비지 컬렉터가 함수를 수집하지 않도록 참조를 유지한다.
    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("ThumbnailExtractor", func);
    return exports;
  }

  /**
   * [LEARN] C++ 생성자
   * - JavaScript에서 new ThumbnailExtractor() 호출 시 실행된다.
   * - info 객체로 JavaScript에서 전달된 인자에 접근한다.
   * - std::make_unique로 실제 C++ 객체를 생성한다.
   */
  ThumbnailExtractorWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<ThumbnailExtractorWrapper>(info),
      extractor_(std::make_unique<ThumbnailExtractor>()) {
  }

private:
  /**
   * [LEARN] 메서드 래핑
   * - JavaScript에서 호출된 메서드를 C++ 메서드로 연결한다.
   * - info.Length(): 인자 개수
   * - info[n]: n번째 인자
   * - As<Type>(): JavaScript 값을 C++ 타입으로 변환
   */
  Napi::Value ExtractThumbnail(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // [LEARN] 인자 검증
    // - JavaScript는 동적 타입이므로 C++에서 명시적으로 검증해야 한다.
    // Validate arguments
    if (info.Length() < 2) {
      Napi::TypeError::New(env, "Expected at least 2 arguments").ThrowAsJavaScriptException();
      return env.Null();
    }

    if (!info[0].IsString()) {
      Napi::TypeError::New(env, "Argument 0 must be a string (video path)").ThrowAsJavaScriptException();
      return env.Null();
    }

    if (!info[1].IsNumber()) {
      Napi::TypeError::New(env, "Argument 1 must be a number (timestamp)").ThrowAsJavaScriptException();
      return env.Null();
    }

    // [LEARN] JavaScript 값 → C++ 값 변환
    std::string video_path = info[0].As<Napi::String>().Utf8Value();
    double timestamp = info[1].As<Napi::Number>().DoubleValue();

    int width = 0;
    int height = 0;

    // [LEARN] 선택적 인자 처리
    if (info.Length() >= 3 && info[2].IsNumber()) {
      width = info[2].As<Napi::Number>().Int32Value();
    }

    if (info.Length() >= 4 && info[3].IsNumber()) {
      height = info[3].As<Napi::Number>().Int32Value();
    }

    try {
      // Extract thumbnail
      std::vector<uint8_t> thumbnail_data = extractor_->extract_thumbnail(video_path, timestamp, width, height);

      // [LEARN] C++ 데이터 → JavaScript Buffer 변환
      // - Napi::Buffer::Copy로 데이터를 JavaScript의 Buffer 객체로 복사한다.
      // - JavaScript에서 이 Buffer를 이미지로 사용할 수 있다.
      // Create Node.js Buffer
      Napi::Buffer<uint8_t> buffer = Napi::Buffer<uint8_t>::Copy(env, thumbnail_data.data(), thumbnail_data.size());

      return buffer;
    } catch (const std::exception& e) {
      // [LEARN] C++ 예외 → JavaScript 예외 변환
      Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
      return env.Null();
    }
  }

  /**
   * [LEARN] 통계 객체 반환
   * - C++ 구조체를 JavaScript 객체로 변환한다.
   * - Napi::Object::New()로 빈 객체를 만들고 Set()으로 속성을 추가한다.
   */
  Napi::Value GetStats(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    ThumbnailExtractor::Stats stats = extractor_->get_stats();

    Napi::Object result = Napi::Object::New(env);
    result.Set("totalExtractions", Napi::Number::New(env, stats.total_extractions));
    result.Set("avgDurationMs", Napi::Number::New(env, stats.avg_duration_ms));
    result.Set("cacheHits", Napi::Number::New(env, stats.cache_hits));
    result.Set("cacheMisses", Napi::Number::New(env, stats.cache_misses));

    return result;
  }

  // [LEARN] 실제 C++ 객체를 unique_ptr로 관리 (RAII 패턴)
  std::unique_ptr<ThumbnailExtractor> extractor_;
};

/**
 * [Order 2] MetadataAnalyzer N-API 래퍼
 *
 * N-API wrapper for MetadataAnalyzer
 */
class MetadataAnalyzerWrapper : public Napi::ObjectWrap<MetadataAnalyzerWrapper> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "MetadataAnalyzer", {
      InstanceMethod("extractMetadata", &MetadataAnalyzerWrapper::ExtractMetadata),
      StaticMethod("isCodecSupported", &MetadataAnalyzerWrapper::IsCodecSupported),
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("MetadataAnalyzer", func);
    return exports;
  }

  MetadataAnalyzerWrapper(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<MetadataAnalyzerWrapper>(info),
      analyzer_(std::make_unique<MetadataAnalyzer>()) {
  }

private:
  Napi::Value ExtractMetadata(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "Expected string argument (video path)").ThrowAsJavaScriptException();
      return env.Null();
    }

    std::string video_path = info[0].As<Napi::String>().Utf8Value();

    try {
      MetadataAnalyzer::VideoMetadata metadata = analyzer_->extract_metadata(video_path);

      Napi::Object result = Napi::Object::New(env);

      // Format info
      Napi::Object format_obj = Napi::Object::New(env);
      format_obj.Set("formatName", Napi::String::New(env, metadata.format.format_name));
      format_obj.Set("formatLongName", Napi::String::New(env, metadata.format.format_long_name));
      format_obj.Set("durationSec", Napi::Number::New(env, metadata.format.duration_sec));
      format_obj.Set("sizeBytes", Napi::Number::New(env, metadata.format.size_bytes));
      format_obj.Set("bitrate", Napi::Number::New(env, metadata.format.bitrate));
      format_obj.Set("nbStreams", Napi::Number::New(env, metadata.format.nb_streams));

      // Metadata tags
      Napi::Object metadata_obj = Napi::Object::New(env);
      for (const auto& pair : metadata.format.metadata) {
        metadata_obj.Set(pair.first, Napi::String::New(env, pair.second));
      }
      format_obj.Set("metadata", metadata_obj);

      result.Set("format", format_obj);

      // Video streams
      Napi::Array video_streams = Napi::Array::New(env, metadata.video_streams.size());
      for (size_t i = 0; i < metadata.video_streams.size(); i++) {
        const auto& vs = metadata.video_streams[i];
        Napi::Object vs_obj = Napi::Object::New(env);
        vs_obj.Set("codecName", Napi::String::New(env, vs.codec_name));
        vs_obj.Set("codecLongName", Napi::String::New(env, vs.codec_long_name));
        vs_obj.Set("width", Napi::Number::New(env, vs.width));
        vs_obj.Set("height", Napi::Number::New(env, vs.height));
        vs_obj.Set("bitrate", Napi::Number::New(env, vs.bitrate));
        vs_obj.Set("fps", Napi::Number::New(env, vs.fps));
        vs_obj.Set("pixelFormat", Napi::String::New(env, vs.pixel_format));
        vs_obj.Set("nbFrames", Napi::Number::New(env, vs.nb_frames));
        video_streams[i] = vs_obj;
      }
      result.Set("videoStreams", video_streams);

      // Audio streams
      Napi::Array audio_streams = Napi::Array::New(env, metadata.audio_streams.size());
      for (size_t i = 0; i < metadata.audio_streams.size(); i++) {
        const auto& as = metadata.audio_streams[i];
        Napi::Object as_obj = Napi::Object::New(env);
        as_obj.Set("codecName", Napi::String::New(env, as.codec_name));
        as_obj.Set("codecLongName", Napi::String::New(env, as.codec_long_name));
        as_obj.Set("sampleRate", Napi::Number::New(env, as.sample_rate));
        as_obj.Set("channels", Napi::Number::New(env, as.channels));
        as_obj.Set("bitrate", Napi::Number::New(env, as.bitrate));
        as_obj.Set("channelLayout", Napi::String::New(env, as.channel_layout));
        audio_streams[i] = as_obj;
      }
      result.Set("audioStreams", audio_streams);

      return result;
    } catch (const std::exception& e) {
      Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
      return env.Null();
    }
  }

  static Napi::Value IsCodecSupported(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsString()) {
      Napi::TypeError::New(env, "Expected string argument (codec name)").ThrowAsJavaScriptException();
      return env.Null();
    }

    std::string codec_name = info[0].As<Napi::String>().Utf8Value();
    bool supported = MetadataAnalyzer::is_codec_supported(codec_name);

    return Napi::Boolean::New(env, supported);
  }

  std::unique_ptr<MetadataAnalyzer> analyzer_;
};

/**
 * Get version
 */
Napi::String GetVersion(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::String::New(env, "2.0.0");
}

/**
 * Initialize the addon
 */
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "getVersion"),
              Napi::Function::New(env, GetVersion));

  ThumbnailExtractorWrapper::Init(env, exports);
  MetadataAnalyzerWrapper::Init(env, exports);

  return exports;
}

NODE_API_MODULE(video_processor, Init)
