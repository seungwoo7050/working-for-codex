# 📘 native-video-editor (native-video-editor/) 클론코딩 가이드

> **학습 중심 개발 과정 재현**: C++ FFmpeg + React + Node.js 풀스택 비디오 에디터

## 🎯 학습 목표 재정의

**핵심 목표**: 실제 소프트웨어 개발 과정을 재현하여 각 기술 스택의 깊이 있는 이해
- **C++ FFmpeg**: 저수준 미디어 처리, 메모리 관리, 성능 최적화
- **React**: 복잡한 UI 상태 관리, 실시간 인터랙션
- **Node.js**: 비동기 파일 처리, 스트리밍, 외부 프로세스 관리
- **시스템 설계**: 다중 언어 통합, 빌드 시스템, 배포 전략

## 📚 기술 스택별 독립 학습 프로젝트

실제 개발처럼 각 기술을 독립적으로 학습한 후 통합:

### 1. cpp-ffmpeg-basics (C++ FFmpeg 기초)
```bash
# 별도 프로젝트: FFmpeg C API 학습
├── include/ffmpeg_wrapper.h    # RAII 래퍼
├── src/thumbnail_extractor.cpp # 썸네일 추출
├── CMakeLists.txt             # 빌드 설정
└── test/                      # 단위 테스트
```

### 2. react-video-components (React 비디오 UI)
```bash
# 별도 프로젝트: React 컴포넌트 학습
├── src/components/VideoPlayer.tsx
├── src/hooks/useVideoState.ts
├── src/utils/videoUtils.ts
└── cypress/                   # E2E 테스트
```

### 3. nodejs-video-api (Node.js 비디오 처리)
```bash
# 별도 프로젝트: 백엔드 API 학습
├── src/services/FFmpegService.ts
├── src/routes/video.routes.ts
├── src/middleware/upload.ts
└── test/                      # API 테스트
```

## 🔄 개발 과정 재현 강화

### 각 단계별 "기술 결정 기록"

**예시: FFmpeg vs Web APIs 선택**

```markdown
## 기술 결정: FFmpeg 직접 사용 vs Web APIs

### 고려사항
1. **Web APIs (HTML5 Video, Canvas)**
   - ✅ 브라우저 네이티브, 추가 설치 불필요
   - ✅ JavaScript만으로 구현 가능
   - ❌ 브라우저 보안 제한 (로컬 파일 접근)
   - ❌ 코덱 지원 제한 (브라우저별 차이)
   - ❌ 고급 편집 기능 제한

2. **FFmpeg C++ 직접 사용**
   - ✅ 모든 비디오/오디오 코덱 지원
   - ✅ 고성능 저수준 처리
   - ✅ Node.js 애드온으로 JavaScript 연동
   - ❌ C++ 학습 곡선 높음
   - ❌ 빌드 복잡성 증가
   - ❌ 메모리 관리 직접 책임

### 최종 결정
**FFmpeg 직접 사용** - 교육적 가치와 기능 완성도를 위해
- 학습 목표: 시스템 프로그래밍, 메모리 관리, 빌드 시스템
- 실제 개발 반영: 성능-critical 애플리케이션의 현실적 선택
```

## 📋 작업 내용
| 순서 | 작업 | Design 문서 | 산출물 | 학습 포커스 |
|------|------|-------------|--------|-------------|
| 0.1 | 프로젝트 디렉토리 생성 | `design/v0.1.0-bootstrap.md` | `native-video-editor/` | 프로젝트 구조 설계 |
| 0.2 | Vite + React + TailwindCSS 설정 | 〃 | `frontend/` | 모던 프론트엔드 툴체인 |
| 0.3 | Express + TypeScript 설정 | 〃 | `backend/` | 타입 안전 백엔드 |
| 0.4 | 로컬 실행 확인 | 〃 | `npm run dev` 동작 | 개발 환경 구축 |

## 🔍 각 단계별 "고민과 결정" 문서화

### Phase 1.0: Basic Infrastructure

**기술적 고민**:
- React 컴포넌트 설계: 단일 책임 원칙 vs 성능 최적화
- 상태 관리: useState vs useReducer vs 외부 라이브러리
- 파일 업로드: 클라이언트 vs 서버 사이드 validation

**결정 기록**:
```markdown
## VideoUpload 컴포넌트 설계 결정

### 고민: Drag & Drop vs Click Upload
- **Drag & Drop**: UX 좋음, 현대적 인터페이스
- **Click Upload**: 단순함, 접근성 좋음
- **결정**: 둘 다 지원 (점진적 향상)

### 고민: 파일 validation 위치
- **클라이언트**: 빠른 피드백, UX 좋음
- **서버**: 보안, 신뢰성 높음
- **결정**: 클라이언트 pre-validation + 서버 final validation
```

## 🏗️ 단순화된 실행 버전 생성

raw-http-server처럼 각 Phase별로 실행 가능한 버전을 만들되, 복잡성을 관리:

### v0.1.0-simple: Bootstrap Only
```bash
# 최소 실행 환경
npm run dev  # 프론트엔드만
npm run server  # 백엔드만
```

### v1.0.0-mock: Basic UI (FFmpeg Mock)
```javascript
// backend/src/services/ffmpeg.service.ts (Mock 버전)
class FFmpegService {
  async trim(videoPath, start, end) {
    // 실제 FFmpeg 대신 파일 카피로 시뮬레이션
    return mockTrimResult;
  }
}
```

### v2.0.0-native: C++ Integration
```bash
# 실제 FFmpeg 통합
npm run build:native
npm run dev
```

## 📖 소스코드 학습 방법

완성된 코드를 효과적으로 학습하기 위한 단계별 접근법:

### 1단계: 전체 구조 파악 (30분)
```bash
# 프로젝트 구조 파악
tree -I node_modules -L 3

# 의존성 분석
cat package.json | jq '.dependencies'
cat package.json | jq '.devDependencies'
```

**학습 포인트**:
- Monorepo 구조 이해 (frontend/, backend/, native/)
- 각 디렉토리의 역할 파악
- 기술 스택별 의존성 분석

### 2단계: 핵심 컴포넌트 분석 (1시간)
```bash
# 주요 파일들 순서대로 읽기
1. frontend/src/App.tsx                    # 애플리케이션 진입점
2. frontend/src/components/VideoUpload.tsx   # 주요 UI 컴포넌트
3. backend/src/routes/upload.routes.ts      # API 엔드포인트
4. backend/src/services/ffmpeg.service.ts   # 비즈니스 로직
5. native/src/video_processor.cpp          # C++ 네이티브 코드
```

**분석 포인트**:
- 각 컴포넌트의 책임 범위
- 데이터 흐름 (Props, State, API 호출)
- 에러 처리 패턴
- 타입 안전성 (TypeScript)

### 3단계: 실행 및 디버깅 (2시간)
```bash
# 로컬 실행
npm run dev          # 프론트엔드
npm run server       # 백엔드
npm run build:native # C++ 빌드

# 브라우저에서 테스트
open http://localhost:5173  # 프론트엔드
curl http://localhost:3001/health  # 백엔드 API
```

**학습 활동**:
- console.log 추가해서 데이터 흐름 추적
- Network 탭에서 API 호출 분석
- React DevTools로 컴포넌트 상태 확인
- 작은 수정으로 동작 변경해보기

### 4단계: 처음부터 재구현 (시간 제한 없음)
```bash
# 새 프로젝트 생성
mkdir my-video-editor
cd my-video-editor

# 단계별 재구현
# Phase 0: 프로젝트 설정
# Phase 1.0: 기본 UI 컴포넌트
# Phase 1.1: FFmpeg 통합
# ...
```

**재구현 원칙**:
- 참고 코드를 보고 완전히 새로 작성
- 각 줄의 이유를 이해하며 작성
- 주석으로 설계 결정 기록
- 테스트 코드 함께 작성

## 📊 학습 진행 추적 시스템

### Phase별 체크리스트
- [ ] **Phase 0**: Bootstrap (3시간)
  - [ ] 디자인 문서 읽기 (30분)
  - [ ] Vite + React + TailwindCSS 설정 (1시간)
  - [ ] Express + TypeScript 설정 (1시간)
  - [ ] 로컬 실행 확인 (30분)

- [ ] **Phase 1.0**: Basic Infrastructure (10시간)
  - [ ] VideoUpload 컴포넌트 구현 (2시간)
  - [ ] useVideoUpload 훅 구현 (1시간)
  - [ ] multer 업로드 라우트 구현 (1시간)
  - [ ] VideoPlayer 컴포넌트 구현 (2시간)
  - [ ] Timeline 컴포넌트 구현 (2시간)
  - [ ] App.tsx 통합 및 테스트 (1시간)

- [ ] **Phase 1.1**: Trim & Split (4시간)
  - [ ] StorageService 구현 (1시간)
  - [ ] FFmpegService 기본 구조 (1시간)
  - [ ] edit.routes.ts API 구현 (1시간)
  - [ ] EditPanel UI 통합 (1시간)

- [ ] **Phase 1.2**: Subtitle & Speed (4시간)
  - [ ] SRT 자막 생성 로직 (1시간)
  - [ ] FFmpeg 필터 체인 구현 (1시간)
  - [ ] SubtitleEditor 컴포넌트 (1시간)
  - [ ] ControlPanel 속도 조절 UI (1시간)

- [ ] **Phase 1.3**: WebSocket & Persistence (6시간)
  - [ ] WebSocketService 구현 (2시간)
  - [ ] DatabaseService 구현 (1시간)
  - [ ] Redis 캐싱 서비스 (1시간)
  - [ ] ProjectPanel 컴포넌트 (1시간)

- [ ] **Phase 2.0**: C++ Native Addon (8시간)
  - [ ] CMake + node-gyp 설정 (2시간)
  - [ ] FFmpeg RAII 래퍼 구현 (2시간)
  - [ ] N-API 바인딩 (2시간)
  - [ ] 빌드 및 통합 테스트 (2시간)

### 시간 추정 vs 실제 소요 기록
| Phase | 예상 시간 | 실제 소요 | 차이 | 주요 교훈 |
|-------|----------|----------|------|----------|
| Phase 0 | 3시간 | | | |
| Phase 1.0 | 10시간 | | | |
| Phase 1.1 | 4시간 | | | |
| Phase 1.2 | 4시간 | | | |
| Phase 1.3 | 6시간 | | | |
| Phase 2.0 | 8시간 | | | |

### 어려웠던 부분 및 해결 기록
```markdown
## 기술적 도전 기록

### 도전 1: FFmpeg C++ 통합
**어려웠던 점**: 메모리 관리, 빌드 시스템 복잡성
**해결 방법**: RAII 패턴 적용, CMake 스크립트 모듈화
**학습 포인트**: 시스템 프로그래밍의 중요성, 크로스 플랫폼 빌드

### 도전 2: WebSocket 실시간 처리
**어려웠던 점**: 상태 동기화, 에러 처리
**해결 방법**: 이벤트 기반 아키텍처, 재연결 로직 구현
**학습 포인트**: 실시간 애플리케이션 설계 패턴

### 도전 3: 다중 언어 디버깅
**어려웠던 점**: C++ ↔ JavaScript 간 디버깅
**해결 방법**: 로깅 강화, 단위 테스트 추가
**학습 포인트**: 다중 언어 시스템의 복잡성 관리
```

### 주간 학습 리뷰 템플릿
```markdown
## 📅 주간 학습 리뷰 (YYYY-MM-DD)

### 이번 주 목표
- [ ] Phase X.Y 완료
- [ ] Z 기술 개념 마스터

### 실제 진행 상황
- 완료된 작업: 
- 진행 중인 작업:
- blocker 이슈:

### 배운 것
**기술적 성취**:
- [개념1]: [설명]
- [개념2]: [설명]

**개발 프로세스**:
- [교훈1]: [설명]
- [교훈2]: [설명]

### 다음 주 계획
- 우선순위 작업:
- 준비사항:
- 예상 도전 과제:
```

## 🔍 디자인 문서 vs 클론코딩 가이드 버전 세부화

### 현재 구조 분석

**CLONE-CODING-GUIDE.md 버전 세부화**:
```
Phase 1: Core Features
├── MVP 1.0: Basic Infrastructure (6단계 작업)
├── MVP 1.1: Trim & Split (5단계 작업)
├── MVP 1.2: Subtitle & Speed (4단계 작업)
└── MVP 1.3: WebSocket & Persistence (7단계 작업)
```

**디자인 문서 버전 세부화** (예상):
```
design/
├── v1.0.0-basic-infrastructure.md    # MVP 1.0 전체
├── v1.1.0-trim-split.md             # MVP 1.1 전체
├── v1.2.0-subtitle-speed.md         # MVP 1.2 전체
└── v1.3.0-websocket-persistence.md  # MVP 1.3 전체
```

### 디자인 문서에서 버전 세부화 정보 파악 가능성

**충분히 파악 가능** ✅

디자인 문서는 각 MVP의 **전체 요구사항과 아키텍처**를 담고 있지만, CLONE-CODING-GUIDE.md처럼 **시간별 작업 세부화**는 담고 있지 않습니다.

**디자인 문서에서 얻을 수 있는 정보**:
- 각 MVP의 목표와 요구사항
- 시스템 아키텍처 설계
- 구현 전략과 기술 결정
- 검증 방법과 완료 기준

**CLONE-CODING-GUIDE.md에서 추가로 얻는 정보**:
- 시간별 작업 단계 (1.0.1, 1.0.2, ...)
- 커밋 메시지 템플릿
- 예상 소요 시간
- 구체적인 파일 경로

### 권장 접근법

1. **디자인 문서로 전체 그림 파악** (30분)
   - 요구사항과 아키텍처 이해
   - 기술 결정 배경 파악

2. **CLONE-CODING-GUIDE.md로 세부 작업 계획** (30분)
   - 시간별 작업 단계 확인
   - 커밋 포인트 계획

3. **소스코드로 구현 세부사항 학습** (시간 제한 없음)
   - 실제 코드 패턴 분석
   - 에러 처리와 최적화 기법 학습

## 📁 버전에 따른 소스코드 변화 처리

### raw-http-server 방식 적용

**native-video-editor도 raw-http-server처럼 각 버전별 실행 파일을 만들면 됩니다:**

```
native-video-editor/
├── main_1.0.1.js          # MVP 1.0의 첫 번째 버전
├── main_1.0.2.js          # MVP 1.0의 두 번째 버전
├── main_1.0.3.js          # MVP 1.0의 세 번째 버전
├── main_1.0.4.js          # MVP 1.0의 네 번째 버전
├── main_1.0.5.js          # MVP 1.0의 다섯 번째 버전
├── main_1.0.6.js          # MVP 1.0 완료 (v1.0.0 태그)
├── main_1.1.1.js          # MVP 1.1 시작
├── main_1.1.2.js          # MVP 1.1 두 번째 버전
├── main_1.1.3.js          # MVP 1.1 세 번째 버전
├── main_1.1.4.js          # MVP 1.1 네 번째 버전
├── main_1.1.5.js          # MVP 1.1 완료 (v1.1.0 태그)
└── ...
```

### 버전별 변화 시각화

**각 버전 파일의 차이점**:
```javascript
// main_1.0.1.js - 기본 컴포넌트만
import VideoUpload from './components/VideoUpload';
import VideoPlayer from './components/VideoPlayer';

// main_1.0.2.js - 훅 추가
import VideoUpload from './components/VideoUpload';
import VideoPlayer from './components/VideoPlayer';
import { useVideoUpload } from './hooks/useVideoUpload';

// main_1.0.3.js - 백엔드 API 추가
import VideoUpload from './components/VideoUpload';
import VideoPlayer from './components/VideoPlayer';
import { useVideoUpload } from './hooks/useVideoUpload';
// + 백엔드 라우트 통합

// main_1.0.4.js - 타임라인 추가
// + Timeline 컴포넌트
// + Canvas 렌더링

// main_1.0.5.js - 통합 및 테스트
// + 모든 컴포넌트 통합
// + 에러 처리 강화
```

### 버전 관리 전략

**1. 점진적 기능 추가**:
- 각 버전마다 하나의 기능만 추가
- 이전 버전의 모든 기능 유지
- 테스트 코드 함께 진화

**2. Git 태그 활용**:
```bash
# MVP 완료 시 태그
git tag -a v1.0.0 -m "MVP 1.0: Basic Infrastructure"
git tag -a v1.1.0 -m "MVP 1.1: Trim & Split"
```

**3. 버전별 디렉토리 구조** (선택):
```
versions/
├── v1.0.0-basic-infrastructure/
│   ├── frontend/
│   ├── backend/
│   └── README.md
├── v1.1.0-trim-split/
│   └── ...
```

### 학습 효과 극대화

**버전별 소스코드 변화의 장점**:
1. **점진적 학습**: 한 번에 하나의 개념 학습
2. **실제 개발 재현**: 실제 개발자의 점진적 개선 과정
3. **디버깅 용이**: 특정 버전에서 문제가 생기면 이전 버전과 비교
4. **코드 리뷰 연습**: 각 버전의 변경사항 분석

**결론**: 디자인 문서에서 버전 세부화 정보를 충분히 파악할 수 있고, 버전에 따른 소스코드 변화는 raw-http-server처럼 각 버전별 실행 파일을 만들어 처리하면 됩니다. 🚀

---

## Phase 1: Core Features (5-7일)
```bash
# 0.1 → 0.2 [📅 2025-07-16 14:00:00]
GIT_AUTHOR_DATE="2025-07-16 14:00:00" GIT_COMMITTER_DATE="2025-07-16 14:00:00" \
git commit -m "chore: initialize video-editor project structure

Decision: Monorepo structure for multi-language integration
- frontend/: React SPA
- backend/: Node.js API server  
- native/: C++ FFmpeg addon
- shared/: Common types/interfaces

Trade-offs: Complexity vs maintainability"

# 0.2 완료 [📅 2025-07-16 16:30:00]
GIT_AUTHOR_DATE="2025-07-16 16:30:00" GIT_COMMITTER_DATE="2025-07-16 16:30:00" \
git commit -m "chore(frontend): setup Vite + React + TailwindCSS

Decision: Vite for fast development
- Build speed: 10-100x faster than CRA
- Modern ES modules, native TypeScript support
- Plugin ecosystem for complex builds

Alternative considered: Create React App (simpler but slower)"

# 0.3 완료 [📅 2025-07-17 19:00:00]
GIT_AUTHOR_DATE="2025-07-17 19:00:00" GIT_COMMITTER_DATE="2025-07-17 19:00:00" \
git commit -m "chore(backend): setup Express + TypeScript

Decision: TypeScript for type safety
- Runtime error reduction: ~20-30%
- Better IDE support and refactoring
- Learning curve but long-term productivity gain

Trade-off: Development speed vs reliability"

# 0.4 완료 → v0.1.0 태그 [📅 2025-07-17 21:00:00]
GIT_AUTHOR_DATE="2025-07-17 21:00:00" GIT_COMMITTER_DATE="2025-07-17 21:00:00" \
git commit -m "chore: verify local development environment

Verification checklist:
- [x] Frontend dev server: http://localhost:5173
- [x] Backend API server: http://localhost:3001  
- [x] TypeScript compilation: 0 errors
- [x] Hot reload working
- [x] Cross-origin requests working"
GIT_COMMITTER_DATE="2025-07-17 21:15:00" git tag -a v0.1.0 -m "Bootstrap: project initialization

Key achievements:
- Multi-language monorepo structure established
- Modern development toolchain configured
- Local development environment verified

Next challenges:
- State management strategy for complex video editor
- File upload handling for large video files
- Real-time UI updates for video processing"
```

### ✅ 완료 기준
- [ ] `frontend/`: Vite 개발 서버 실행 (http://localhost:5173)
- [ ] `backend/`: Express 서버 실행 (http://localhost:3001)
- [ ] TypeScript 컴파일 에러 0
- [ ] **학습 목표**: 각 기술 스택의 기본 설정 이해

---

## Phase 1: Core Features (5-7일)

### MVP 1.0: Basic Infrastructure (2일)

> 📅 **권장 기간**: 2025년 7월 18일 ~ 7월 22일

#### 🎯 학습 포커스: 풀스택 아키텍처 설계
- **프론트엔드**: React 컴포넌트 설계 패턴, 상태 관리 전략
- **백엔드**: RESTful API 설계, 파일 업로드 처리
- **통합**: CORS, 에러 처리, 타입 안전성

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/react/01-react-js-ts-component-minimum-basics.md
│   └── 컴포넌트, useState, useRef
├── prerequisite/react/02-react-vite-project-structure-api-client-layer.md
│   └── Vite 구조, API 클라이언트
└── prerequisite/node.js/02-express-rest-api-minimal-server-structure.md
    └── Express 라우팅, 미들웨어, multer
```

#### 🏗️ 기술 결정 기록

**VideoUpload 컴포넌트: Drag & Drop vs Traditional Upload**
```markdown
## 결정: Progressive Enhancement 접근

### 고려사항
1. **Modern UX (Drag & Drop)**
   - ✅ Intuitive, desktop app-like experience
   - ✅ Supports multiple files
   - ❌ Browser compatibility issues
   - ❌ Mobile unfriendly

2. **Traditional Upload (Click)**
   - ✅ Universal compatibility
   - ✅ Simple, reliable
   - ❌ Less engaging UX
   - ❌ Single file focus

### 최종 결정
**둘 다 지원**: Progressive enhancement
- Base: Click upload (fallback)
- Enhancement: Drag & drop (modern browsers)
- Mobile: Touch-optimized upload

### 구현 전략
```typescript
// useVideoUpload.ts
const useVideoUpload = () => {
  const [isDragOver, setIsDragOver] = useState(false);
  
  const handleDragOver = (e: DragEvent) => {
    e.preventDefault();
    setIsDragOver(true);
  };
  
  const handleDrop = (e: DragEvent) => {
    e.preventDefault();
    setIsDragOver(false);
    const files = Array.from(e.dataTransfer.files);
    uploadFiles(files);
  };
  
  // Fallback for traditional upload
  const handleFileSelect = (e: ChangeEvent<HTMLInputElement>) => {
    const files = Array.from(e.target.files);
    uploadFiles(files);
  };
};
```
```

**파일 Validation: Client vs Server**
```markdown
## 결정: Defense in Depth

### 전략
1. **Client-side pre-validation**: 즉각적 피드백
   - 파일 타입 체크 (video/*)
   - 파일 크기 제한 (100MB)
   - 빠른 사용자 피드백

2. **Server-side final validation**: 보안
   - MIME 타입 검증
   - 바이러스 스캔 (미래 확장)
   - 저장소 용량 체크

### 학습 포인트
- 웹 보안: Never trust client input
- UX: Progressive validation feedback
- 성능: Client-side validation으로 서버 부하 감소
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 1.0.1 | VideoUpload 컴포넌트 | `design/v1.0.0-basic-infrastructure.md` | `frontend/src/components/VideoUpload.tsx` | React 컴포넌트 설계 |
| 1.0.2 | useVideoUpload 훅 | 〃 | `frontend/src/hooks/useVideoUpload.ts` | 커스텀 훅 패턴 |
| 1.0.3 | multer 업로드 라우트 | 〃 | `backend/src/routes/upload.routes.ts` | 파일 업로드 처리 |
| 1.0.4 | VideoPlayer 컴포넌트 | 〃 | `frontend/src/components/VideoPlayer.tsx` | HTML5 Video API |
| 1.0.5 | Timeline (Canvas) 컴포넌트 | 〃 | `frontend/src/components/Timeline.tsx` | Canvas 렌더링 |
| 1.0.6 | App.tsx 통합 | 〃 | `frontend/src/App.tsx` | 컴포넌트 통합 |

#### 🔖 커밋 포인트
```bash
# 1.0.1 완료
git commit -m "feat(frontend): implement VideoUpload component with drag-drop

Decision: Progressive enhancement for upload UX
- Base: Traditional file input
- Enhancement: Drag & drop overlay
- Mobile: Touch-optimized interaction

Trade-off: Complexity vs UX improvement"

# 1.0.2 완료
git commit -m "feat(frontend): add useVideoUpload hook for upload state

Pattern: Custom hook for reusable upload logic
- State management: loading, progress, error
- File validation: client-side pre-checks
- API integration: fetch-based upload

Learning: Separation of concerns in React"

# 1.0.3 완료
git commit -m "feat(backend): add multer upload route with file validation

Decision: Multer for file handling
- Memory storage for small files (< 100MB)
- Disk storage fallback for large files
- Validation: MIME type, size limits

Security consideration: File type validation"

# 1.0.4 완료
git commit -m "feat(frontend): implement VideoPlayer with HTML5 video API

Challenge: Video loading states
- Loading spinner during buffering
- Error handling for corrupted files
- Seek bar with time display

Learning: Media element APIs"

# 1.0.5 완료
git commit -m "feat(frontend): add Timeline component with Canvas rendering

Technical decision: Canvas vs SVG
- Canvas: Better performance for frequent updates
- SVG: Easier for complex shapes (future feature)

Learning: Canvas 2D API, coordinate systems"

# 1.0.6 완료 → v1.0.0 태그
git commit -m "feat(frontend): integrate components in App.tsx

Architecture: Component composition
- Props drilling consideration (future: Context API)
- Component lifecycle management
- Error boundaries for stability"
git tag -a v1.0.0 -m "MVP 1.0: Basic Infrastructure

Technical achievements:
- Progressive enhancement upload UX
- Canvas-based timeline rendering
- RESTful file upload API
- Component-based architecture

Learnings:
- Defense in depth validation strategy
- React custom hooks pattern
- HTML5 media APIs
- Canvas rendering fundamentals"
```

#### ✅ 완료 기준
- [ ] 100MB 비디오 업로드 → URL 반환
- [ ] 비디오 재생/일시정지/시크 동작
- [ ] Canvas 타임라인 렌더링
- [ ] 콘솔 에러 0
- [ ] **학습 목표**: 풀스택 컴포넌트 설계 이해

---

### MVP 1.1: Trim & Split (1.5일)

> 📅 **권장 기간**: 2025년 7월 23일 ~ 7월 25일

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/FFmpeg/F01-ffmpeg-concepts-build-link-cmake.md
│   └── FFmpeg 개념, 코덱 카피 vs 리인코딩
└── prerequisite/javascript-typescript/02-js-async-and-patterns.md
    └── Promise, async/await (FFmpeg 비동기 처리)
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.1.1 | StorageService 구현 | `design/v1.1.0-trim-split.md` | `backend/src/services/storage.service.ts` |
| 1.1.2 | FFmpegService (trim/split) | 〃 | `backend/src/services/ffmpeg.service.ts` |
| 1.1.3 | edit.routes.ts 구현 | 〃 | `backend/src/routes/edit.routes.ts` |
| 1.1.4 | EditPanel 컴포넌트 | 〃 | `frontend/src/components/EditPanel.tsx` |
| 1.1.5 | useVideoEdit 훅 | 〃 | `frontend/src/hooks/useVideoEdit.ts` |

#### 🔖 커밋 포인트
```bash
# 1.1.1 완료
git commit -m "feat(backend): implement StorageService for file management"
# 1.1.2 완료
git commit -m "feat(backend): add FFmpegService with trim/split operations"
# 1.1.3 완료
git commit -m "feat(backend): add edit routes for trim and split API"
# 1.1.4 완료
git commit -m "feat(frontend): implement EditPanel with trim/split UI"
# 1.1.5 완료 → v1.1.0 태그
git commit -m "feat(frontend): add useVideoEdit hook for edit operations"
git tag -a v1.1.0 -m "MVP 1.1: Trim & Split"
```

#### ✅ 완료 기준
- [ ] Trim: 1분 영상 10s-30s 구간 추출 → 20s 출력
- [ ] Split: 30s 지점에서 분할 → 2개 파일 생성
- [ ] 처리 시간 < 5s (100MB, 코덱 카피)

---

### MVP 1.2: Subtitle & Speed (1.5일)

> 📅 **권장 기간**: 2025년 7월 26일 ~ 7월 28일

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
└── prerequisite/c++/FFmpeg/F01-ffmpeg-concepts-build-link-cmake.md
    └── FFmpeg 필터 그래프 (setpts, atempo, subtitles)
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.2.1 | SRT 생성 로직 | `design/v1.2.0-subtitle-speed.md` | `backend/src/services/ffmpeg.service.ts` |
| 1.2.2 | FFmpeg 필터 체인 (speed + subtitle) | 〃 | 〃 |
| 1.2.3 | SubtitleEditor 컴포넌트 | 〃 | `frontend/src/components/SubtitleEditor.tsx` |
| 1.2.4 | 속도 조절 UI | 〃 | `frontend/src/components/ControlPanel.tsx` |

#### 🔖 커밋 포인트
```bash
# 1.2.1 완료
git commit -m "feat(backend): add SRT subtitle generation logic"
# 1.2.2 완료
git commit -m "feat(backend): implement FFmpeg filter chain for speed and subtitle"
# 1.2.3 완료
git commit -m "feat(frontend): implement SubtitleEditor component"
# 1.2.4 완료 → v1.2.0 태그
git commit -m "feat(frontend): add speed control UI to ControlPanel"
git tag -a v1.2.0 -m "MVP 1.2: Subtitle & Speed"
```

#### ✅ 완료 기준
- [ ] 자막 burn-in 동작 (UTF-8 한글/이모지)
- [ ] 0.5x ~ 2.0x 속도 조절
- [ ] 자막 + 속도 동시 적용

---

### MVP 1.3: WebSocket & Persistence (2일)

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/server-basic/06-database-connection-pool.md
│   └── 커넥션 풀, PostgreSQL
├── prerequisite/node.js/03-node-express-mysql-prisma-data-layer.md
│   └── ORM 패턴, 데이터 레이어 (Prisma 대신 pg 직접 사용)
└── prerequisite/node.js/04-node-express-prisma-relational-modeling.md
    └── 관계형 모델링 패턴
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.3.1 | WebSocketService 구현 | `design/v1.3.0-websocket-persistence.md` | `backend/src/ws/websocket.service.ts` |
| 1.3.2 | useWebSocket 훅 | 〃 | `frontend/src/hooks/useWebSocket.ts` |
| 1.3.3 | PostgreSQL 마이그레이션 | 〃 | `migrations/001_create_projects.sql` |
| 1.3.4 | DatabaseService 구현 | 〃 | `backend/src/db/database.service.ts` |
| 1.3.5 | Redis 캐싱 서비스 | 〃 | `backend/src/db/redis.service.ts` |
| 1.3.6 | project.routes.ts 구현 | 〃 | `backend/src/routes/project.routes.ts` |
| 1.3.7 | ProjectPanel 컴포넌트 | 〃 | `frontend/src/components/ProjectPanel.tsx` |

#### 🔖 커밋 포인트
```bash
# 1.3.1 완료
git commit -m "feat(backend): implement WebSocketService with ws library"
# 1.3.2 완료
git commit -m "feat(frontend): add useWebSocket hook with auto-reconnect"
# 1.3.3 완료
git commit -m "feat(db): add PostgreSQL migration for projects table"
# 1.3.4 완료
git commit -m "feat(backend): implement DatabaseService with pg client"
# 1.3.5 완료
git commit -m "feat(backend): add Redis caching service"
# 1.3.6 완료
git commit -m "feat(backend): add project routes for CRUD operations"
# 1.3.7 완료 → v1.3.0 태그
git commit -m "feat(frontend): implement ProjectPanel component"
git tag -a v1.3.0 -m "MVP 1.3: WebSocket & Persistence"
```

#### ✅ 완료 기준
- [ ] WebSocket 연결/재연결 동작
- [ ] 처리 진행률 실시간 표시
- [ ] 프로젝트 저장/불러오기 동작
- [ ] Redis 캐시 히트율 > 80%

---

## Phase 2: Native Performance (5-7일)

### MVP 2.0: C++ Native Addon Setup (2일)

> 📅 **권장 기간**: 2025년 8월 4일 ~ 8월 10일

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite (핵심!)
├── prerequisite/c++/01-cpp17-server-ffmpeg-minimum-basics.md
│   └── C++17 기본 문법, 컴파일러, 빌드
├── prerequisite/c++/02-cpp17-memory-raii-error-logging.md
│   └── 스마트 포인터, RAII 패턴 ⭐
├── prerequisite/c++/03-cpp17-cmake-project-structure-debugging.md
│   └── CMake, node-gyp, 빌드 시스템
├── prerequisite/c++/04-cpp17-move-semantics-rvalue.md
│   └── std::move, std::forward, 이동 의미론 ⭐
└── prerequisite/c++/07-cpp17-napi-node-addon.md
    └── N-API, node-addon-api, ObjectWrap ⭐⭐
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.0.1 | binding.gyp 설정 | `design/v2.0.0-native-addon-setup.md` | `native/binding.gyp` |
| 2.0.2 | RAII 래퍼 헤더 | 〃 | `native/include/ffmpeg_raii.h` |
| 2.0.3 | 메모리 풀 구현 | 〃 | `native/src/memory_pool.cpp` |
| 2.0.4 | N-API 바인딩 스켈레톤 | 〃 | `native/src/video_processor.cpp` |
| 2.0.5 | 빌드 및 로드 테스트 | 〃 | `npm run build` |

#### 🔖 커밋 포인트
```bash
# 2.0.1 완료
git commit -m "chore(native): configure binding.gyp for node-addon-api"
# 2.0.2 완료
git commit -m "feat(native): add RAII wrappers for FFmpeg resources"
# 2.0.3 완료
git commit -m "feat(native): implement memory pool for frame buffers"
# 2.0.4 완료
git commit -m "feat(native): add N-API binding skeleton for video_processor"
# 2.0.5 완료 → v2.0.0 태그
git commit -m "test(native): verify native module build and load"
git tag -a v2.0.0 -m "MVP 2.0: C++ Native Addon Setup"
```

#### ✅ 완료 기준
- [ ] `npm run build` → 네이티브 모듈 컴파일 성공
- [ ] `require('video_processor')` → 로드 성공
- [ ] valgrind 메모리 누수 0

---

### MVP 2.1: Thumbnail Extraction (2일)

> 📅 **권장 기간**: 2025년 8월 11일 ~ 8월 16일

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/FFmpeg/F02-ffmpeg-open-input-stream-info-wrapper.md
│   └── AVFormatContext, 스트림 열기
└── prerequisite/c++/FFmpeg/F03-ffmpeg-video-decode-to-raw-frames.md
    └── 디코딩, AVFrame, swscale
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.1.1 | ThumbnailExtractor 헤더 | `design/v2.1.0-thumbnail-extraction.md` | `native/include/thumbnail_extractor.h` |
| 2.1.2 | open_video / seek_and_decode | 〃 | `native/src/thumbnail_extractor.cpp` |
| 2.1.3 | frame_to_jpeg 구현 | 〃 | 〃 |
| 2.1.4 | N-API ObjectWrap 바인딩 | 〃 | `native/src/video_processor.cpp` |
| 2.1.5 | Express 라우트 연동 | 〃 | `backend/src/routes/thumbnail.ts` |

#### 🔖 커밋 포인트
```bash
# 2.1.1 완료
git commit -m "feat(native): add ThumbnailExtractor header with interface"
# 2.1.2 완료
git commit -m "feat(native): implement video open and seek_and_decode"
# 2.1.3 완료
git commit -m "feat(native): implement frame_to_jpeg conversion"
# 2.1.4 완료
git commit -m "feat(native): add N-API ObjectWrap binding for ThumbnailExtractor"
# 2.1.5 완료 → v2.1.0 태그
git commit -m "feat(backend): add thumbnail extraction route"
git tag -a v2.1.0 -m "MVP 2.1: Thumbnail Extraction"
```

#### ✅ 완료 기준
- [ ] 썸네일 추출 p99 < 50ms
- [ ] 메모리 풀 재사용 동작
- [ ] 손상된 비디오 graceful 에러 처리

---

### MVP 2.2: Metadata Analysis (1.5일)

> 📅 **권장 기간**: 2025년 8월 17일 ~ 8월 21일

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.2.1 | MetadataAnalyzer 구현 | `design/v2.2.0-metadata-analysis.md` | `native/src/metadata_analyzer.cpp` |
| 2.2.2 | N-API 바인딩 | 〃 | `native/src/video_processor.cpp` |
| 2.2.3 | Redis 캐싱 연동 | 〃 | `backend/src/routes/metadata.ts` |

#### 🔖 커밋 포인트
```bash
# 2.2.1 완료
git commit -m "feat(native): implement MetadataAnalyzer with stream parsing"
# 2.2.2 완료
git commit -m "feat(native): add N-API binding for MetadataAnalyzer"
# 2.2.3 완료 → v2.2.0 태그
git commit -m "feat(backend): add metadata route with Redis caching"
git tag -a v2.2.0 -m "MVP 2.2: Metadata Analysis"
```

#### ✅ 완료 기준
- [ ] 메타데이터 추출 p99 < 10ms
- [ ] 코덱, 해상도, duration 정확히 반환

---

### MVP 2.3: Performance Monitoring (1.5일)

> 📅 **권장 기간**: 2025년 8월 22일 ~ 8월 26일

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 2.3.1 | Prometheus 메트릭 서비스 | `design/v2.3.0-performance-monitoring.md` | `backend/src/services/metrics.service.ts` |
| 2.3.2 | /metrics 엔드포인트 | 〃 | `backend/src/routes/metrics.ts` |
| 2.3.3 | Grafana 대시보드 설정 | 〃 | `monitoring/grafana/` |

#### 🔖 커밋 포인트
```bash
# 2.3.1 완료
git commit -m "feat(backend): implement Prometheus metrics service"
# 2.3.2 완료
git commit -m "feat(backend): add /metrics endpoint"
# 2.3.3 완료 → v2.3.0 태그
git commit -m "chore(monitoring): add Grafana dashboard configuration"
git tag -a v2.3.0 -m "MVP 2.3: Performance Monitoring"
```

#### ✅ 완료 기준
- [ ] 썸네일 추출 히스토그램 메트릭
- [ ] Grafana 대시보드 조회 가능

---

## Phase 3: Production (2-3일)

### MVP 3.0: Docker Deployment

> 📅 **권장 기간**: 2025년 9월 1일 ~ 9월 15일

#### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
└── prerequisite/node.js/05-node-express-production-deployment-guide.md
    └── Docker, 멀티스테이지 빌드, Nginx
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 3.0.1 | Frontend Dockerfile | `design/v3.0.0-production-deployment.md` | `frontend/Dockerfile.prod` |
| 3.0.2 | Backend Dockerfile | 〃 | `backend/Dockerfile.prod` |
| 3.0.3 | docker-compose.yml | 〃 | `deployments/docker/docker-compose.yml` |
| 3.0.4 | Nginx 설정 | 〃 | `frontend/nginx.conf` |

#### 🔖 커밋 포인트
```bash
# 3.0.1 완료
git commit -m "chore(frontend): add production Dockerfile with multi-stage build"
# 3.0.2 완료
git commit -m "chore(backend): add production Dockerfile with native addon"
# 3.0.3 완료
git commit -m "chore(deploy): add docker-compose for full stack"
# 3.0.4 완료 → v3.0.0 태그
git commit -m "chore(frontend): add Nginx configuration for SPA routing"
git tag -a v3.0.0 -m "MVP 3.0: Production Deployment"
```

#### ✅ 완료 기준
- [ ] `docker-compose up` → 전체 스택 실행
- [ ] 외부에서 접근 가능

---

## Phase 3 Extended: WebGL & WebAudio Integration (4-6주)

> 📅 **권장 기간**: 2025년 10월 1일 ~ 11월 15일

### Phase 3.1: WebGL 기반 비디오 편집

#### 🎯 학습 포커스: 브라우저 그래픽 파이프라인
- **WebGL 2.0**: 셰이더 프로그래밍, 텍스처 관리
- **GLSL ES**: 버텍스/프래그먼트 셰이더 작성
- **GPU 렌더링**: 하드웨어 가속, 성능 최적화

---

### MVP 3.1.0: WebGL 코어 엔진 포팅 (2주)

> 📅 **권장 기간**: 2025년 10월 1일 ~ 10월 14일
> **참조 문서**: `v3.1.md`

#### 📖 사전 학습
```
📚 읽어야 할 자료
├── WebGL Fundamentals (https://webglfundamentals.org/)
├── WebGL 2.0 Specification
└── GLSL ES Reference
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.1.0.1 | WebGL 컨텍스트 초기화 | `design/v3.1.0-webgl-core-engine.md` | `frontend/src/webgl/WebGLEngine.ts` | WebGL2 컨텍스트 생성 |
| 3.1.0.2 | WebGL 확장 지원 | 〃 | `frontend/src/webgl/WebGLExtensions.ts` | 확장 감지, 폴백 |
| 3.1.0.3 | WebGL 디버깅 유틸리티 | 〃 | `frontend/src/webgl/WebGLDebug.ts` | 에러 핸들링 |
| 3.1.0.4 | WebGL 메모리 관리 | 〃 | `frontend/src/webgl/WebGLMemoryManager.ts` | 리소스 관리 |

#### 🔖 커밋 포인트
```bash
# 3.1.0.1 완료
git commit -m "feat(webgl): initialize WebGL context with extensions"
# 3.1.0.2 완료
git commit -m "feat(webgl): add WebGL extension detection and fallback"
# 3.1.0.3 완료
git commit -m "feat(webgl): implement WebGL debugging utilities"
# 3.1.0.4 완료 → v3.1.0 태그
git commit -m "feat(webgl): add WebGL memory management system"
git tag -a v3.1.0 -m "v3.1.0: WebGL Core Engine Port"
```

#### ✅ 완료 기준
- [ ] WebGL2 컨텍스트 초기화 성공
- [ ] 확장 감지 및 폴백 동작
- [ ] 메모리 누수 없음 (DevTools Memory 확인)
- [ ] **학습 목표**: WebGL 초기화 및 리소스 관리 이해

---

### MVP 3.1.1: WebGL 셰이더 시스템 (1주)

> 📅 **권장 기간**: 2025년 10월 15일 ~ 10월 21일
> **참조 문서**: `v3.1.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.1.1.1 | GLSL 셰이더 컴파일러 | `design/v3.1.1-webgl-shader-system.md` | `frontend/src/webgl/ShaderCompiler.ts` | 셰이더 컴파일 |
| 3.1.1.2 | 셰이더 프로그램 관리 | 〃 | `frontend/src/webgl/ShaderProgram.ts` | 프로그램 링킹 |
| 3.1.1.3 | 유니폼 변수 바인딩 | 〃 | `frontend/src/webgl/UniformBinder.ts` | 유니폼 전달 |
| 3.1.1.4 | 셰이더 캐싱 시스템 | 〃 | `frontend/src/webgl/ShaderCache.ts` | 성능 최적화 |

#### 🔖 커밋 포인트
```bash
# 3.1.1.1 완료
git commit -m "feat(shader): implement GLSL shader compiler for WebGL"
# 3.1.1.2 완료
git commit -m "feat(shader): add shader program management"
# 3.1.1.3 완료
git commit -m "feat(shader): implement uniform variable binding"
# 3.1.1.4 완료 → v3.1.1 태그
git commit -m "feat(shader): add shader caching for performance"
git tag -a v3.1.1 -m "v3.1.1: WebGL Shader System"
```

#### ✅ 완료 기준
- [ ] 셰이더 컴파일 및 에러 리포팅
- [ ] 프로그램 링킹 성공
- [ ] 유니폼 타입별 바인딩 동작
- [ ] **학습 목표**: GLSL 셰이더 프로그래밍 이해

---

### MVP 3.1.2: WebGL 텍스처 관리 (1주)

> 📅 **권장 기간**: 2025년 10월 22일 ~ 10월 28일
> **참조 문서**: `v3.1.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.1.2.1 | 비디오 텍스처 로딩 | `design/v3.1.2-webgl-texture-management.md` | `frontend/src/webgl/VideoTexture.ts` | 비디오 → 텍스처 |
| 3.1.2.2 | 텍스처 필터링 | 〃 | `frontend/src/webgl/TextureFilter.ts` | 샘플링 모드 |
| 3.1.2.3 | Mipmap 생성 | 〃 | `frontend/src/webgl/MipmapGenerator.ts` | LOD 처리 |
| 3.1.2.4 | 텍스처 압축 | 〃 | `frontend/src/webgl/TextureCompressor.ts` | 메모리 최적화 |

#### 🔖 커밋 포인트
```bash
# 3.1.2.1 완료
git commit -m "feat(texture): implement video texture loading from canvas"
# 3.1.2.2 완료
git commit -m "feat(texture): add texture filtering and sampling"
# 3.1.2.3 완료
git commit -m "feat(texture): implement automatic mipmap generation"
# 3.1.2.4 완료 → v3.1.2 태그
git commit -m "feat(texture): add texture compression for WebGL"
git tag -a v3.1.2 -m "v3.1.2: WebGL Texture Management"
```

#### ✅ 완료 기준
- [ ] 비디오 프레임을 텍스처로 변환
- [ ] 필터링 모드 (LINEAR, NEAREST) 동작
- [ ] Mipmap 자동 생성
- [ ] **학습 목표**: GPU 텍스처 파이프라인 이해

---

### MVP 3.1.3: WebGL 렌더링 파이프라인 (1.5주)

> 📅 **권장 기간**: 2025년 10월 29일 ~ 11월 10일
> **참조 문서**: `v3.1.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.1.3.1 | 프레임버퍼 객체 | `design/v3.1.3-webgl-rendering-pipeline.md` | `frontend/src/webgl/Framebuffer.ts` | 오프스크린 렌더링 |
| 3.1.3.2 | 렌더버퍼 관리 | 〃 | `frontend/src/webgl/Renderbuffer.ts` | 깊이/스텐실 |
| 3.1.3.3 | 다중 패스 렌더링 | 〃 | `frontend/src/webgl/MultiPassRenderer.ts` | 렌더 패스 |
| 3.1.3.4 | 포스트 프로세싱 | 〃 | `frontend/src/webgl/PostProcessor.ts` | 블룸, 블러 효과 |

#### 🔖 커밋 포인트
```bash
# 3.1.3.1 완료
git commit -m "feat(render): implement framebuffer objects for offscreen rendering"
# 3.1.3.2 완료
git commit -m "feat(render): add renderbuffer management for depth/stencil"
# 3.1.3.3 완료
git commit -m "feat(render): implement multi-pass rendering pipeline"
# 3.1.3.4 완료 → v3.1.3 태그
git commit -m "feat(render): add post-processing effects (bloom, blur, etc.)"
git tag -a v3.1.3 -m "v3.1.3: WebGL Rendering Pipeline"
```

#### ✅ 완료 기준
- [ ] FBO 기반 오프스크린 렌더링
- [ ] 다중 패스 효과 적용
- [ ] 포스트 프로세싱 효과 동작
- [ ] **학습 목표**: 고급 렌더링 기법 이해

---

### MVP 3.1.4: WebGL 성능 최적화 (1주)

> 📅 **권장 기간**: 2025년 11월 11일 ~ 11월 17일
> **참조 문서**: `v3.1.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.1.4.1 | WebGL 상태 캐싱 | `design/v3.1.4-webgl-performance.md` | `frontend/src/webgl/StateCache.ts` | 상태 변경 최소화 |
| 3.1.4.2 | 배치 렌더링 | 〃 | `frontend/src/webgl/BatchRenderer.ts` | 드로우콜 최적화 |
| 3.1.4.3 | GPU 메모리 최적화 | 〃 | `frontend/src/webgl/GPUMemoryOptimizer.ts` | 메모리 관리 |
| 3.1.4.4 | WebGL 프로파일링 | 〃 | `frontend/src/webgl/WebGLProfiler.ts` | 성능 측정 |

#### 🔖 커밋 포인트
```bash
# 3.1.4.1 완료
git commit -m "perf(webgl): implement WebGL state caching to reduce calls"
# 3.1.4.2 완료
git commit -m "perf(webgl): add batch rendering for draw call optimization"
# 3.1.4.3 완료
git commit -m "perf(webgl): optimize GPU memory usage and cleanup"
# 3.1.4.4 완료 → v3.1.4 태그
git commit -m "perf(webgl): add WebGL performance profiling tools"
git tag -a v3.1.4 -m "v3.1.4: WebGL Performance Optimization Complete"
```

#### ✅ 완료 기준
- [ ] 60 FPS 실시간 프리뷰
- [ ] 4K 비디오 지원
- [ ] 브라우저 메모리 < 500MB
- [ ] **학습 목표**: GPU 성능 최적화 기법 이해

---

### Phase 3.2: WebAudio 기반 오디오 편집

#### 🎯 학습 포커스: 웹 오디오 프로세싱
- **WebAudio API**: AudioContext, AudioNode, AudioWorklet
- **실시간 프로세싱**: FFT 분석, 필터링
- **시각화**: 웨이브폼, 스펙트로그램

---

### MVP 3.2.0: WebAudio 코어 엔진 구축 (2주)

> 📅 **권장 기간**: 2025년 11월 18일 ~ 12월 1일
> **참조 문서**: `v3.2.md`

#### 📖 사전 학습
```
📚 읽어야 할 자료
├── Web Audio API (MDN)
├── AudioWorklet Documentation
└── Real-time Audio Processing Guide
```

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.2.0.1 | AudioContext 초기화 | `design/v3.2.0-webaudio-core-engine.md` | `frontend/src/audio/WebAudioEngine.ts` | AudioContext 생성 |
| 3.2.0.2 | 오디오 파일 로딩 | 〃 | `frontend/src/audio/AudioLoader.ts` | fetch + decodeAudioData |
| 3.2.0.3 | 오디오 버퍼 관리 | 〃 | `frontend/src/audio/AudioBufferManager.ts` | 버퍼 풀링 |
| 3.2.0.4 | WebAudio 디버깅 | 〃 | `frontend/src/audio/AudioDebug.ts` | 디버깅 유틸리티 |

#### 🔖 커밋 포인트
```bash
# 3.2.0.1 완료
git commit -m "feat(webaudio): initialize AudioContext with optimal settings"
# 3.2.0.2 완료
git commit -m "feat(webaudio): implement audio file loading with fetch API"
# 3.2.0.3 완료
git commit -m "feat(webaudio): add audio buffer management system"
# 3.2.0.4 완료 → v3.2.0 태그
git commit -m "feat(webaudio): implement WebAudio debugging utilities"
git tag -a v3.2.0 -m "v3.2.0: WebAudio Core Engine"
```

#### ✅ 완료 기준
- [ ] AudioContext 초기화 및 resume
- [ ] 오디오 파일 로딩 및 디코딩
- [ ] 버퍼 재사용 시스템 동작
- [ ] **학습 목표**: WebAudio 기본 구조 이해

---

### MVP 3.2.1: 오디오 노드 시스템 (1주)

> 📅 **권장 기간**: 2025년 12월 2일 ~ 12월 8일
> **참조 문서**: `v3.2.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.2.1.1 | 노드 그래프 관리 | `design/v3.2.1-audio-node-system.md` | `frontend/src/audio/AudioNodeGraph.ts` | 노드 연결 |
| 3.2.1.2 | 커스텀 오디오 노드 | 〃 | `frontend/src/audio/CustomAudioNode.ts` | 노드 확장 |
| 3.2.1.3 | 노드 연결 시스템 | 〃 | `frontend/src/audio/NodeConnector.ts` | 동적 연결 |
| 3.2.1.4 | 노드 파라미터 바인딩 | 〃 | `frontend/src/audio/NodeParameter.ts` | 오토메이션 |

#### 🔖 커밋 포인트
```bash
# 3.2.1.1 완료
git commit -m "feat(nodes): implement audio node graph management"
# 3.2.1.2 완료
git commit -m "feat(nodes): add custom audio node creation"
# 3.2.1.3 완료
git commit -m "feat(nodes): implement node connection system"
# 3.2.1.4 완료 → v3.2.1 태그
git commit -m "feat(nodes): add node parameter binding and automation"
git tag -a v3.2.1 -m "v3.2.1: Audio Node System"
```

#### ✅ 완료 기준
- [ ] 노드 그래프 생성/수정/삭제
- [ ] 커스텀 노드 동작
- [ ] 파라미터 오토메이션
- [ ] **학습 목표**: AudioNode 그래프 이해

---

### MVP 3.2.2: 실시간 오디오 프로세싱 (1.5주)

> 📅 **권장 기간**: 2025년 12월 9일 ~ 12월 19일
> **참조 문서**: `v3.2.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.2.2.1 | ScriptProcessor 노드 | `design/v3.2.2-realtime-audio-processing.md` | `frontend/src/audio/RealtimeProcessor.ts` | 레거시 방식 |
| 3.2.2.2 | AudioWorklet 구현 | 〃 | `frontend/src/audio/AudioWorkletProcessor.ts` | 고성능 처리 |
| 3.2.2.3 | FFT 분석 | 〃 | `frontend/src/audio/FFTAnalyzer.ts` | 주파수 분석 |
| 3.2.2.4 | 실시간 필터링 | 〃 | `frontend/src/audio/RealtimeFilter.ts` | 이퀄라이저 |

#### 🔖 커밋 포인트
```bash
# 3.2.2.1 완료
git commit -m "feat(processing): implement ScriptProcessor for real-time audio"
# 3.2.2.2 완료
git commit -m "feat(processing): add AudioWorklet for high-performance processing"
# 3.2.2.3 완료
git commit -m "feat(processing): implement FFT analysis for frequency domain"
# 3.2.2.4 완료 → v3.2.2 태그
git commit -m "feat(processing): add real-time audio filtering effects"
git tag -a v3.2.2 -m "v3.2.2: Real-time Audio Processing"
```

#### ✅ 완료 기준
- [ ] AudioWorklet 고성능 처리
- [ ] FFT 분석 < 10ms
- [ ] 실시간 필터 적용
- [ ] **학습 목표**: 실시간 오디오 처리 이해

---

### MVP 3.2.3: 오디오 시각화 (1주)

> 📅 **권장 기간**: 2025년 12월 20일 ~ 12월 26일
> **참조 문서**: `v3.2.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.2.3.1 | 웨이브폼 렌더링 | `design/v3.2.3-audio-visualization.md` | `frontend/src/audio/WaveformRenderer.ts` | Canvas 2D |
| 3.2.3.2 | 스펙트로그램 | 〃 | `frontend/src/audio/SpectrogramRenderer.ts` | 주파수 시각화 |
| 3.2.3.3 | 실시간 시각화 | 〃 | `frontend/src/audio/RealtimeVisualizer.ts` | 라이브 렌더링 |
| 3.2.3.4 | WebGL 오디오 시각화 | 〃 | `frontend/src/audio/WebGLAudioVisualizer.ts` | GPU 가속 |

#### 🔖 커밋 포인트
```bash
# 3.2.3.1 완료
git commit -m "feat(viz): implement waveform rendering with Canvas 2D"
# 3.2.3.2 완료
git commit -m "feat(viz): add spectrogram visualization"
# 3.2.3.3 완료
git commit -m "feat(viz): implement real-time audio visualization"
# 3.2.3.4 완료 → v3.2.3 태그
git commit -m "feat(viz): add WebGL-accelerated audio visualization"
git tag -a v3.2.3 -m "v3.2.3: Audio Visualization"
```

#### ✅ 완료 기준
- [ ] 웨이브폼 렌더링 동작
- [ ] 스펙트로그램 실시간 업데이트
- [ ] WebGL 가속 시각화
- [ ] **학습 목표**: 오디오 시각화 기법 이해

---

### MVP 3.2.4: WebAudio 성능 최적화 (1주)

> 📅 **권장 기간**: 2025년 12월 27일 ~ 2026년 1월 2일
> **참조 문서**: `v3.2.md`

#### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 | 학습 포커스 |
|------|------|-------------|----------|-------------|
| 3.2.4.1 | 오디오 버퍼 풀 | `design/v3.2.4-webaudio-performance.md` | `frontend/src/audio/AudioBufferPool.ts` | 버퍼 재사용 |
| 3.2.4.2 | 노드 재사용 | 〃 | `frontend/src/audio/NodePool.ts` | 노드 풀링 |
| 3.2.4.3 | 메모리 관리 | 〃 | `frontend/src/audio/AudioMemoryManager.ts` | GC 최소화 |
| 3.2.4.4 | WebAudio 프로파일링 | 〃 | `frontend/src/audio/AudioProfiler.ts` | 성능 측정 |

#### 🔖 커밋 포인트
```bash
# 3.2.4.1 완료
git commit -m "perf(audio): implement audio buffer pooling for reuse"
# 3.2.4.2 완료
git commit -m "perf(audio): add audio node pooling and reuse"
# 3.2.4.3 완료
git commit -m "perf(audio): optimize audio memory management"
# 3.2.4.4 완료 → v3.2.4 태그
git commit -m "perf(audio): add WebAudio performance profiling"
git tag -a v3.2.4 -m "v3.2.4: WebAudio Performance Optimization Complete"
```

#### ✅ 완료 기준
- [ ] 128 샘플 버퍼로 실시간 처리
- [ ] 메모리 사용 < 100MB
- [ ] 지연 시간 최소화
- [ ] **학습 목표**: 오디오 성능 최적화 이해

---

## 📊 Phase 3 Extended 진행 추적

### 시간 추정 vs 실제 소요 기록
| Phase | 예상 시간 | 실제 소요 | 차이 | 주요 교훈 |
|-------|----------|----------|------|----------|
| v3.1.0 | 2주 | | | |
| v3.1.1 | 1주 | | | |
| v3.1.2 | 1주 | | | |
| v3.1.3 | 1.5주 | | | |
| v3.1.4 | 1주 | | | |
| v3.2.0 | 2주 | | | |
| v3.2.1 | 1주 | | | |
| v3.2.2 | 1.5주 | | | |
| v3.2.3 | 1주 | | | |
| v3.2.4 | 1주 | | | |

---

## 🌐 브라우저 지원 (Phase 3 Extended)

### WebGL 요구사항
- Chrome 51+
- Firefox 45+
- Safari 10+
- Edge 79+

### WebAudio 요구사항
- Chrome 14+
- Firefox 25+
- Safari 6+
- Edge 12+

---

## 📚 추가 학습 자료

### WebGL
- [WebGL Fundamentals](https://webglfundamentals.org/)
- [WebGL 2.0 Specification](https://www.khronos.org/registry/webgl/specs/latest/2.0/)
- [GLSL ES Reference](https://www.khronos.org/files/opengles_shading_language.pdf)

### WebAudio
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)
- [Audio Worklet](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet)
- [Real-time Audio Processing](https://webaudio.github.io/web-audio-api/)