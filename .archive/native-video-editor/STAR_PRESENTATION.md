# Native Video Editor — STAR 방식 프로젝트 발표

> 행동 기반 역량 면접(BEI) 대응을 위한 STAR 구조 발표 자료

---

## 🎯 Situation (상황)

### 프로젝트 배경

**목표 회사**: Voyager X 및 미디어 테크 스타트업

**시장 요구사항**:
- 웹 기반 비디오 편집기 시장 성장 (Canva, Kapwing 등)
- 브라우저 환경에서 데스크톱급 편집 성능 요구
- 실시간 미리보기, 빠른 응답 시간이 핵심 경쟁력

**기술적 도전**:
1. **성능 한계**: JavaScript/Node.js만으로는 미디어 처리 성능 부족
2. **실시간 렌더링**: 60 FPS 미리보기를 웹에서 구현
3. **대용량 처리**: 수 GB 영상 파일의 효율적 처리
4. **사용자 경험**: 작업 진행률 실시간 피드백

### 개인 역할

풀스택 개발자로서 프론트엔드, 백엔드, 네이티브 애드온 전 영역을 설계 및 구현

---

## 📋 Task (과업)

### 핵심 목표

| 영역 | 목표 | 측정 지표 |
|------|------|----------|
| **성능** | 데스크톱급 편집 성능 | 60 FPS, 50ms 응답 |
| **기능** | 전문 편집 기능 | Trim, Split, 자막, 속도 |
| **안정성** | 프로덕션 수준 품질 | 99.9% 가용성 |
| **확장성** | 대규모 미디어 처리 | 10GB+ 파일 지원 |

### 기술적 요구사항

1. **프론트엔드**
   - WebGL2 기반 GPU 가속 렌더링
   - WebAudio API 실시간 오디오 처리
   - 드래그 기반 타임라인 UI

2. **백엔드**
   - RESTful API + WebSocket 실시간 통신
   - PostgreSQL 영속화, Redis 캐싱
   - 청크 기반 대용량 업로드

3. **네이티브**
   - C++ N-API로 FFmpeg 직접 연동
   - RAII 패턴으로 메모리 안전성
   - AVFrame 풀링으로 성능 최적화

---

## 🔧 Action (행동)

### 1단계: 기초 인프라 구축 (v0.1.0 ~ v1.3.0)

#### 프론트엔드 아키텍처
```
React 18 + TypeScript + Vite
├── 컴포넌트 설계: Atomic Design 패턴
├── 상태 관리: Context + useReducer
└── 성능 최적화: React.memo, useMemo
```

**핵심 결정**: Vite 선택
- 이유: HMR 속도, ESM 기반 빌드
- 결과: 개발 서버 시작 1초 이내

#### 백엔드 설계
```
Node.js 20 + Express + TypeScript
├── fluent-ffmpeg: 영상 처리
├── WebSocket (ws): 실시간 진행률
└── PostgreSQL/Redis: 데이터 관리
```

**핵심 결정**: WebSocket 진행률 시스템
- 문제: HTTP polling은 서버 부하 과다
- 해결: WebSocket 단방향 푸시
- 결과: 네트워크 요청 90% 감소

### 2단계: 네이티브 성능 최적화 (v2.0.0 ~ v2.3.0)

#### 문제 분석
```
기존 (fluent-ffmpeg):
  메타데이터 추출: ~100ms
  썸네일 생성: ~200ms
  원인: 프로세스 spawn 오버헤드
```

#### 해결책: C++ Native Addon
```cpp
// RAII 패턴으로 자원 관리
class AVFormatContextPtr {
    AVFormatContext* ctx_;
public:
    ~AVFormatContextPtr() {
        if (ctx_) avformat_close_input(&ctx_);
    }
};

// AVFrame 메모리 풀
class FramePool {
    std::vector<AVFrame*> pool_;  // 32 프레임 재사용
    // LRU 정책으로 관리
};
```

**성과**:
| 작업 | Before | After | 개선 |
|------|--------|-------|------|
| 메타데이터 | 100ms | 2ms | **50x** |
| 썸네일 | 200ms | 47ms | **4x** |

### 3단계: 렌더링 엔진 (v3.1.0 ~ v3.2.4)

#### WebGL2 파이프라인
```
Texture Upload → Shader Processing → Framebuffer → Display
      ↓                   ↓                ↓
  GPU 메모리         밝기/대비/채도      다중 패스
```

**핵심 최적화**:
1. **Texture Atlas**: 다수 썸네일을 단일 2K 텍스처에 패킹
2. **Draw Call Batching**: 상태 변경 최소화
3. **Frame Pool**: 8개 GPU 프레임 재사용

#### WebAudio 시스템
```
AudioContext
    ├── GainNode (볼륨)
    ├── AnalyserNode (시각화)
    ├── BiquadFilter (EQ)
    └── DynamicsCompressor
```

**핵심 기능**:
- FFT 기반 주파수 시각화
- 프레임 정확도 비디오-오디오 동기화
- AudioBuffer 풀링

### 4단계: 프로덕션 인프라 (v3.0.0)

#### Docker 구성
```yaml
services:
  frontend:    # Nginx + React 빌드
  backend:     # Node.js API
  postgres:    # 데이터 영속화
  redis:       # 캐싱
  prometheus:  # 메트릭 수집
  grafana:     # 대시보드
```

#### 모니터링
- Prometheus: 커스텀 메트릭 (응답 시간, 에러율)
- Grafana: 실시간 대시보드

---

## 📈 Result (결과)

### 정량적 성과

| 메트릭 | 목표 | 달성 | 상태 |
|--------|------|------|------|
| 미리보기 FPS | 60 | 60 | ✅ |
| 썸네일 추출 p99 | 50ms | 47ms | ✅ |
| 메타데이터 분석 | 100ms | 2ms | ✅ 50x |
| API 응답 p99 | 200ms | 180ms | ✅ |

### 기술적 성취

1. **풀스택 구현**: 프론트엔드 → 백엔드 → 네이티브 전 레이어 직접 개발
2. **시스템 프로그래밍**: C++ N-API로 Node.js와 FFmpeg 직접 연동
3. **GPU 프로그래밍**: WebGL2, GLSL 셰이더 직접 작성
4. **실시간 시스템**: WebSocket + WebAudio 기반 저지연 처리

### 배운 점

1. **언어 경계 최적화**: JavaScript ↔ C++ 데이터 전송 최소화
2. **메모리 관리**: RAII, 풀링으로 GC 부담 제거
3. **GPU 활용**: CPU 병목을 GPU 오프로딩으로 해결
4. **관측성**: 메트릭 기반 성능 진단 중요성

---

## 💡 핵심 기술 역량 증명

### 1. 문제 해결 능력
> "fluent-ffmpeg 100ms → C++ 직접 연동 2ms"

JavaScript 라이브러리 한계를 파악하고, 시스템 수준 해결책 도출

### 2. 성능 최적화
> "WebGL2 + GPU 풀링으로 60 FPS 달성"

브라우저 환경의 제약 내에서 데스크톱급 성능 구현

### 3. 풀스택 역량
> "프론트 → 백엔드 → 네이티브 → 인프라"

단일 프로젝트에서 모든 기술 스택 직접 구현

### 4. 프로덕션 마인드셋
> "Prometheus/Grafana 통합 모니터링"

개발 완료가 아닌 운영까지 고려한 설계

---

## 🎤 예상 질문 및 답변

### Q1: 왜 C++ Native Addon을 선택했나요?

**답변**: 
- fluent-ffmpeg는 매번 FFmpeg 프로세스를 spawn하여 IPC 오버헤드 발생
- 썸네일 수십 장 생성 시 초 단위 지연
- C++ N-API로 직접 연동하면 in-process 호출로 오버헤드 제거
- 결과: 50배 성능 향상

### Q2: WebGL2 선택 이유는?

**답변**:
- Canvas 2D는 CPU 기반으로 30 FPS 한계
- WebGL2는 GPU 활용으로 60 FPS 가능
- Framebuffer로 멀티패스 렌더링 지원
- 이펙트 셰이더 커스터마이징 가능

### Q3: 가장 어려웠던 문제는?

**답변**:
- **문제**: FFmpeg AVFrame 메모리 누수
- **원인**: 수동 메모리 관리 실수
- **해결**: RAII 래퍼 클래스 + 메모리 풀
- **교훈**: C++에서는 자원 관리가 핵심

### Q4: 확장성 고려는?

**답변**:
- 청크 업로드: 100MB 단위로 대용량 지원
- Redis 캐싱: 반복 요청 최소화
- PostgreSQL: 프로젝트/미디어 영속화
- Docker: 수평 확장 가능한 컨테이너 구조
