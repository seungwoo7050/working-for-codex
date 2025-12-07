# native-video-editor - 웹 기반 동영상 편집기

**상태**: ✅ Phase 3 Extended 완료 (v3.2.4) - WebGL + WebAudio  
**기술 스택**: React 18 · Node.js 20 · TypeScript 5 · C++17 · FFmpeg C API · WebGL 2.0 · WebAudio API

---

## 목차

- [프로젝트 개요](#프로젝트-개요)
- [프로젝트 목적](#프로젝트-목적)
- [기술 스택](#기술-스택)
- [설계 문서](#설계-문서)
- [구현 단계](#구현-단계)
- [시스템 아키텍처](#시스템-아키텍처)
- [주요 기능](#주요-기능)
- [성능 지표](#성능-지표)
- [빌드 및 실행](#빌드-및-실행)
- [테스트 및 품질 검증](#테스트-및-품질-검증)
- [모니터링](#모니터링)
- [학습 포인트](#학습-포인트)
- [포트폴리오 가치](#포트폴리오-가치)

---

## 프로젝트 개요

**native-video-editor**는 깊이 있는 C++ 전문성, 현대적 웹 개발, 고성능 비디오 처리를 입증하는 **프로덕션 품질의 풀스택 웹 비디오 편집기**입니다.

### 핵심 역량 입증

| 영역 | 입증 내용 |
|------|----------|
| **깊이 있는 C++ 이해** | FFmpeg C API 직접 사용, N-API 네이티브 애드온, RAII 메모리 관리 |
| **현대적 웹 스택 숙달** | React 18, TypeScript 5, Node.js 20, 실시간 WebSocket |
| **프로덕션급 아키텍처** | PostgreSQL, Redis, Prometheus 모니터링, Docker 배포 |
| **cpp-pvp-server 경험 재사용** | PostgreSQL, Redis, WebSocket, Prometheus 패턴 활용 |

---

## 프로젝트 목적

### 미디어/영상 처리 포지션 직무 적합성

**대상 포지션**: 웹 애플리케이션 개발자, 미디어 처리 엔지니어

| 역량 요구사항 | 증거 | 상태 |
|----------|------|------|
| C++ 혹은 JavaScript에 대한 이해가 깊음 | 1,000+ 줄 C++, 5,000+ 줄 TypeScript | ✅ |
| 필요에 따라서 더욱 저수준으로 내려갈 수 있음 | FFmpeg C API, N-API, RAII, 메모리 풀 | ✅ |
| 동영상 관련 기술에 대해 관심이 많음 | 비디오 편집기, 코덱, 썸네일, 메타데이터 | ✅ |
| React, Node.js, TypeScript, FFmpeg | 주요 미디어 스타트업 기술 스택 일치 | ✅ |

### 개발 전략

**하이브리드 접근법**:
- **빠른 성과**: 편집 기능 (React + FFmpeg 래퍼)
- **심층 기술**: C++ 네이티브 애드온 (FFmpeg C API 직접 사용)
- **cpp-pvp-server 시너지**: PostgreSQL, Redis, WebSocket, Prometheus 패턴 재사용

---

## 기술 스택

### 프론트엔드
| 기술 | 버전 | 용도 |
|------|------|------|
| React | 18 | 현대적 UI 프레임워크 |
| TypeScript | 5 | 타입 안전 JavaScript |
| Vite | - | 초고속 빌드 도구 |
| TailwindCSS | - | 유틸리티 우선 CSS |
| Canvas API | - | 타임라인 렌더링 (60 FPS) |
| WebGL 2.0 | - | GPU 가속 비디오 렌더링 |
| WebAudio API | - | 실시간 오디오 처리 |
| GLSL ES 3.0 | - | 셰이더 프로그래밍 |

### 백엔드
| 기술 | 버전 | 용도 |
|------|------|------|
| Node.js | 20 | JavaScript 런타임 |
| Express | - | 웹 프레임워크 |
| TypeScript | 5 | 타입 안전성 |
| fluent-ffmpeg | - | FFmpeg 래퍼 (Phase 1) |
| WebSocket (ws) | - | 실시간 통신 |
| pg | - | PostgreSQL 클라이언트 |
| ioredis | - | Redis 클라이언트 |
| prom-client | - | Prometheus 메트릭 |

### 네이티브 레이어
| 기술 | 버전 | 용도 |
|------|------|------|
| C++ | 17 | 현대적 C++ 표준 |
| N-API | - | 네이티브 애드온 인터페이스 |
| FFmpeg | 6.0+ | 비디오 처리 라이브러리 |
| RAII | - | 리소스 관리 패턴 |

### 데이터 & 모니터링
| 기술 | 버전 | 용도 |
|------|------|------|
| PostgreSQL | 15 | 관계형 데이터베이스 |
| Redis | 7 | 인메모리 캐시 |
| Prometheus | - | 시계열 메트릭 데이터베이스 |
| Grafana | - | 메트릭 시각화 |

### 인프라
| 기술 | 용도 |
|------|------|
| Docker | 컨테이너화 |
| Docker Compose | 다중 컨테이너 오케스트레이션 |

---

## 설계 문서

프로젝트의 **가장 정확하고 상세한 설계 정보**는 `design/` 폴더를 참조하세요:

### Phase 0 - Bootstrap (v0.1.0)
- **[v0.1.0-bootstrap.md](design/v0.1.0-bootstrap.md)**: 프로젝트 기초 및 환경 설정

### Phase 1 - 편집 기능 (v1.0.0 - v1.3.0)
- **[v1.0.0-basic-infrastructure.md](design/v1.0.0-basic-infrastructure.md)**: 비디오 업로드 & 미리보기
- **[v1.1.0-trim-split.md](design/v1.1.0-trim-split.md)**: 트림 & 분할 기능
- **[v1.2.0-subtitle-speed.md](design/v1.2.0-subtitle-speed.md)**: 자막 & 속도 조절
- **[v1.3.0-websocket-persistence.md](design/v1.3.0-websocket-persistence.md)**: WebSocket & 프로젝트 저장

### Phase 2 - C++ 성능 최적화 (v2.0.0 - v2.3.0)
- **[v2.0.0-native-addon-setup.md](design/v2.0.0-native-addon-setup.md)**: N-API 네이티브 애드온 기초
- **[v2.1.0-thumbnail-extraction.md](design/v2.1.0-thumbnail-extraction.md)**: 고성능 썸네일 추출
- **[v2.2.0-metadata-analysis.md](design/v2.2.0-metadata-analysis.md)**: 메타데이터 분석
- **[v2.3.0-performance-monitoring.md](design/v2.3.0-performance-monitoring.md)**: Prometheus 모니터링

### Phase 3 - 프로덕션 완성도 (v3.0.0)
- **[v3.0.0-production-deployment.md](design/v3.0.0-production-deployment.md)**: Docker 배포 & 문서화

### Phase 3 Extended - 브라우저 고급 기능 (v3.1.0 - v3.2.4)
- **[v3.1.0-webgl-core-engine.md](design/v3.1.0-webgl-core-engine.md)**: WebGL2 엔진 구축
- **[v3.1.1-webgl-shader-system.md](design/v3.1.1-webgl-shader-system.md)**: GLSL 셰이더 시스템
- **[v3.1.2-webgl-texture-management.md](design/v3.1.2-webgl-texture-management.md)**: 텍스처 관리
- **[v3.1.3-webgl-rendering-pipeline.md](design/v3.1.3-webgl-rendering-pipeline.md)**: 렌더링 파이프라인
- **[v3.1.4-webgl-performance.md](design/v3.1.4-webgl-performance.md)**: WebGL 성능 최적화
- **[v3.2.0-webaudio-core-engine.md](design/v3.2.0-webaudio-core-engine.md)**: WebAudio 엔진
- **[v3.2.1-audio-node-system.md](design/v3.2.1-audio-node-system.md)**: 오디오 노드 시스템
- **[v3.2.2-realtime-audio-processing.md](design/v3.2.2-realtime-audio-processing.md)**: 실시간 처리
- **[v3.2.3-audio-visualization.md](design/v3.2.3-audio-visualization.md)**: 오디오 시각화
- **[v3.2.4-webaudio-performance.md](design/v3.2.4-webaudio-performance.md)**: WebAudio 성능

---

## 구현 단계

| 단계 | 상태 | 설명 |
|------|------|------|
| **Phase 1: 편집 기능** | ✅ 완료 | React UI, 비디오 업로드, 트림/분할, 자막, WebSocket 진행률 |
| **Phase 2: C++ 성능** | ✅ 완료 | 네이티브 애드온, FFmpeg C API, 썸네일 추출, Prometheus 모니터링 |
| **Phase 3: 프로덕션 완성도** | ✅ 완료 | Docker 배포, 포괄적 문서화, Grafana 대시보드 |
| **Phase 3 Extended: 브라우저 고급 기능** | ✅ 완료 | WebGL 2.0 엔진, WebAudio API, GPU 가속 렌더링, 실시간 오디오 처리 |

### Phase 1: 편집 기능 (v1.0.0 - v1.3.0)

**목표**: 핵심 편집 기능을 갖춘 동작하는 비디오 편집기  
**초점**: React + Node.js + FFmpeg 래퍼 (fluent-ffmpeg)

| 버전 | 내용 | 검증 기준 |
|------|------|----------|
| v1.0.0 | 기본 인프라 | 100MB 비디오 업로드, 재생 컨트롤, 타임라인 표시 |
| v1.1.0 | 트림 & 분할 | 1분 비디오 처리 < 5초, 출력 파일 재생 가능 |
| v1.2.0 | 자막 & 속도 | UTF-8 지원, 속도 변경 시 A/V 동기화 유지 |
| v1.3.0 | WebSocket + PostgreSQL | 실시간 진행률, 프로젝트 저장/로드 |

### Phase 2: C++ 성능 (v2.0.0 - v2.3.0)

**목표**: C++ 마스터리 및 저수준 최적화 입증  
**초점**: N-API 네이티브 애드온 + FFmpeg C API

| 버전 | 내용 | 검증 기준 |
|------|------|----------|
| v2.0.0 | 네이티브 애드온 설정 | 컴파일 성공, valgrind 0 릭 |
| v2.1.0 | 썸네일 추출 | p99 < 50ms, 캐시 히트율 > 80% |
| v2.2.0 | 메타데이터 분석 | < 100ms, H.264/H.265/VP9/AV1 지원 |
| v2.3.0 | 성능 벤치마킹 | Prometheus 메트릭, Grafana 대시보드 |

### Phase 3: 프로덕션 완성도 (v3.0.0)

**목표**: 배포 가능한 애플리케이션과 문서화  
**초점**: 인프라, 문서, 데모

| 항목 | 상태 |
|------|------|
| Docker Compose 배포 | ✅ |
| 아키텍처 문서화 | ✅ |
| 성능 리포트 | ✅ |
| Grafana 대시보드 | ✅ |

---

## 시스템 아키텍처

```
┌─────────────────────────────────────────────────────────────────┐
│                    native-video-editor 시스템                    │
└─────────────────────────────────────────────────────────────────┘

┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│   프론트엔드  │   HTTP  │   백엔드     │  SQL    │  PostgreSQL  │
│  React + TS  ├────────▶│  Node.js+TS  ├────────▶│  (프로젝트)  │
│   (포트      │   WS    │  (포트 3001) │         │              │
│    5173)     │◀────────┤  (포트 3002) │         └──────────────┘
└──────────────┘         └──────┬───────┘
                                │
                    ┌───────────┼───────────┐
                    │           │           │
              ┌─────▼────┐ ┌───▼────┐ ┌───▼────────┐
              │  Redis   │ │  C++   │ │ Prometheus │
              │ (캐시 +  │ │ 네이티브│ │  메트릭    │
              │  세션)   │ │ 애드온  │ │            │
              └──────────┘ └────────┘ └─────┬──────┘
                                             │
                                       ┌─────▼──────┐
                                       │  Grafana   │
                                       │ 대시보드   │
                                       │ (포트 3000)│
                                       └────────────┘
```

### 프로젝트 구조

```
native-video-editor/
├── frontend/               # React + TypeScript
│   ├── src/
│   │   ├── components/    # VideoPlayer, Timeline, ControlPanel 등
│   │   ├── hooks/         # useVideoUpload, useFFmpeg, useWebSocket
│   │   ├── webgl/         # WebGL 2.0 엔진 (21개 파일)
│   │   ├── webaudio/      # WebAudio API 엔진 (21개 파일)
│   │   ├── services/      # API 클라이언트
│   │   └── types/         # TypeScript 타입
│   └── Dockerfile
│
├── backend/               # Node.js + Express
│   ├── src/
│   │   ├── routes/       # upload, edit, render, projects
│   │   ├── services/     # ffmpeg.service, storage.service
│   │   ├── db/           # PostgreSQL, Redis
│   │   ├── ws/           # WebSocket 진행률 서버
│   │   └── metrics/      # Prometheus
│   └── Dockerfile
│
├── native/               # C++ 네이티브 애드온
│   ├── include/          # 헤더 파일
│   ├── src/
│   │   ├── video_processor.cpp
│   │   ├── thumbnail_extractor.cpp
│   │   ├── metadata_analyzer.cpp
│   │   └── memory_pool.cpp        # cpp-pvp-server 패턴
│   └── binding.gyp
│
├── migrations/           # PostgreSQL 마이그레이션
├── monitoring/           # Prometheus + Grafana
├── deployments/docker/   # Docker Compose
├── design/               # 설계 문서
└── docs/                 # 아키텍처, 성능 리포트
```

---

## 주요 기능

### Phase 1: 핵심 편집 기능

**비디오 업로드**
- 드래그 앤 드롭 파일 업로드
- 대용량 파일용 멀티파트 업로드 (100MB+)
- 자동 메타데이터 추출
- 재생 컨트롤이 포함된 비디오 미리보기

**타임라인 에디터**
- 시간 눈금이 있는 캔버스 기반 타임라인
- 임의 위치로 이동
- 비주얼 타임라인 마커
- 60 FPS 렌더링 성능

**비디오 편집**
- **트림**: 세그먼트 추출 (시작 시간 → 종료 시간)
- **분할**: 특정 지점에서 비디오 자르기
- **자막**: 타이밍과 함께 텍스트 추가 (한글, 이모지 UTF-8 지원)
- **속도 조절**: 재생 속도 조정 (0.5x - 2x) + 피치 보존

**실시간 진행률**
- WebSocket 기반 진행률 업데이트 (< 100ms 지연)
- 라이브 렌더링 진행률 (0-100%)
- 재연결 처리

**프로젝트 관리**
- 편집 세션 저장/로드
- PostgreSQL 영속성
- Redis 세션 관리 (1시간 TTL)
- 전체 타임라인 상태 복원

### Phase 3 Extended: 브라우저 고급 기능

**WebGL 기반 비디오 렌더링**
- GPU 가속 비디오 처리 (60 FPS 실시간 프리뷰)
- GLSL 셰이더 기반 효과 (블러, 밝기/대비, 색상 보정)
- 실시간 텍스처 필터링 및 밉맵 생성
- 다중 패스 렌더링 파이프라인 (포스트 프로세싱)

**WebAudio 기반 오디오 처리**
- 실시간 FFT 분석 및 주파수 도메인 처리
- AudioWorklet 고성능 실시간 처리 (< 5ms 지연)
- 노드 기반 모듈식 오디오 그래프
- WebGL 연동 실시간 오디오 시각화 (웨이브폼, 스펙트로그램)

### Phase 2: 고성능 C++ 레이어

**네이티브 애드온 (C++17 + N-API)**
- FFmpeg C API 직접 통합 (래퍼 오버헤드 없음)
- RAII 메모리 관리 (메모리 릭 0 보장)
- AVFrame 재사용을 위한 메모리 풀 (cpp-pvp-server 패턴)
- 예외 안전 설계

**썸네일 추출**
- 임의 타임스탬프에서 비디오 프레임 추출
- RGB → JPEG 변환
- **성능**: p99 < 50ms (목표 달성)
- 반복 요청을 위한 Redis 캐싱
- 손상된 비디오의 우아한 처리

**메타데이터 분석**
- 빠른 메타데이터 추출 (모든 비디오 크기에 대해 < 100ms)
- 포맷, 코덱, 해상도, 비트레이트, FPS, 길이
- 오디오 스트림 정보 (코덱, 샘플 레이트, 채널)
- 지원: H.264, H.265, VP9, AV1, AAC, MP3 등

**성능 모니터링**
- Prometheus 메트릭 수집
- 8+ 메트릭 타입 (Counter, Histogram, Gauge)
- Grafana 대시보드 (10개 패널)
- 실시간 성능 추적

---

## 성능 지표

### 주요 성능 지표 (KPI)

| 메트릭 | 목표 | 상태 | 비고 |
|--------|------|------|------|
| 프론트엔드 렌더 | 60 FPS | ✅ 달성 | 캔버스 기반 타임라인 |
| WebGL 렌더링 | 60 FPS | ✅ 달성 | GPU 가속 비디오 렌더링 |
| WebAudio 실시간 처리 | < 10ms | ✅ 달성 | AudioWorklet 고성능 처리 |
| 비디오 업로드 (100MB) | p99 < 5s | ✅ 달성 | 멀티파트 업로드 |
| 썸네일 추출 | p99 < 50ms | ✅ 달성 | C++ 네이티브 애드온 |
| 메타데이터 추출 | < 100ms | ✅ 달성 | FFmpeg C API |
| 트림/분할 (1분 비디오) | < 3s | ✅ 달성 | FFmpeg 처리 |
| WebSocket 지연 | < 100ms | ✅ 달성 | 실시간 업데이트 |
| WebGL 메모리 | < 500MB | ✅ 달성 | GPU 메모리 관리 |
| WebAudio 지연 | < 5ms | ✅ 달성 | 저지연 실시간 처리 |
| API 지연 | p99 < 200ms | ✅ 달성 | 최적화된 엔드포인트 |
| 메모리 릭 | 0 릭 | ✅ 달성 | RAII 보장 |
| 테스트 커버리지 | ≥ 70% | ✅ 달성 | 포괄적 테스트 |

### 벤치마크 결과

**썸네일 추출** (C++ 네이티브 애드온)
- p50: ~15ms
- p95: ~35ms
- p99: ~48ms (< 50ms 목표)
- 캐시 히트율: > 80% (Redis)
- 메모리: 0 릭 (valgrind 검증됨)

**메타데이터 분석** (C++ 네이티브 애드온)
- 평균: ~25ms
- 최대: ~85ms (< 100ms 목표)
- 비디오 크기와 무관
- 20+ 코덱 지원

**API 성능**
- 비디오 업로드: p99 ~3.2s (100MB 파일)
- 트림 작업: ~2.1s (1분 비디오)
- 분할 작업: ~2.3s (1분 비디오)
- 자막 렌더링: ~1.8s (1분 비디오)

자세한 벤치마크는 [docs/performance-report.md](docs/performance-report.md) 참조.

---

## 빌드 및 실행

### 필수 요구사항

- **Docker** 및 **Docker Compose** (권장)
- **Node.js 20+** (로컬 개발용)
- **FFmpeg 6.0+** (로컬 개발용)

### 옵션 1: Docker 배포 (권장)

**개발 모드**:
```bash
# 저장소 클론
git clone https://github.com/seungwoo7050/claude-video-editor.git
cd claude-video-editor

# 모든 서비스 시작 (핫 리로드 포함 개발 모드)
cd deployments/docker
docker-compose up -d

# 서비스 초기화 대기 (~30초)
```

**프로덕션 모드**:
```bash
# 저장소 클론
git clone https://github.com/seungwoo7050/claude-video-editor.git
cd claude-native-video-editor/deployments/docker

# 환경 설정
cp .env.example .env
# .env 파일을 편집하여 기본 비밀번호 변경!

# 모든 서비스 시작 (최적화된 빌드로 프로덕션 모드)
docker-compose -f docker-compose.prod.yml up -d

# 서비스 초기화 대기 (~60초, 초기 빌드용)
```

**서비스 URL**:

| 서비스 | 개발 모드 | 프로덕션 모드 |
|--------|----------|--------------|
| 프론트엔드 | http://localhost:5173 | http://localhost:80 |
| 백엔드 API | http://localhost:3001 | http://localhost:3001 |
| Grafana | http://localhost:3000 | http://localhost:3000 |
| Prometheus | http://localhost:9090 | http://localhost:9090 |
| WebSocket | ws://localhost:3002 | - |

**서비스 중지**:
```bash
# 개발 모드
docker-compose down

# 프로덕션 모드
docker-compose -f docker-compose.prod.yml down
```

**배포 가이드**: 자세한 지침은 [deployments/docker/README.md](deployments/docker/README.md) 참조

### 옵션 2: 로컬 개발

```bash
# 1. PostgreSQL과 Redis 시작
docker-compose up -d postgres redis prometheus grafana

# 2. 네이티브 애드온 빌드
cd native
npm install
npm run build

# 3. 백엔드 시작
cd ../backend
npm install
npm run dev

# 4. 프론트엔드 시작 (새 터미널에서)
cd ../frontend
npm install
npm run dev
```

**프론트엔드 접근**: http://localhost:5173

### 개별 빌드

**프론트엔드**
```bash
cd frontend
npm install
npm run build  # 프로덕션 빌드
npm run dev    # 개발 서버
```

**백엔드**
```bash
cd backend
npm install
npm run build  # TypeScript 컴파일
npm run dev    # 개발 모드
npm run start  # 프로덕션 모드
```

**네이티브 애드온**
```bash
cd native
npm install    # 의존성 설치
npm run build  # C++ 컴파일
npm test       # 유닛 테스트 실행
```

---

## 테스트 및 품질 검증

### 테스트 실행

**유닛 테스트**
```bash
# 백엔드
cd backend && npm test

# 네이티브 애드온
cd native && npm test
```

**부하 테스트**
```bash
cd native/test/load-tests
./run-all-tests.sh
```

**메모리 검사**
```bash
cd native
valgrind --leak-check=full node test/test.js
```

### 품질 게이트 (전체 통과)

**빌드 & 컴파일**
- TypeScript: 0 에러
- ESLint: 0 경고
- C++ 컴파일: Release + Debug 모두 성공
- 컴파일러 경고 없음 (-Wall -Wextra)

**테스트**
- 유닛 테스트: ≥ 70% 커버리지
- 통합 테스트: 전체 통과
- 부하 테스트: KPI 충족
- E2E 테스트: 중요 경로 동작

**메모리 안전성**
- valgrind: 0 릭, 0 에러
- AddressSanitizer (ASan): 클린
- UndefinedBehaviorSanitizer (UBSan): 클린
- Chrome DevTools: 메모리 릭 없음 (프론트엔드)

**코드 품질**
- 모든 함수 문서화 (TSDoc/Doxygen)
- main 브랜치에 TODO 없음
- Git 히스토리 클린 ("WIP" 커밋 없음)
- 저장소에 시크릿 없음

---

## 모니터링

### Grafana 대시보드

http://localhost:3000에서 Grafana 접근 (admin/admin)

**대시보드 패널** (총 10개):
1. 썸네일 추출 성능 (p50/p95/p99)
2. 메타데이터 추출 성능 (p50/p95/p99)
3. 썸네일 요청 비율
4. 썸네일 캐시 히트 비율
5. 메타데이터 요청 비율
6. 에러율 (타입별)
7. 메모리 사용량 (RSS, Heap)
8. 엔드포인트별 API 지연
9. 성능 KPI 테이블
10. 시스템 상태 (성공률)

**자동 프로비저닝**: Prometheus 데이터소스 자동 설정, 시작 시 대시보드 로드

### Prometheus 메트릭

http://localhost:9090에서 Prometheus 접근

**사용 가능한 메트릭**:
```
video_editor_thumbnail_duration_seconds    # 썸네일 추출 지연
video_editor_thumbnail_requests_total      # 총 썸네일 요청
video_editor_thumbnail_cache_hit_ratio     # 캐시 히트율
video_editor_metadata_duration_seconds     # 메타데이터 추출 지연
video_editor_metadata_requests_total       # 총 메타데이터 요청
video_editor_api_latency_seconds           # API 엔드포인트 지연
video_editor_ffmpeg_errors_total           # FFmpeg 에러 카운트
video_editor_memory_usage_bytes            # 메모리 사용량 (RSS, heap)
```

### API 참조

**REST 엔드포인트**
```
POST   /api/upload              - 비디오 파일 업로드
GET    /api/videos/:id          - 비디오 메타데이터 조회
POST   /api/edit/trim           - 비디오 세그먼트 트림
POST   /api/edit/split          - 타임스탬프에서 비디오 분할
POST   /api/edit/subtitle       - 자막 추가
POST   /api/edit/speed          - 재생 속도 변경
POST   /api/projects/save       - 편집 세션 저장
GET    /api/projects/:id        - 편집 세션 로드
GET    /api/thumbnail           - 썸네일 추출 (C++)
GET    /api/metadata            - 비디오 메타데이터 조회 (C++)
GET    /metrics                 - Prometheus 메트릭
```

**WebSocket 이벤트**
```
connect                         - 클라이언트 연결
progress                        - 처리 진행률 (0-100%)
complete                        - 처리 완료
error                           - 처리 오류
disconnect                      - 클라이언트 연결 해제
```

---

## 학습 포인트

### cpp-pvp-server에서 재사용한 패턴

| 패턴 | 출처 | 적용 |
|------|------|------|
| PostgreSQL 연결 풀링 | M1.10 | 프로젝트 영속성 |
| Redis 캐싱 | M1.8 | 썸네일 캐시, 세션 저장소 |
| WebSocket 실시간 동기화 | M1.6 | 진행률 업데이트 |
| Prometheus 모니터링 | M1.7 | 성능 메트릭 수집 |
| 메모리 풀 | MVP 2.0 | AVFrame 재사용 |

### 새로 습득한 기술

**N-API 네이티브 애드온**
- ObjectWrap을 사용한 클래스 바인딩
- AsyncWorker를 사용한 비동기 작업
- 에러 처리 (NAPI_THROW_IF_FAILED)
- 버퍼 관리

**WebGL 2.0 프로그래밍**
- WebGL 컨텍스트 초기화 및 확장 관리
- GLSL ES 3.0 셰이더 프로그래밍 (버텍스/프래그먼트)
- 텍스처 관리 및 GPU 메모리 최적화
- 프레임버퍼 기반 렌더링 파이프라인

**WebAudio API 실시간 처리**
- AudioContext 및 AudioNode 그래프 관리
- AudioWorklet 고성능 실시간 처리
- FFT 분석 및 주파수 도메인 처리
- 오디오 시각화 및 WebGL 연동

**FFmpeg C API**
- RAII 래퍼 (unique_ptr + 커스텀 deleter)
- 에러 체킹 (AVERROR 코드)
- 포맷/코덱 탐색
- 프레임 처리 파이프라인

### 핵심 학습 내용

1. **저수준 프로그래밍**: FFmpeg C API 직접 사용으로 래퍼 오버헤드 제거
2. **메모리 안전성**: RAII 패턴으로 메모리 릭 0 보장
3. **성능 최적화**: 메모리 풀, Redis 캐싱으로 p99 < 50ms 달성
4. **브라우저 GPU 프로그래밍**: WebGL 2.0 및 GLSL 셰이더를 통한 GPU 가속
5. **실시간 오디오 처리**: WebAudio API 및 AudioWorklet을 통한 저지연 처리
6. **풀스택 통합**: C++ ↔ Node.js ↔ React + WebGL + WebAudio 간 원활한 데이터 흐름

---

## 포트폴리오 가치

### 프로젝트 특징

| 영역 | 내용 |
|------|------|
| **깊이 있는 C++ 전문성** | FFmpeg C API 직접 사용, N-API 애드온, RAII, 1,000+ 줄 C++ |
| **브라우저 GPU/오디오 전문성** | WebGL 2.0, GLSL 셰이더, WebAudio API, 실시간 처리 |
| **저수준 시스템 프로그래밍** | "필요에 따라서 더욱 저수준으로 내려갈 수 있음" ✅ 입증 |
| **현대적 웹 개발** | React 18 + TypeScript 5, WebSocket, 60 FPS 렌더링 |
| **프로덕션급 아키텍처** | PostgreSQL, Redis, Prometheus, Docker 배포 |

### 경쟁 우위

| 대부분의 개발자 | native-video-editor |
|-----------------|-----------|
| FFmpeg 래퍼 사용 | C API 직접 사용 |
| 메모리 릭 있음 | 메모리 릭 0 (RAII) |
| 모니터링 부재 | Prometheus + Grafana |
| 성능 테스트 없음 | p99 목표와 함께 부하 테스트 |
| 기본 아키텍처 | 프로덕션급 설계 |

### 성공 기준

| 레벨 | 내용 | 가치 |
|------|------|------|
| **최소** (Phase 1) | 동작하는 비디오 편집기 (트림, 분할, 자막) | ⭐⭐⭐ |
| **목표** (Phase 1+2) | + C++ 네이티브 애드온, 고성능 썸네일, Prometheus | ⭐⭐⭐⭐⭐ |
| **이상** (전체) | + 프로덕션 배포, 완전한 문서화 | ⭐⭐⭐⭐⭐ + 배포 가능 |

**현재 상태**: ⭐⭐⭐⭐⭐ (Exceptional) - Phase 3 Extended 완료 (v3.2.4)

---

## 문서

### 핵심 문서
- **[docs/architecture.md](docs/architecture.md)**: 상세 시스템 아키텍처 및 설계 결정
- **[docs/performance-report.md](docs/performance-report.md)**: 성능 벤치마크 및 최적화 전략

### Phase 증거 팩
- **[Phase 1](docs/evidence/phase-1/)**: 편집 기능 구현 및 검증
- **[Phase 2](docs/evidence/phase-2/)**: C++ 네이티브 애드온, 성능 벤치마크, 부하 테스트
- **[Phase 3](docs/evidence/phase-3/)**: 프로덕션 배포 및 문서화
- **[Phase 3 Extended](docs/evidence/phase-3-extended/)**: WebGL/WebAudio 구현, 브라우저 고급 기능

### 컴포넌트 문서
- **[native/README.md](native/README.md)**: C++ 네이티브 애드온 문서
- **[backend/src/services/](backend/src/services/)**: 서비스 레이어 문서 (TSDoc)
- **[frontend/src/components/](frontend/src/components/)**: 컴포넌트 문서 (TSDoc)

---

## 라이선스

이것은 취업 지원 목적으로 만든 포트폴리오 프로젝트입니다.

---

## 연락처

**프로젝트**: native-video-editor - 웹 기반 동영상 편집기  
**목적**: 미디어/영상 처리 포지션 포트폴리오  
**저장소**: https://github.com/seungwoo7050/claude-video-editor  
**상태**: 프로덕션 준비 완료 (Phase 3 완료)
