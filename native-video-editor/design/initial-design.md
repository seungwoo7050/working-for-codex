# native-video-editor — 웹 기반 동영상 편집기 초기 설계서
> 백지 상태에서 시작하는 프로덕션급 비디오 에디터 프로젝트

---

## 1. 프로젝트 비전 & 동기

### 1.1 왜 이 프로젝트인가?

**목표 회사: Voyager X (Vrew)**

Voyager X 웹 애플리케이션 개발자 채용 요구사항:
```
- "C++ 혹은 JavaScript에 대한 이해가 깊음"
- "필요에 따라서 더욱 저수준으로 내려갈 수 있음"
- "동영상 관련 기술에 대해 관심이 많음"
- React, Node.js, TypeScript, FFmpeg, WebGL
```

**전략적 접근:**
```
┌─────────────────────────────────────────────────────────────────┐
│                    native-video-editor 증명 포인트                        │
├─────────────────────────────────────────────────────────────────┤
│  • 웹 개발 숙련도  ────► React 18 + TypeScript + Vite           │
│  • 깊은 C++ 이해   ────► FFmpeg C API + N-API Native Addon      │
│  • 저수준 접근력   ────► 메모리 관리, RAII, 성능 최적화         │
│  • 동영상 기술     ────► 트림/분할, 썸네일, 인코딩/디코딩       │
│  • 실전 인프라     ────► PostgreSQL, Redis, Prometheus          │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 프로젝트 목표

```
"C++과 웹 기술을 결합한 프로덕션급 비디오 편집기를 구축하여
 Voyager X의 기술 요구사항과 100% 매칭되는 역량을 증명한다"
```

**달성 기준:**
1. React + Node.js 기반 비디오 업로드/미리보기 → **Phase 1**
2. 타임라인 에디터 + 트림/분할 기능 → **Phase 1**
3. C++ Native Addon (FFmpeg C API 직접 사용) → **Phase 2**
4. 성능 모니터링 (Prometheus + Grafana) → **Phase 2**
5. Docker 배포 + 프로덕션 문서화 → **Phase 3.0**
6. WebGL 기반 고성능 비디오 렌더링 엔진 → **Phase 3.1**
7. WebAudio 기반 실시간 오디오 처리 엔진 → **Phase 3.2**

---

## 2. 설계 철학

### 2.1 핵심 원칙

#### 원칙 1: 하이브리드 아키텍처
```
┌─────────────────────────────────────────────────────────────────┐
│                    Layer Architecture                           │
├─────────────────────────────────────────────────────────────────┤
│  Frontend    │ React 18 + TypeScript + Vite                     │
│  (빠른 개발) │ Canvas/WebGL 기반 타임라인                       │
├──────────────┼──────────────────────────────────────────────────┤
│  Backend     │ Node.js + Express + TypeScript                   │
│  (API 계층)  │ fluent-ffmpeg → C++ Native Addon 전환            │
├──────────────┼──────────────────────────────────────────────────┤
│  Native      │ C++17 + N-API + FFmpeg C API                     │
│  (고성능)    │ 썸네일 추출, 메타데이터 분석, 트랜스코딩         │
└──────────────┴──────────────────────────────────────────────────┘
```

#### 원칙 2: 점진적 성능 개선
```
Phase 1: fluent-ffmpeg (래퍼)
    ↓ 기능 동작 확인
Phase 2: C++ Native Addon
    ↓ 성능 10배 이상 개선
Phase 3: 프로덕션 배포
```

- Phase 1에서 빠르게 기능 구현
- Phase 2에서 성능 크리티컬 부분을 C++로 교체
- 기능과 성능 모두 확보

#### 원칙 3: 증거 기반 개발
```
모든 주장에는 증거가 필요:
- 성능 주장 → 벤치마크 결과
- 품질 주장 → 테스트 커버리지
- 안정성 주장 → 모니터링 대시보드
```

### 2.2 기술 스택 결정 근거

| 기술 | 선택 이유 | Voyager X 매칭 |
|------|-----------|----------------|
| **React 18** | Vrew 기술 스택, 모던 훅 | ✓ 필수 |
| **TypeScript 5** | 타입 안전, 대규모 코드베이스 | ✓ 필수 |
| **Node.js 20** | V8 성능, N-API 지원 | ✓ 필수 |
| **C++17** | FFmpeg API, 성능 크리티컬 | ✓ 핵심 |
| **FFmpeg** | 업계 표준 비디오 처리 | ✓ 필수 |
| **PostgreSQL** | 안정성, JSON 지원 | 실무 역량 |
| **Redis** | 캐싱, 세션 관리 | 실무 역량 |
| **Prometheus** | 메트릭 수집, 알림 | 실무 역량 |

---

## 3. Phase 로드맵

### 3.1 전체 로드맵 개요

```
┌─────────────────────────────────────────────────────────────────┐
│                     native-video-editor 로드맵                            │
├─────────┬───────────────────────────────────────────────────────┤
│ Phase 0 │ Bootstrap                                             │
│         │ - 프로젝트 구조 생성                                  │
│         │ - React + Vite 프론트엔드 설정                        │
│         │ - Node.js + Express 백엔드 설정                       │
│         │ - TypeScript 설정                                     │
├─────────┼───────────────────────────────────────────────────────┤
│ Phase 1 │ 편집 기능 (Quick Win)                                 │
│         │                                                       │
│  1.0    │ - 비디오 업로드 (드래그 앤 드롭, multipart)           │
│         │ - 비디오 미리보기 (HTML5 Video)                       │
│         │ - Canvas 기반 타임라인                                │
│  1.1    │ - 트림/분할 기능 (fluent-ffmpeg)                      │
│         │ - 내보내기 기능                                       │
│  1.2    │ - 자막 오버레이                                       │
│         │ - 재생 속도 조절                                      │
│  1.3    │ - WebSocket 진행률 알림                               │
│         │ - PostgreSQL 프로젝트 저장                            │
├─────────┼───────────────────────────────────────────────────────┤
│ Phase 2 │ C++ 성능 최적화 (Deep Tech)                           │
│         │                                                       │
│  2.0    │ - N-API 네이티브 애드온 기초                          │
│         │ - FFmpeg 라이브러리 연동                              │
│  2.1    │ - 고성능 썸네일 추출 (<50ms/프레임)                   │
│         │ - 메모리 풀 관리                                      │
│  2.2    │ - 메타데이터 분석                                     │
│         │ - 코덱 정보, 해상도, 프레임레이트                     │
│  2.3    │ - Prometheus 메트릭                                   │
│         │ - Grafana 대시보드                                    │
├─────────┼───────────────────────────────────────────────────────┤
│ Phase 3 │ 프로덕션 완성도 & 고급 렌더링                         │
│         │                                                       │
│  3.0    │ - Docker Compose 배포                                 │
│         │ - 포괄적 문서화                                       │
│         │ - 성능 벤치마크 보고서                                │
│  3.1    │ - WebGL 코어 엔진                                     │
│         │ - 셰이더 시스템                                       │
│         │ - 텍스처 관리 & 렌더링 파이프라인                     │
│         │ - WebGL 성능 최적화                                   │
│  3.2    │ - WebAudio 코어 엔진                                  │
│         │ - 오디오 노드 시스템                                  │
│         │ - 실시간 오디오 처리                                  │
│         │ - 오디오 시각화 & 성능 최적화                         │
└─────────┴───────────────────────────────────────────────────────┘
```

### 3.2 핵심 성과 지표 (KPI)

**반드시 달성해야 할 모든 지표:**

| 영역 | 지표 | 목표값 |
|------|------|--------|
| **프론트엔드** | 타임라인 렌더링 | **60 FPS** |
| **업로드** | 100MB 비디오 업로드 | **p99 < 5초** |
| **C++ 썸네일** | 프레임당 추출 시간 | **p99 < 50ms** |
| **편집** | 1분 비디오 트림/분할 | **< 3초** |
| **WebSocket** | 지연 시간 | **< 100ms** |
| **API** | 응답 시간 | **p99 < 200ms** |
| **메모리** | 누수 | **0개** (valgrind + Chrome) |
| **테스트** | 커버리지 | **≥ 70%** |

---

## 4. 아키텍처 스케치

### 4.1 시스템 아키텍처

```
┌─────────────────────────────────────────────────────────────────┐
│                         Browser                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                    React Application                     │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │   │
│  │  │ VideoPlayer │  │  Timeline   │  │ ControlPanel│       │   │
│  │  │ (HTML5)     │  │  (Canvas)   │  │             │       │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘       │   │
│  ├──────────────────────────────────────────────────────────┤   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │   │
│  │  │ useUpload   │  │ useTimeline │  │ useWebSocket│       │   │
│  │  │ Hook        │  │ Hook        │  │ Hook        │       │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘       │   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────┬───────────────────────────────────────┘
                          │ HTTP / WebSocket
                          ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Node.js Server                             │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                     Express Routes                       │   │
│  │  /api/upload   /api/projects   /api/edit   /api/export   │   │
│  └──────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                       Services                           │   │
│  │  FFmpegService   StorageService   ProjectService         │   │
│  └──────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                   Native Addon (C++)                     │   │
│  │ video_processor   thumbnail_extractor   metadata_analyzer│   │
│  └──────────────────────────────────────────────────────────┘   │
└─────────────────────────┬───────────────────────────────────────┘
                          │
        ┌─────────────────┼─────────────────┬─────────────────┐
        ▼                 ▼                 ▼                 ▼
┌───────────────┐ ┌───────────────┐ ┌───────────────┐ ┌───────────────┐
│  PostgreSQL   │ │     Redis     │ │  File Storage │ │  Prometheus   │
│  (Projects)   │ │   (Session)   │ │   (Videos)    │ │   (Metrics)   │
└───────────────┘ └───────────────┘ └───────────────┘ └───────────────┘
```

### 4.2 C++ Native Addon 아키텍처

```
┌─────────────────────────────────────────────────────────────────┐
│                    Native Addon Structure                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                     N-API Layer                         │    │
│  │  (JavaScript ↔ C++ 바인딩)                              │    │
│  │  - napi_value 변환                                      │    │
│  │  - 비동기 작업 관리                                     │    │
│  │  - 에러 처리                                            │    │
│  └─────────────────────────────────────────────────────────┘    │
│                              │                                  │
│  ┌───────────────┬───────────┴───────────┬───────────────┐      │
│  ▼               ▼                       ▼               ▼      │
│  ┌─────────┐  ┌─────────┐  ┌─────────────────┐  ┌─────────┐     │
│  │Thumbnail│  │Metadata │  │ VideoProcessor  │  │ Memory  │     │
│  │Extractor│  │Analyzer │  │ (트랜스코딩)    │  │  Pool   │     │
│  └────┬────┘  └────┬────┘  └────────┬────────┘  └────┬────┘     │
│       │            │                │                │          │
│  └────┴────────────┴────────────────┴────────────────┘          │
│                              │                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                   FFmpeg C API Layer                    │    │
│  │  libavformat  libavcodec  libavutil  libswscale         │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 4.3 디렉토리 구조

```
native-video-editor/
├── .github/
│   └── workflows/
│       └── ci.yml                  # GitHub Actions CI
├── frontend/                       # React + Vite
│   ├── src/
│   │   ├── components/
│   │   │   ├── VideoPlayer.tsx     # 비디오 재생
│   │   │   ├── Timeline.tsx        # 타임라인 에디터
│   │   │   ├── ControlPanel.tsx    # 컨트롤 패널
│   │   │   ├── VideoUpload.tsx     # 업로드 컴포넌트
│   │   │   ├── EditPanel.tsx       # 편집 패널
│   │   │   ├── ProjectPanel.tsx    # 프로젝트 패널
│   │   │   ├── SubtitleEditor.tsx  # 자막 에디터
│   │   │   └── ProgressBar.tsx     # 진행률 표시
│   │   ├── hooks/
│   │   │   ├── useVideoUpload.ts   # 업로드 훅
│   │   │   ├── useVideoEdit.ts     # 편집 훅
│   │   │   ├── useProjects.ts      # 프로젝트 훅
│   │   │   └── useWebSocket.ts     # WebSocket 훅
│   │   ├── webgl/                  # WebGL 렌더링 엔진 (Phase 3.1)
│   │   ├── webaudio/               # WebAudio 오디오 엔진 (Phase 3.2)
│   │   ├── types/
│   │   │   └── video.ts            # 타입 정의
│   │   ├── App.tsx
│   │   └── main.tsx
│   ├── package.json
│   ├── tsconfig.json
│   └── vite.config.ts
├── backend/                        # Node.js + Express
│   ├── src/
│   │   ├── routes/
│   │   │   ├── upload.ts           # 업로드 API
│   │   │   ├── projects.ts         # 프로젝트 API
│   │   │   └── edit.ts             # 편집 API
│   │   ├── services/
│   │   │   ├── ffmpeg.service.ts   # FFmpeg 래퍼
│   │   │   ├── storage.service.ts  # 파일 저장
│   │   │   ├── metrics.service.ts  # Prometheus 메트릭
│   │   │   └── native-video.service.ts # C++ 네이티브 연동
│   │   ├── db/
│   │   │   ├── database.service.ts # PostgreSQL 직접 연결
│   │   │   └── redis.service.ts    # Redis 서비스
│   │   ├── ws/
│   │   │   └── progress.ts         # WebSocket 진행률
│   │   ├── types/
│   │   │   └── index.ts            # 타입 정의
│   │   └── server.ts
│   ├── package.json
│   └── tsconfig.json
├── native/                         # C++ Native Addon
│   ├── src/
│   │   ├── video_processor.cpp     # 비디오 처리 + N-API 진입점
│   │   ├── thumbnail_extractor.cpp # 썸네일 추출
│   │   ├── metadata_analyzer.cpp   # 메타데이터 분석
│   │   └── memory_pool.cpp         # 메모리 풀
│   ├── include/
│   │   ├── ffmpeg_raii.h           # FFmpeg RAII 래퍼
│   │   ├── thumbnail_extractor.h
│   │   ├── metadata_analyzer.h
│   │   └── memory_pool.h
│   └── binding.gyp                 # node-gyp 빌드 설정
├── design/
│   ├── initial-design.md           # 초기 설계서 (이 문서)
│   ├── ci.md                        # CI/CD 파이프라인 설계
│   ├── v0.1.0-bootstrap.md          # 프로젝트 부트스트랩
│   ├── v1.0.0-basic-infrastructure.md
│   ├── v1.1.0-trim-split.md
│   ├── v1.2.0-subtitle-speed.md
│   ├── v1.3.0-websocket-persistence.md
│   ├── v2.0.0-native-addon-setup.md
│   ├── v2.1.0-thumbnail-extraction.md
│   ├── v2.2.0-metadata-analysis.md
│   ├── v2.3.0-performance-monitoring.md
│   ├── v3.0.0-production-deployment.md
│   ├── v3.1.0-webgl-core-engine.md  # WebGL 엔진 (Phase 3.1)
│   ├── v3.1.1-webgl-shader-system.md
│   ├── v3.1.2-webgl-texture-management.md
│   ├── v3.1.3-webgl-rendering-pipeline.md
│   ├── v3.1.4-webgl-performance.md
│   ├── v3.2.0-webaudio-core-engine.md # WebAudio 엔진 (Phase 3.2)
│   ├── v3.2.1-audio-node-system.md
│   ├── v3.2.2-realtime-audio-processing.md
│   ├── v3.2.3-audio-visualization.md
│   └── v3.2.4-webaudio-performance.md
├── docs/
│   ├── architecture.md
│   ├── performance-report.md
│   └── evidence/                   # 증거 스크린샷
│       ├── phase-1/
│       ├── phase-2/
│       └── phase-3/
├── migrations/                     # PostgreSQL 마이그레이션
├── monitoring/
│   ├── prometheus/
│   │   └── prometheus.yml
│   └── grafana/
│       └── dashboards/
├── deployments/
│   └── docker/
│       ├── docker-compose.yml
│       └── docker-compose.prod.yml
├── expansion/                      # 확장 계획
│   ├── complete/                   # 완료된 확장
│   ├── not-yet/                    # 미완료 확장
│   └── prompt/                     # 개발 가이드
├── snapshots/                      # 버전 스냅샷
└── README.md
```

---

## 5. 핵심 기능 설계

### 5.1 비디오 업로드 플로우

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   Browser   │────►│  Express    │────►│   Storage   │
│  (Dropzone) │     │  (multer)   │     │  (uploads/) │
└─────────────┘     └──────┬──────┘     └─────────────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  Thumbnail  │
                    │  Generation │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  WebSocket  │
                    │  Progress   │
                    └─────────────┘
```

### 5.2 타임라인 에디터 상태 관리

```typescript
interface TimelineState {
  clips: Clip[];           // 클립 목록
  currentTime: number;     // 현재 재생 위치
  duration: number;        // 전체 길이
  zoom: number;            // 줌 레벨
  selection: {
    clipId: string | null;
    trimStart: number;
    trimEnd: number;
  };
}

interface Clip {
  id: string;
  sourceVideoId: string;
  startTime: number;       // 타임라인 상 시작 위치
  duration: number;        // 클립 길이
  inPoint: number;         // 원본 비디오 시작점
  outPoint: number;        // 원본 비디오 끝점
}
```

### 5.3 C++ 썸네일 추출 인터페이스

```cpp
// thumbnail_extractor.h
class ThumbnailExtractor {
public:
    struct Options {
        int width = 160;
        int height = 90;
        AVPixelFormat format = AV_PIX_FMT_RGB24;
    };

    struct Result {
        std::vector<uint8_t> data;
        int width;
        int height;
        int64_t timestamp_ms;
    };

    // 단일 프레임 추출
    Result extractFrame(const std::string& videoPath, 
                        int64_t timestamp_ms,
                        const Options& options);

    // 다중 프레임 추출 (타임라인용)
    std::vector<Result> extractFrames(const std::string& videoPath,
                                      int count,
                                      const Options& options);
};
```

---

## 6. 데이터 모델

### 6.1 PostgreSQL 스키마

```sql
-- 프로젝트
CREATE TABLE projects (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    name VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 비디오 소스
CREATE TABLE videos (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    project_id UUID REFERENCES projects(id),
    filename VARCHAR(255) NOT NULL,
    path VARCHAR(512) NOT NULL,
    duration_ms INTEGER NOT NULL,
    width INTEGER NOT NULL,
    height INTEGER NOT NULL,
    codec VARCHAR(50),
    bitrate INTEGER,
    framerate DECIMAL(5,2),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 타임라인 클립
CREATE TABLE clips (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    project_id UUID REFERENCES projects(id),
    video_id UUID REFERENCES videos(id),
    track INTEGER DEFAULT 0,
    start_time_ms INTEGER NOT NULL,
    duration_ms INTEGER NOT NULL,
    in_point_ms INTEGER NOT NULL,
    out_point_ms INTEGER NOT NULL,
    order_index INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 내보내기 작업
CREATE TABLE exports (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    project_id UUID REFERENCES projects(id),
    status VARCHAR(20) DEFAULT 'pending',  -- pending, processing, completed, failed
    progress INTEGER DEFAULT 0,
    output_path VARCHAR(512),
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    error_message TEXT
);
```

---

## 7. API 설계

### 7.1 RESTful API 엔드포인트

```yaml
업로드:
  POST /api/upload:
    request: multipart/form-data (video file)
    response:
      id: string
      filename: string
      duration_ms: number
      thumbnail_url: string

프로젝트:
  GET /api/projects:
    response: Project[]
  
  POST /api/projects:
    request: { name: string }
    response: Project
  
  GET /api/projects/:id:
    response: Project (with videos, clips)

편집:
  POST /api/edit/trim:
    request: { videoId, startMs, endMs }
    response: { jobId }
  
  POST /api/edit/split:
    request: { videoId, splitPointMs }
    response: { jobId, clips: Clip[] }

내보내기:
  POST /api/export:
    request: { projectId, format, quality }
    response: { exportId }
  
  GET /api/export/:id/status:
    response: { status, progress, outputUrl? }
```

### 7.2 WebSocket 이벤트

```yaml
연결:
  ws://localhost:3001/progress

이벤트:
  upload:progress:
    { videoId, progress: 0-100 }
  
  edit:progress:
    { jobId, progress: 0-100 }
  
  export:progress:
    { exportId, progress: 0-100, eta_seconds }
  
  export:complete:
    { exportId, outputUrl }
  
  error:
    { type, message }
```

---

## 8. 성능 전략

### 8.1 프론트엔드 최적화

| 영역 | 전략 | 목표 |
|------|------|------|
| 타임라인 렌더링 | Canvas + requestAnimationFrame | 60 FPS |
| 대용량 비디오 | 청크 업로드 (5MB) | 안정적 업로드 |
| 썸네일 로딩 | Lazy loading + 캐싱 | 즉시 표시 |
| 번들 사이즈 | 코드 분할 + Tree shaking | < 500KB |

### 8.2 백엔드 최적화

| 영역 | 전략 | 목표 |
|------|------|------|
| 파일 I/O | 스트리밍 처리 | 메모리 효율 |
| FFmpeg | 스레드 풀 (4개) | 병렬 처리 |
| 캐싱 | Redis (썸네일, 메타데이터) | 반복 요청 감소 |
| 연결 | Keep-Alive | 연결 오버헤드 감소 |

### 8.3 C++ 최적화

| 영역 | 전략 | 목표 |
|------|------|------|
| 메모리 | 메모리 풀 재사용 | 할당 오버헤드 감소 |
| 디코딩 | 하드웨어 가속 (선택) | 10배 속도 향상 |
| 스레딩 | N-API AsyncWorker | 논블로킹 |

---

## 9. 리스크 & 완화 전략

### 9.1 기술적 리스크

| 리스크 | 영향 | 완화 전략 |
|--------|------|----------|
| FFmpeg C API 복잡성 | 높음 | 래퍼 라이브러리 참조, 점진적 구현 |
| N-API 학습 곡선 | 중간 | 공식 예제 기반, 단순한 함수부터 시작 |
| 브라우저 비디오 호환성 | 중간 | MP4 (H.264) 표준화 |
| 메모리 누수 | 높음 | valgrind 정기 검사, RAII 패턴 |

### 9.2 일정 리스크

| 리스크 | 영향 | 완화 전략 |
|--------|------|----------|
| Phase 1 지연 | 높음 | fluent-ffmpeg로 빠른 프로토타입 |
| Phase 2 지연 | 중간 | 핵심 기능만 C++로 구현 |
| 테스트 누락 | 중간 | CI에서 커버리지 체크 |

---

## 10. 증거 문서 계획

### 10.1 수집할 증거

```yaml
성능 벤치마크:
  - 썸네일 추출 시간 (fluent-ffmpeg vs C++)
  - 타임라인 FPS 측정
  - API 응답 시간 분포

코드 품질:
  - 테스트 커버리지 보고서
  - ESLint 결과
  - TypeScript 타입 체크 결과

인프라:
  - Grafana 대시보드 스크린샷
  - Docker 컨테이너 리소스 사용량
  - 메모리 누수 검사 결과 (valgrind)
```

### 10.2 문서화 산출물

```
docs/
├── architecture.md          # 시스템 아키텍처 상세
├── performance-report.md    # 성능 벤치마크 결과
├── native-addon-guide.md    # C++ 네이티브 애드온 가이드
└── evidence/
    ├── benchmarks/          # 벤치마크 스크린샷
    ├── dashboards/          # Grafana 스크린샷
    └── tests/               # 테스트 결과
```

---

## 11. 성공 지표

### 11.1 정량적 지표

```yaml
기능 완성도:
  - [ ] 비디오 업로드 동작
  - [ ] 타임라인 편집 동작
  - [ ] 트림/분할 동작
  - [ ] 내보내기 동작
  - [ ] C++ 썸네일 추출 < 50ms

성능:
  - [ ] 타임라인 60 FPS
  - [ ] API p99 < 200ms
  - [ ] 메모리 누수 0개

품질:
  - [ ] 테스트 커버리지 ≥ 70%
  - [ ] 린트 경고 0개
```

### 11.2 Voyager X 매칭 체크리스트

```yaml
필수 요구사항:
  - [x] React + TypeScript 사용
  - [x] Node.js 백엔드
  - [x] C++ 깊은 이해 증명 (Native Addon)
  - [x] FFmpeg 직접 사용
  - [x] 동영상 기술 구현

추가 역량:
  - [x] 프로덕션급 인프라 (Docker, Prometheus)
  - [x] 성능 최적화 경험
  - [x] 문서화 능력
```

---

## 12. 다음 단계

### 즉시 실행 항목

1. **Phase 0 Bootstrap**
   - React + Vite 프론트엔드 생성
   - Node.js + Express 백엔드 생성
   - TypeScript 설정
   - 개발 환경 구축

2. **Phase 1.0 시작**
   - 비디오 업로드 구현
   - HTML5 Video 미리보기
   - Canvas 타임라인 기초

### 참고 문서
- [Phase 0 Bootstrap](./v0.1.0-bootstrap.md)
- [Phase 1.0 Basic Infrastructure](./v1.0.0-basic-infrastructure.md)
- [Phase 2.0 Native Addon Setup](./v2.0.0-native-addon-setup.md)
- [Phase 3.1 WebGL Core Engine](./v3.1.0-webgl-core-engine.md)
- [Phase 3.2 WebAudio Core Engine](./v3.2.0-webaudio-core-engine.md)
- [FFmpeg 공식 문서](https://ffmpeg.org/documentation.html)
- [N-API 가이드](https://nodejs.org/api/n-api.html)
