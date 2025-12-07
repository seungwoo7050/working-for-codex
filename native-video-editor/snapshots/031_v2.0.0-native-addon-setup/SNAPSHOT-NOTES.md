# 031_v2.0.0-native-addon-setup 스냅샷

## 📋 패치 정보

| 항목 | 값 |
|------|-----|
| 패치 범위 | 2.0.1 ~ 2.0.5 |
| 버전 태그 | v2.0.0 |
| 이름 | Native Addon Setup |
| 이전 단계 | v1.3.0 (WebSocket & Persistence) |

## 🎯 이 스냅샷의 목표

C++ Native Addon 인프라를 구축합니다. 이 단계에서 JavaScript와 C++를 연결하는 N-API 바인딩 기초를 설정합니다.

### 추가된 기능
- **binding.gyp**: node-gyp 빌드 설정
- **ffmpeg_raii.h**: FFmpeg 리소스 RAII 래퍼
- **memory_pool.h/cpp**: AVFrame 재사용 메모리 풀
- **video_processor.cpp**: N-API 바인딩 스켈레톤

## 📁 포함되어야 할 파일

### v1.3.0에서 상속
- 모든 v1.3.0 파일들

### 신규 추가 파일
```
native/
├── binding.gyp            # node-gyp 빌드 설정
├── package.json           # native 모듈 패키지
├── include/
│   ├── ffmpeg_raii.h      # RAII 래퍼
│   └── memory_pool.h      # 메모리 풀 헤더
└── src/
    ├── memory_pool.cpp    # 메모리 풀 구현
    └── video_processor.cpp # N-API 바인딩
```

## 🔧 추가 요구사항

- **FFmpeg 개발 라이브러리**
  - macOS: `brew install ffmpeg`
  - Linux: `apt install libavformat-dev libavcodec-dev libavutil-dev libswscale-dev`
- **C++ 컴파일러** (C++17 지원)
- **node-gyp**: `npm install -g node-gyp`

## 🚀 빌드 및 실행

```bash
# Native 모듈 빌드
cd native
npm install
npm run build

# 로드 테스트
node -e "const addon = require('./build/Release/video_processor'); console.log(addon);"

# Backend 실행 (native 모듈 연동)
cd ../backend && npm install && npm run dev
```

## ✅ 테스트

1. `npm run build` → 컴파일 성공
2. `require('video_processor')` → 모듈 로드 성공
3. Valgrind 메모리 누수 0 (Linux)

## 📝 완료 기준

- [x] node-gyp 빌드 성공
- [x] N-API 모듈 로드 성공
- [x] RAII 래퍼 컴파일 성공
- [x] 메모리 풀 동작 확인

## 📝 다음 단계

→ `036_v2.1.0-thumbnail-extraction/`: 썸네일 추출 기능
