# Native Video Editor — 클론코딩 가이드

> 버전별 커밋 가이드 (주니어-미드레벨 현업 개발자 기준 현실적 타임라인)

---

## 커밋 방법

아래 명령어를 그대로 복사하여 터미널에 붙여넣으면 됩니다.  
`GIT_AUTHOR_DATE`와 `GIT_COMMITTER_DATE`가 동시에 설정됩니다.

---

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **개발 기간** | 2025-01-01 ~ 2025-04-25 (약 16주) |
| **난이도** | 중상 (풀스택 + C++ Native + GPU) |
| **작업 페이스** | 주 5일, 일 1커밋 기준 (복잡 기능 2-4일) |

---

## 레퍼런스

### 공식 문서
- [React Documentation](https://react.dev/)
- [Vite Guide](https://vitejs.dev/guide/)
- [Node.js N-API](https://nodejs.org/api/n-api.html)
- [node-addon-api](https://github.com/nodejs/node-addon-api)
- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [WebGL2 Fundamentals](https://webgl2fundamentals.org/)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)

### N-API / Native Addon
- [node-addon-examples](https://github.com/nodejs/node-addon-examples) — 공식 예제
- [FFmpeg libav Tutorial](https://github.com/leandromoreira/ffmpeg-libav-tutorial) — 필독
- [napi-rs](https://github.com/napi-rs/napi-rs) — Rust 버전 참고

### 미디어 처리
- [FFmpeg Wiki](https://trac.ffmpeg.org/wiki)
- [fluent-ffmpeg](https://github.com/fluent-ffmpeg/node-fluent-ffmpeg)
- [Video.js](https://github.com/videojs/video.js)

### WebGL/WebAudio
- [Three.js](https://github.com/mrdoob/three.js) — WebGL 참고
- [Tone.js](https://github.com/Tonejs/Tone.js) — WebAudio 참고
- [WebGL Best Practices](https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API/WebGL_best_practices)

### 유사 프로젝트
- [Remotion](https://github.com/remotion-dev/remotion) — React 비디오 에디터
- [FFmpeg.wasm](https://github.com/ffmpegwasm/ffmpeg.wasm) — 브라우저 FFmpeg

---

## Phase 1: 기초 인프라

### v0.1.0 — Bootstrap

```bash
GIT_AUTHOR_DATE="2024-07-01 20:30:00" GIT_COMMITTER_DATE="2024-07-01 20:30:00" git commit -m "feat: initialize frontend with Vite + React 18"
```

```bash
GIT_AUTHOR_DATE="2024-07-02 21:00:00" GIT_COMMITTER_DATE="2024-07-02 21:00:00" git commit -m "feat: setup backend with Express + TypeScript"
```

```bash
GIT_AUTHOR_DATE="2024-07-03 19:45:00" GIT_COMMITTER_DATE="2024-07-03 19:45:00" git commit -m "chore: add GitHub Actions CI pipeline"
```

```bash
GIT_AUTHOR_DATE="2024-07-04 20:15:00" GIT_COMMITTER_DATE="2024-07-04 20:15:00" git commit -m "chore: add ESLint + Prettier config"
```

---

### v1.0.0 — Basic Infrastructure

```bash
GIT_AUTHOR_DATE="2024-07-08 20:00:00" GIT_COMMITTER_DATE="2024-07-08 20:00:00" git commit -m "feat: implement multipart video upload"
```

```bash
GIT_AUTHOR_DATE="2024-07-09 21:30:00" GIT_COMMITTER_DATE="2024-07-09 21:30:00" git commit -m "feat: add HTML5 video player component"
```

```bash
GIT_AUTHOR_DATE="2024-07-11 19:30:00" GIT_COMMITTER_DATE="2024-07-11 19:30:00" git commit -m "feat: implement Canvas timeline UI"
```

```bash
GIT_AUTHOR_DATE="2024-07-12 20:45:00" GIT_COMMITTER_DATE="2024-07-12 20:45:00" git commit -m "feat: add waveform visualization"
```

---

### v1.1.0 — Trim/Split

```bash
GIT_AUTHOR_DATE="2024-07-15 19:15:00" GIT_COMMITTER_DATE="2024-07-15 19:15:00" git commit -m "feat: add video trim endpoint"
```

```bash
GIT_AUTHOR_DATE="2024-07-17 21:00:00" GIT_COMMITTER_DATE="2024-07-17 21:00:00" git commit -m "feat: implement split with codec copy"
```

```bash
GIT_AUTHOR_DATE="2024-07-18 20:30:00" GIT_COMMITTER_DATE="2024-07-18 20:30:00" git commit -m "test: add trim/split unit tests"
```

---

### v1.2.0 — Subtitle/Speed

```bash
GIT_AUTHOR_DATE="2024-07-22 19:45:00" GIT_COMMITTER_DATE="2024-07-22 19:45:00" git commit -m "feat: add SRT subtitle burn-in"
```

```bash
GIT_AUTHOR_DATE="2024-07-24 20:00:00" GIT_COMMITTER_DATE="2024-07-24 20:00:00" git commit -m "feat: implement speed control with atempo"
```

```bash
GIT_AUTHOR_DATE="2024-07-25 19:30:00" GIT_COMMITTER_DATE="2024-07-25 19:30:00" git commit -m "feat: add re-encoding pipeline"
```

---

### v1.3.0 — WebSocket/Persistence

```bash
GIT_AUTHOR_DATE="2024-07-29 20:15:00" GIT_COMMITTER_DATE="2024-07-29 20:15:00" git commit -m "feat: add WebSocket progress updates"
```

```bash
GIT_AUTHOR_DATE="2024-07-31 21:00:00" GIT_COMMITTER_DATE="2024-07-31 21:00:00" git commit -m "feat: implement PostgreSQL persistence"
```

```bash
GIT_AUTHOR_DATE="2024-08-01 19:15:00" GIT_COMMITTER_DATE="2024-08-01 19:15:00" git commit -m "feat: add Redis caching layer"
```

```bash
GIT_AUTHOR_DATE="2024-08-02 20:45:00" GIT_COMMITTER_DATE="2024-08-02 20:45:00" git commit -m "feat: implement auto-save with debounce"
```

---

## Phase 2: 네이티브 성능

### v2.0.0 — Native Addon Setup

```bash
GIT_AUTHOR_DATE="2024-08-05 19:30:00" GIT_COMMITTER_DATE="2024-08-05 19:30:00" git commit -m "feat: setup N-API binding structure"
```

```bash
GIT_AUTHOR_DATE="2024-08-08 21:00:00" GIT_COMMITTER_DATE="2024-08-08 21:00:00" git commit -m "feat: integrate FFmpeg C API"
```

```bash
GIT_AUTHOR_DATE="2024-08-12 19:45:00" GIT_COMMITTER_DATE="2024-08-12 19:45:00" git commit -m "feat: implement RAII wrappers for AVFormat"
```

```bash
GIT_AUTHOR_DATE="2024-08-14 20:30:00" GIT_COMMITTER_DATE="2024-08-14 20:30:00" git commit -m "feat: add AVFrame memory pool"
```

---

### v2.1.0 — Thumbnail Extraction

```bash
GIT_AUTHOR_DATE="2024-08-19 19:15:00" GIT_COMMITTER_DATE="2024-08-19 19:15:00" git commit -m "feat: implement native thumbnail extraction"
```

```bash
GIT_AUTHOR_DATE="2024-08-21 20:00:00" GIT_COMMITTER_DATE="2024-08-21 20:00:00" git commit -m "feat: add batch thumbnail generation"
```

---

### v2.2.0 — Metadata Analysis

```bash
GIT_AUTHOR_DATE="2024-08-26 19:30:00" GIT_COMMITTER_DATE="2024-08-26 19:30:00" git commit -m "feat: implement native metadata analysis"
```

```bash
GIT_AUTHOR_DATE="2024-08-28 21:00:00" GIT_COMMITTER_DATE="2024-08-28 21:00:00" git commit -m "perf: optimize to 50x faster metadata extraction"
```

---

### v2.3.0 — Performance Monitoring

```bash
GIT_AUTHOR_DATE="2024-09-02 19:45:00" GIT_COMMITTER_DATE="2024-09-02 19:45:00" git commit -m "feat: add Prometheus metrics endpoint"
```

```bash
GIT_AUTHOR_DATE="2024-09-04 20:30:00" GIT_COMMITTER_DATE="2024-09-04 20:30:00" git commit -m "feat: setup Grafana dashboards"
```

---

### v3.0.0 — Production Deployment

```bash
GIT_AUTHOR_DATE="2024-09-09 19:15:00" GIT_COMMITTER_DATE="2024-09-09 19:15:00" git commit -m "feat: add Docker Compose production stack"
```

```bash
GIT_AUTHOR_DATE="2024-09-11 20:45:00" GIT_COMMITTER_DATE="2024-09-11 20:45:00" git commit -m "feat: add healthcheck endpoints"
```

---

## Phase 3: 렌더링 엔진

### v3.1.0 — WebGL Core

```bash
GIT_AUTHOR_DATE="2024-09-16 19:30:00" GIT_COMMITTER_DATE="2024-09-16 19:30:00" git commit -m "feat: initialize WebGL2 context"
```

```bash
GIT_AUTHOR_DATE="2024-09-18 20:00:00" GIT_COMMITTER_DATE="2024-09-18 20:00:00" git commit -m "feat: add context recovery handling"
```

---

### v3.1.1 — Shader System

```bash
GIT_AUTHOR_DATE="2024-09-23 19:15:00" GIT_COMMITTER_DATE="2024-09-23 19:15:00" git commit -m "feat: implement shader compilation system"
```

```bash
GIT_AUTHOR_DATE="2024-09-25 21:00:00" GIT_COMMITTER_DATE="2024-09-25 21:00:00" git commit -m "feat: add effect shaders"
```

---

### v3.1.2 — Texture Management

```bash
GIT_AUTHOR_DATE="2024-09-27 19:45:00" GIT_COMMITTER_DATE="2024-09-27 19:45:00" git commit -m "feat: implement TextureManager"
```

```bash
GIT_AUTHOR_DATE="2024-09-30 20:30:00" GIT_COMMITTER_DATE="2024-09-30 20:30:00" git commit -m "feat: add texture atlas with LRU cache"
```

---

### v3.1.3 — Rendering Pipeline

```bash
GIT_AUTHOR_DATE="2024-10-02 19:15:00" GIT_COMMITTER_DATE="2024-10-02 19:15:00" git commit -m "feat: implement RenderTargetManager"
```

```bash
GIT_AUTHOR_DATE="2024-10-03 20:00:00" GIT_COMMITTER_DATE="2024-10-03 20:00:00" git commit -m "feat: add multi-pass rendering"
```

---

### v3.1.4 — WebGL Performance

```bash
GIT_AUTHOR_DATE="2024-10-07 19:30:00" GIT_COMMITTER_DATE="2024-10-07 19:30:00" git commit -m "perf: implement GPU frame pooling"
```

```bash
GIT_AUTHOR_DATE="2024-10-08 21:00:00" GIT_COMMITTER_DATE="2024-10-08 21:00:00" git commit -m "perf: add WebGL state batching"
```

---

### v3.2.0 — WebAudio Core

```bash
GIT_AUTHOR_DATE="2024-10-09 19:45:00" GIT_COMMITTER_DATE="2024-10-09 19:45:00" git commit -m "feat: initialize WebAudio context"
```

```bash
GIT_AUTHOR_DATE="2024-10-10 20:30:00" GIT_COMMITTER_DATE="2024-10-10 20:30:00" git commit -m "feat: add master gain control"
```

---

### v3.2.1 — Audio Node System

```bash
GIT_AUTHOR_DATE="2024-10-11 19:15:00" GIT_COMMITTER_DATE="2024-10-11 19:15:00" git commit -m "feat: implement audio node chain"
```

```bash
GIT_AUTHOR_DATE="2024-10-14 20:45:00" GIT_COMMITTER_DATE="2024-10-14 20:45:00" git commit -m "feat: add audio effects nodes"
```

---

### v3.2.2 — Realtime Processing

```bash
GIT_AUTHOR_DATE="2024-10-15 19:30:00" GIT_COMMITTER_DATE="2024-10-15 19:30:00" git commit -m "feat: implement AudioWorklet processor"
```

```bash
GIT_AUTHOR_DATE="2024-10-16 21:00:00" GIT_COMMITTER_DATE="2024-10-16 21:00:00" git commit -m "feat: add video-audio sync"
```

---

### v3.2.3 — Audio Visualization

```bash
GIT_AUTHOR_DATE="2024-10-17 19:45:00" GIT_COMMITTER_DATE="2024-10-17 19:45:00" git commit -m "feat: implement FFT visualization"
```

```bash
GIT_AUTHOR_DATE="2024-10-18 20:00:00" GIT_COMMITTER_DATE="2024-10-18 20:00:00" git commit -m "feat: add waveform canvas renderer"
```

---

### v3.2.4 — WebAudio Performance

```bash
GIT_AUTHOR_DATE="2024-10-21 19:15:00" GIT_COMMITTER_DATE="2024-10-21 19:15:00" git commit -m "perf: implement AudioBuffer pooling"
```

```bash
GIT_AUTHOR_DATE="2024-10-22 20:30:00" GIT_COMMITTER_DATE="2024-10-22 20:30:00" git commit -m "perf: optimize audio scheduling"
```

---

## 타임라인 요약

| 구간 | 버전 | 기간 | 커밋 수 | 비고 |
|------|------|------|--------|------|
| Phase 1 | v0.1.0 ~ v1.3.0 | 1/1 ~ 2/7 | 18 | 웹 기초 |
| Phase 2 | v2.0.0 ~ v3.0.0 | 2/10 ~ 3/14 | 12 | N-API |
| Phase 3 | v3.1.0 ~ v3.2.4 | 3/17 ~ 4/25 | 20 | WebGL/Audio |
| **합계** | | **약 16주** | **50** | |

---

## 버전별 완료 체크리스트

### Phase 1 완료 (v1.3.0)
- [ ] 비디오 업로드 성공 (100MB 청크)
- [ ] 타임라인 UI에서 스크러빙 동작
- [ ] Trim/Split 작업 정상
- [ ] 자막 번인 적용 확인
- [ ] WebSocket 진행률 실시간 업데이트
- [ ] PostgreSQL에 프로젝트 저장

### Phase 2 완료 (v3.0.0)
- [ ] 네이티브 애드온 빌드 성공
- [ ] 썸네일 추출 p99 < 50ms
- [ ] 메타데이터 분석 < 5ms (fluent-ffmpeg 대비 50배)
- [ ] Grafana 대시보드 메트릭 확인
- [ ] Docker Compose 6 서비스 정상 실행

### Phase 3 완료 (v3.2.4)
- [ ] WebGL 렌더링 60 FPS 유지
- [ ] 이펙트 셰이더 (밝기/대비/채도) 적용
- [ ] 오디오 이펙트 실시간 반영
- [ ] FFT 시각화 동작
- [ ] 비디오-오디오 동기화 정확

---

## 주요 구현 팁

### N-API 빌드
```bash
# node-gyp 설치
npm install -g node-gyp

# 네이티브 빌드
cd native
node-gyp configure
node-gyp build

# 빌드 산출물
# build/Release/video_native.node
```

### FFmpeg C API 사용
```cpp
// native/src/metadata/extractor.cc
Napi::Object ExtractMetadata(const Napi::CallbackInfo& info) {
  AVFormatContext* fmt_ctx = avformat_alloc_context();
  avformat_open_input(&fmt_ctx, filename.c_str(), nullptr, nullptr);
  avformat_find_stream_info(fmt_ctx, nullptr);
  
  // duration, codec, resolution 추출
  Napi::Object result = Napi::Object::New(env);
  result.Set("duration", fmt_ctx->duration / AV_TIME_BASE);
  return result;
}
```

### WebGL 텍스처 업로드
```typescript
// frontend/src/utils/webgl.ts
function uploadTexture(gl: WebGL2RenderingContext, video: HTMLVideoElement) {
  const texture = gl.createTexture();
  gl.bindTexture(gl.TEXTURE_2D, texture);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, video);
  return texture;
}
```
