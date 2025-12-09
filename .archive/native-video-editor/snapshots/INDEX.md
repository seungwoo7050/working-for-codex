# 📚 native-video-editor 스냅샷 인덱스

> 각 패치 시점의 독립 빌드 가능한 소스코드 스냅샷

**생성일**: 2025-12-07  
**총 스냅샷 수**: 9개 (주요 마일스톤 기준)  
**총 패치 수**: 46개 (CLONE-GUIDE.md 기준)

## 🎯 스냅샷 목적

이 디렉토리는 CLONE-GUIDE.md의 패치 단위를 기준으로, 각 시점의 **빌드/테스트 가능한 스냅샷 소스코드**를 제공합니다.

- 학습자가 각 단계를 독립적으로 실행하고 테스트할 수 있음
- 점진적 기능 누적 과정을 명확히 이해할 수 있음
- 각 버전 간 차이를 비교 분석할 수 있음

## 🔧 빠른 시작

```bash
# 원하는 스냅샷 디렉토리로 이동
cd snapshots/010_v1.0.0-basic-infrastructure/

# 프론트엔드 실행
cd frontend && npm install && npm run dev

# 백엔드 실행 (새 터미널)
cd backend && npm install && npm run dev

# 브라우저에서 확인
open http://localhost:5173
```

---

## 📋 스냅샷 디렉토리 목록

### Phase 0: Bootstrap

| 디렉토리 | 패치 ID | 이름 | 주요 기능 | 빌드 가능 |
|----------|---------|------|----------|-----------|
| `004_v0.1.0-bootstrap/` | 0.1~0.4 | Bootstrap 완료 | 프로젝트 초기화, Vite+React, Express 설정 | ✅ |

### Phase 1: Core Features

| 디렉토리 | 패치 ID | 이름 | 주요 기능 | 빌드 가능 |
|----------|---------|------|----------|-----------|
| `010_v1.0.0-basic-infrastructure/` | 1.0.1~1.0.6 | Basic Infrastructure | 비디오 업로드/재생, 타임라인 | ✅ |
| `015_v1.1.0-trim-split/` | 1.1.1~1.1.5 | Trim & Split | 비디오 트림/분할 편집 | ✅ |
| `019_v1.2.0-subtitle-speed/` | 1.2.1~1.2.4 | Subtitle & Speed | 자막 추가, 속도 조절 | ✅ |
| `026_v1.3.0-websocket-persistence/` | 1.3.1~1.3.7 | WebSocket & DB | 실시간 진행률, PostgreSQL 저장 | ✅* |

### Phase 2: Native Performance

| 디렉토리 | 패치 ID | 이름 | 주요 기능 | 빌드 가능 |
|----------|---------|------|----------|-----------|
| `031_v2.0.0-native-addon-setup/` | 2.0.1~2.0.5 | Native Addon 기초 | N-API, RAII, 메모리 풀 | ✅** |
| `036_v2.1.0-thumbnail-extraction/` | 2.1.1~2.1.5 | 썸네일 추출 | FFmpeg C API 썸네일 | ✅** |
| `039_v2.2.0-metadata-analysis/` | 2.2.1~2.2.3 | 메타데이터 분석 | 50배 빠른 메타데이터 추출 | ✅** |
| `042_v2.3.0-performance-monitoring/` | 2.3.1~2.3.3 | 성능 모니터링 | Prometheus + Grafana | ✅ |

### Phase 3: Production

| 디렉토리 | 패치 ID | 이름 | 주요 기능 | 빌드 가능 |
|----------|---------|------|----------|-----------|
| (최종 상태) | 3.0.1~3.0.4 | Production Deployment | Docker 전체 스택 | ✅ |

> **참고**: 마지막 패치(v3.0.0)는 현재 레포지토리의 최종 상태이므로 스냅샷을 별도로 생성하지 않습니다.

### Phase 3 Extended: WebGL & WebAudio

| 디렉토리 | 패치 ID | 이름 | 주요 기능 | 빌드 가능 |
|----------|---------|------|----------|-----------|
| (최종 상태) | 3.1.1~3.1.4 | WebGL Core Engine | WebGL2 컨텍스트, 셰이더, 텍스처 | ✅ |
| (최종 상태) | 3.1.5~3.1.8 | WebGL Shader System | 셰이더 컴파일, 유니폼, 캐싱 | ✅ |
| (최종 상태) | 3.1.9~3.1.12 | WebGL Texture Management | 비디오 텍스처, 필터링, 밉맵 | ✅ |
| (최종 상태) | 3.1.13~3.1.16 | WebGL Rendering Pipeline | 프레임버퍼, 멀티패스, 포스트프로세싱 | ✅ |
| (최종 상태) | 3.1.17~3.1.20 | WebGL Performance | 상태 캐싱, 배치 렌더링, 프로파일링 | ✅ |
| (최종 상태) | 3.2.1~3.2.4 | WebAudio Core Engine | AudioContext, 로더, 버퍼 관리 | ✅ |
| (최종 상태) | 3.2.5~3.2.8 | Audio Node System | 노드 그래프, 커스텀 노드, 파라미터 | ✅ |
| (최종 상태) | 3.2.9~3.2.12 | Realtime Audio Processing | ScriptProcessor, AudioWorklet, FFT | ✅ |
| (최종 상태) | 3.2.13~3.2.16 | Audio Visualization | 파형, 스펙트로그램, 실시간 시각화 | ✅ |
| (최종 상태) | 3.2.17~3.2.20 | WebAudio Performance | 버퍼 풀, 노드 풀, 메모리 관리 | ✅ |

> **참고**: Phase 3 Extended (v3.1.x~v3.2.x)는 현재 레포지토리의 최종 상태에 포함되어 있습니다.

---

## 🔧 빌드 요구사항

### 기본 요구사항 (모든 스냅샷)
- Node.js 20.x
- npm 9.x 이상

### 테스트 프레임워크
- **Backend**: Jest (ESM 지원, `NODE_OPTIONS=--experimental-vm-modules`)
- **Frontend**: Vitest (jsdom 환경)
- **Native**: Node.js 내장 테스트

### Phase 1 추가 요구사항 (v1.1.0 이후)
- FFmpeg 설치 (`brew install ffmpeg` 또는 `apt install ffmpeg`)

### Phase 1.3 추가 요구사항 (v1.3.0)
- PostgreSQL 15 (`* 표시`)
- Redis 7

### Phase 2 추가 요구사항 (v2.0.0 이후)
- FFmpeg 개발 라이브러리 (`** 표시`)
  - macOS: `brew install ffmpeg`
  - Linux: `apt install libavformat-dev libavcodec-dev libavutil-dev libswscale-dev`
- C++ 컴파일러 (C++17 지원)
- node-gyp

---

## 🚀 스냅샷 실행 방법

### 1. 스냅샷 디렉토리로 이동
```bash
cd snapshots/010_v1.0.0-basic-infrastructure/
```

### 2. 의존성 설치
```bash
# Frontend
cd frontend && npm install

# Backend
cd ../backend && npm install
```

### 3. 개발 서버 실행
```bash
# Terminal 1: Frontend
cd frontend && npm run dev

# Terminal 2: Backend
cd backend && npm run dev
```

### 4. 브라우저에서 확인
- Frontend: http://localhost:5173
- Backend API: http://localhost:3001/health

---

## 📊 스냅샷 상세 정보

### 004_v0.1.0-bootstrap

**포함 기능**:
- 모노레포 디렉토리 구조
- Vite + React 18 + TypeScript 5 프론트엔드
- Express + TypeScript 백엔드
- TailwindCSS 스타일링

**핵심 파일**:
```
frontend/
├── package.json
├── vite.config.ts
├── tsconfig.json
└── src/
    ├── App.tsx (Hello World)
    └── main.tsx

backend/
├── package.json
├── tsconfig.json
└── src/
    └── server.ts (기본 Express)
```

---

### 010_v1.0.0-basic-infrastructure

**이전 버전 대비 추가**:
- VideoUpload 컴포넌트 (드래그앤드롭)
- useVideoUpload 훅
- multer 파일 업로드 라우트
- VideoPlayer 컴포넌트
- Timeline Canvas 컴포넌트
- 컴포넌트 통합

**핵심 파일**:
```
frontend/src/
├── components/
│   ├── VideoUpload.tsx
│   ├── VideoPlayer.tsx
│   └── Timeline.tsx
├── hooks/
│   └── useVideoUpload.ts
└── App.tsx (통합)

backend/src/
├── routes/
│   └── upload.routes.ts
└── server.ts
```

---

### 015_v1.1.0-trim-split

**이전 버전 대비 추가**:
- StorageService (파일 관리)
- FFmpegService (trim/split)
- edit.routes.ts (편집 API)
- EditPanel 컴포넌트
- useVideoEdit 훅

**핵심 파일**:
```
backend/src/
├── services/
│   ├── storage.service.ts
│   └── ffmpeg.service.ts
├── routes/
│   └── edit.routes.ts

frontend/src/
├── components/
│   └── EditPanel.tsx
├── hooks/
│   └── useVideoEdit.ts
```

---

### 019_v1.2.0-subtitle-speed

**이전 버전 대비 추가**:
- SRT 자막 생성 로직
- FFmpeg 필터 체인 (setpts, atempo, subtitles)
- SubtitleEditor 컴포넌트
- ControlPanel 속도 조절 UI

**핵심 파일**:
```
backend/src/services/
└── ffmpeg.service.ts (확장: 자막+속도)

frontend/src/components/
├── SubtitleEditor.tsx
└── ControlPanel.tsx
```

---

### 026_v1.3.0-websocket-persistence

**이전 버전 대비 추가**:
- WebSocketService (ws 라이브러리)
- useWebSocket 훅
- PostgreSQL 마이그레이션
- DatabaseService
- Redis 캐싱 서비스
- project.routes.ts
- ProjectPanel 컴포넌트

**핵심 파일**:
```
backend/src/
├── ws/
│   └── websocket.service.ts
├── db/
│   ├── database.service.ts
│   └── redis.service.ts
├── routes/
│   └── project.routes.ts

frontend/src/
├── hooks/
│   ├── useWebSocket.ts
│   └── useProjects.ts
├── components/
│   └── ProjectPanel.tsx

migrations/
└── 001_initial_schema.sql
```

---

### 031_v2.0.0-native-addon-setup

**이전 버전 대비 추가**:
- binding.gyp (node-gyp 설정)
- ffmpeg_raii.h (RAII 래퍼)
- memory_pool.h/cpp (메모리 풀)
- video_processor.cpp (N-API 스켈레톤)

**핵심 파일**:
```
native/
├── binding.gyp
├── package.json
├── include/
│   ├── ffmpeg_raii.h
│   └── memory_pool.h
└── src/
    ├── memory_pool.cpp
    └── video_processor.cpp
```

---

### 036_v2.1.0-thumbnail-extraction

**이전 버전 대비 추가**:
- ThumbnailExtractor 클래스
- thumbnail.ts 라우트

**핵심 파일**:
```
native/
├── include/
│   └── thumbnail_extractor.h
└── src/
    └── thumbnail_extractor.cpp

backend/src/routes/
└── thumbnail.ts
```

---

### 039_v2.2.0-metadata-analysis

**이전 버전 대비 추가**:
- MetadataAnalyzer 클래스
- metadata.ts 라우트 (Redis 캐싱)

**핵심 파일**:
```
native/
├── include/
│   └── metadata_analyzer.h
└── src/
    └── metadata_analyzer.cpp

backend/src/routes/
└── metadata.ts
```

---

### 042_v2.3.0-performance-monitoring

**이전 버전 대비 추가**:
- MetricsService (prom-client)
- /metrics 엔드포인트
- Grafana 대시보드

**핵심 파일**:
```
backend/src/
├── services/
│   └── metrics.service.ts
├── routes/
│   └── metrics.ts

monitoring/
├── prometheus/
│   └── prometheus.yml
└── grafana/
    ├── provisioning/
    └── dashboards/
```

---

## ⚠️ 제한사항 및 참고사항

### 빌드 실패 가능 케이스

1. **v1.3.0 이후**: PostgreSQL/Redis 없이 실행 시 DB 연결 에러
   - 해결: Docker로 DB 실행 또는 환경 변수로 비활성화

2. **v2.0.0 이후**: FFmpeg 개발 라이브러리 없이 빌드 실패
   - 해결: 시스템에 FFmpeg 개발 라이브러리 설치

3. **Native Addon**: macOS/Linux만 지원, Windows 미지원
   - 해결: WSL2 또는 Docker 사용

### 스냅샷 간 차이 확인

```bash
# 두 스냅샷 간 차이 비교
diff -rq snapshots/010_v1.0.0-basic-infrastructure/ snapshots/015_v1.1.0-trim-split/
```

---

## 📖 관련 문서

- [CLONE-GUIDE.md](../CLONE-GUIDE.md) - 전체 패치 타임라인
- [design/](../design/) - 각 버전별 상세 설계 문서
- [README.md](../README.md) - 프로젝트 개요
