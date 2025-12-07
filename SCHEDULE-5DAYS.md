# 📅 5일 클론코딩 마스터 스케줄 (핵심 집중)

> **목표**: 2개 핵심 프로젝트 완전 클론코딩
> **기간**: 5일 (Day 1 ~ Day 5)
> **전략**: 취업에 직결되는 핵심 프로젝트에 100% 집중

---

## 📊 포트폴리오 경쟁력 분석 결과

### 핵심 프로젝트 (필수)
| 프로젝트 | 최종 버전 | 예상 시간 | 취업 연관성 | 우선순위 |
|----------|----------|-----------|------------|----------|
| **spring-patterns** | v2.3.0 | ~25시간 | ⭐⭐⭐⭐⭐ | 1 (백엔드 필수) |
| **native-video-editor** | v3.2.4 | ~35시간 | ⭐⭐⭐⭐⭐ | 2 (풀스택 종합) |

### 부가 프로젝트 (시간 여유 시)
| 프로젝트 | 최종 버전 | 예상 시간 | 취업 연관성 | 비고 |
|----------|----------|-----------|------------|------|
| **modern-irc** | v3.0.0 | ~15시간 | ⭐⭐ | 네트워크/시스템 회사 지원 시 |
| **ray-tracer** | v3.0.0 | ~12시간 | ⭐ | 게임/그래픽스 회사 지원 시만 |

**핵심 프로젝트 총 예상 시간**: ~60시간 → **하루 평균 12시간 (5일)**

---

### 💡 spring-patterns v1.6.0 경쟁력 평가

**v1.6.0은 충분히 경쟁력 있습니다!**

| 역량 | 현재 상태 | 증명 수준 |
|------|----------|----------|
| Spring Boot 기본 | Layered CRUD, 트랜잭션 | ⭐⭐⭐⭐⭐ |
| 데이터베이스/JPA | N+1 해결, 관계 매핑 | ⭐⭐⭐⭐ |
| 캐싱/검색 | Redis + Elasticsearch | ⭐⭐⭐⭐ |
| 이벤트/비동기 | @Async, @EventListener | ⭐⭐⭐⭐ |
| 컨테이너화 | Docker Compose 풀스택 | ⭐⭐⭐⭐ |
| API Gateway | Spring Cloud Gateway | ⭐⭐⭐ |

> ✅ **결론**: v2.x 리액티브 전환 없이도 대부분의 백엔드 포지션에 충분

---

### 🤔 C++ 프로젝트를 부가로 분류한 이유

| 프로젝트 | 장점 | 한계 |
|----------|------|------|
| ray-tracer | 수학적 사고력, 알고리즘 | 그래픽스/게임 외 회사에서 **무관** |
| modern-irc | 네트워크 프로그래밍, RFC 이해 | 대부분 웹 회사에서 **저평가** |

> 💡 **spring-patterns + native-video-editor** 조합으로:
> - 백엔드 역량 (Spring Boot, JPA, 캐싱, 검색)
> - 풀스택 역량 (React, Node.js, TypeScript)
> - 시스템 역량 (C++ FFmpeg, 네이티브 애드온)
> - DevOps 역량 (Docker, CI/CD, 모니터링)
> 
> **→ 모든 주요 역량을 2개 프로젝트로 증명 가능!**

---

## 🔥 Day 1 - Spring Boot Foundation

> **테마**: Spring Boot 기초 ~ v1.2.0 (Batch & Cache)
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Bootstrap & CRUD

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-10:00 | v0.1.1~2 | Spring Boot 프로젝트 생성, 설정 | `Application.java` |
| 10:00-10:30 | v0.1.3 | HealthController 구현 | `/api/health` 동작 |
| 10:30-11:00 | v0.1.4 | GlobalExceptionHandler | 에러 처리 |
| 11:00-11:30 | v0.1.5 | GitHub Actions CI | CI 파이프라인 |
| 11:30-12:00 | v0.1.6 | 기본 테스트 작성 → **v0.1.0 완료** | 테스트 통과 |
| 12:00-13:00 | v1.0.1~2 | JPA Entity, Repository | `Issue.java` |

### 오후 세션 (3시간) - Layered CRUD

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-15:00 | v1.0.3~4 | Service + Controller Layer | `IssueService.java` |
| 15:00-15:30 | v1.0.5 | DTO & Mapper | `IssueDto.java` |
| 15:30-16:00 | v1.0.6 | 페이징 & 정렬 | Pageable 지원 |
| 16:00-17:00 | v1.0.7 | 트랜잭션 롤백 테스트 → **v1.0.0 완료** | 테스트 통과 |

### 저녁 세션 (3시간) - Team & RBAC + Batch

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v1.1.1 | Team, TeamMember 엔티티 | `Team.java` |
| 19:30-20:00 | v1.1.2 | TeamService 구현 | `TeamService.java` |
| 20:00-20:30 | v1.1.3~4 | RBAC + 예외 처리 | 권한 검증 |
| 20:30-21:00 | v1.1.5 | 통합 테스트 → **v1.1.0 완료** | 테스트 통과 |
| 21:00-21:30 | v1.2.1~2 | DailyIssueStats + @Scheduled | 배치 작업 |
| 21:30-22:00 | v1.2.3~5 | @Cacheable + 테스트 → **v1.2.0 완료** | 캐시 동작 |

### Day 1 체크포인트 ✅
- [ ] spring-patterns: v0.1.0 완료
- [ ] spring-patterns: v1.0.0 완료
- [ ] spring-patterns: v1.1.0 완료
- [ ] spring-patterns: v1.2.0 완료

---

## 🔥 Day 2 - Spring Boot Production Ready

> **테마**: Elasticsearch, Events, Docker, Gateway + WebFlux, Virtual Threads, Rate Limiting, Caching → **v2.3.0 완료**
> **목표 시간**: 15시간

### 오전 세션 (4시간) - Elasticsearch & Events

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v1.3.1~2 | Spring Data ES, ProductDocument | ES 설정 |
| 09:30-10:30 | v1.3.3~4 | ProductSearchService + API | 검색 서비스 |
| 10:30-11:00 | v1.3.5 | ES 동기화 → **v1.3.0 완료** | 검색 동작 |
| 11:00-11:30 | v1.4.1~2 | Order 엔티티 + ApplicationEvent | 이벤트 정의 |
| 11:30-12:00 | v1.4.3 | @EventListener | 이벤트 핸들러 |
| 12:00-13:00 | v1.4.4~5 | @Async + 테스트 → **v1.4.0 완료** | 비동기 동작 |

### 오후 세션 (4시간) - Docker Production & WebFlux Migration

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v1.5.1 | PostgreSQL 마이그레이션 | `application-prod.yml` |
| 14:30-15:00 | v1.5.2 | Dockerfile (멀티스테이지) | `Dockerfile` |
| 15:00-15:30 | v1.5.3 | docker-compose.yml | 오케스트레이션 |
| 15:30-16:00 | v1.5.4 | Redis 캐시 연동 | Redis 동작 |
| 16:00-16:30 | v1.5.5 | 전체 스택 테스트 → **v1.5.0 완료** | Docker 실행 |
| 16:30-17:00 | v2.0.1~2 | WebFlux + R2DBC 의존성 | `build.gradle` |
| 17:00-17:30 | v2.0.3~4 | 리액티브 컨트롤러/서비스 | `ProductController.java` |

### 저녁 세션 (7시간) - Virtual Threads & Advanced Features

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v2.0.5 | SSE 스트리밍 → **v2.0.0 완료** | 리액티브 전환 |
| 19:30-20:00 | v2.1.0.1~2 | Virtual Threads 설정 | `VirtualThreadConfig.java` |
| 20:00-20:30 | v2.1.1.1~2 | 블로킹 코드 래핑 | `HybridProductService.java` |
| 20:30-21:00 | v2.1.2.1~3 | 외부 API 병렬 호출 → **v2.1.0 완료** | Virtual Threads |
| 21:00-21:30 | v2.2.0.1~2 | Redis Rate Limiter | `RateLimitingFilter.java` |
| 21:30-22:00 | v2.2.0.3~4 | Rate Limit 헤더 → **v2.2.0 완료** | Rate Limiting |
| 22:00-22:30 | v2.3.0.1~2 | Redis Cache 설정 | `CachingConfig.java` |
| 22:30-23:00 | v2.3.0.3~4 | Gzip 압축 필터 → **v2.3.0 완료 🎉** | Caching & Compression |
| 23:00-23:30 | 정리 | 고성능 테스트 & 검증 | spring-patterns 완료!

### Day 2 체크포인트 ✅
- [ ] spring-patterns: v1.3.0 완료 (Elasticsearch)
- [ ] spring-patterns: v1.4.0 완료 (Async Events)
- [ ] spring-patterns: v1.5.0 완료 (Docker)
- [ ] spring-patterns: v2.0.0 완료 (WebFlux & R2DBC)
- [ ] spring-patterns: v2.1.0 완료 (Virtual Threads)
- [ ] spring-patterns: v2.2.0 완료 (Rate Limiting)
- [ ] spring-patterns: v2.3.0 완료 🎉 **(고성능 프로젝트 완성!)**

---

## 🔥 Day 3 - Video Editor Foundation

> **테마**: native-video-editor 기본 인프라 ~ v1.3.0
> **목표 시간**: 11시간

### 오전 세션 (4시간) - Bootstrap & Basic UI

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v0.1.1~2 | 프로젝트 디렉토리, Vite + React | 프론트엔드 설정 |
| 09:30-10:00 | v0.1.3~4 | Express + TypeScript → **v0.1.0 완료** | 개발 환경 |
| 10:00-10:30 | v1.0.1~2 | VideoUpload + useVideoUpload | 업로드 컴포넌트 |
| 10:30-11:00 | v1.0.3 | multer 업로드 라우트 | `upload.routes.ts` |
| 11:00-11:30 | v1.0.4 | VideoPlayer 컴포넌트 | `VideoPlayer.tsx` |
| 11:30-12:00 | v1.0.5 | Timeline 컴포넌트 | `Timeline.tsx` |
| 12:00-13:00 | v1.0.6 | App.tsx 통합 → **v1.0.0 완료** | 기본 UI |

### 오후 세션 (4시간) - Trim, Split, Subtitle, Speed

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v1.1.1~2 | StorageService + FFmpegService | 서비스 구현 |
| 14:30-15:00 | v1.1.3 | edit.routes.ts | 편집 API |
| 15:00-15:30 | v1.1.4~5 | EditPanel + 훅 → **v1.1.0 완료** | Trim/Split |
| 15:30-16:00 | v1.2.1~2 | SRT + FFmpeg 필터 | 자막 처리 |
| 16:00-16:30 | v1.2.3~4 | SubtitleEditor + Speed → **v1.2.0 완료** | 자막/속도 |
| 16:30-17:00 | v1.3.1~2 | WebSocket + 훅 | 실시간 연결 |

### 저녁 세션 (3시간) - WebSocket & Persistence

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v1.3.3~4 | PostgreSQL + DB Service | 데이터베이스 |
| 19:30-20:00 | v1.3.5 | Redis 캐시 | 캐시 구현 |
| 20:00-20:30 | v1.3.6 | project.routes.ts | 프로젝트 API |
| 20:30-21:00 | v1.3.7 | ProjectPanel UI → **v1.3.0 완료** | 영속성 |
| 21:00-22:00 | 정리 | Phase 1 테스트 & 검증 | Phase 1 완료! |

### Day 3 체크포인트 ✅
- [ ] native-video-editor: v0.1.0 완료 (Bootstrap)
- [ ] native-video-editor: v1.0.0 완료 (Basic UI)
- [ ] native-video-editor: v1.1.0 완료 (Trim/Split)
- [ ] native-video-editor: v1.2.0 완료 (Subtitle/Speed)
- [ ] native-video-editor: v1.3.0 완료 (WebSocket/Persistence)

---

## 🔥 Day 4 - Native Addon & Production

> **테마**: C++ 네이티브 애드온 ~ v3.0.0 (Production)
> **목표 시간**: 11시간

### 오전 세션 (4시간) - Native Addon Setup

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v2.0.1 | binding.gyp 설정 | 빌드 설정 |
| 09:30-10:00 | v2.0.2 | RAII 래퍼 헤더 | `ffmpeg_raii.h` |
| 10:00-10:30 | v2.0.3 | 메모리 풀 구현 | 메모리 관리 |
| 10:30-11:00 | v2.0.4~5 | N-API 바인딩 → **v2.0.0 완료** | 네이티브 빌드 |
| 11:00-11:30 | v2.1.1~3 | 썸네일 추출 | 고성능 썸네일 |
| 11:30-12:00 | v2.1.4~5 | 캐시 + 테스트 → **v2.1.0 완료** | 썸네일 캐시 |
| 12:00-13:00 | v2.2.1~3 | 메타데이터 분석 | 코덱 분석 |

### 오후 세션 (4시간) - Monitoring & Production

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v2.2.4~5 | 통합 + 테스트 → **v2.2.0 완료** | 메타데이터 |
| 14:30-15:00 | v2.3.1~2 | Prometheus 메트릭 서비스 | 메트릭 수집 |
| 15:00-15:30 | v2.3.3~4 | Grafana 대시보드 | 시각화 |
| 15:30-16:00 | v2.3.5 | 알림 설정 → **v2.3.0 완료** | 모니터링 |
| 16:00-16:30 | v3.0.1~2 | Dockerfile 작성 | 컨테이너화 |
| 16:30-17:00 | v3.0.3~4 | docker-compose | 오케스트레이션 |

### 저녁 세션 (3시간) - Production & WebGL Start

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v3.0.5~7 | 문서화 + 테스트 → **v3.0.0 완료** | 배포 준비 |
| 19:30-20:00 | v3.1.0.1~2 | WebGL 컨텍스트, 확장 지원 | `WebGLEngine.ts` |
| 20:00-20:30 | v3.1.0.3~4 | WebGL 디버깅, 메모리 → **v3.1.0 완료** | WebGL 코어 |
| 20:30-21:00 | v3.1.1 | 셰이더 시스템 | `ShaderManager.ts` |
| 21:00-21:30 | v3.1.2 | 텍스처 관리 | `TextureManager.ts` |
| 21:30-22:00 | v3.1.3~4 | 렌더링 파이프라인 → **v3.1.4 완료** | WebGL 완성 |

### Day 4 체크포인트 ✅
- [ ] native-video-editor: v2.0.0 완료 (Native Addon)
- [ ] native-video-editor: v2.1.0 완료 (Thumbnail)
- [ ] native-video-editor: v2.2.0 완료 (Metadata)
- [ ] native-video-editor: v2.3.0 완료 (Monitoring)
- [ ] native-video-editor: v3.0.0 완료 (Production)
- [ ] native-video-editor: v3.1.4 완료 (WebGL)

---

## 🔥 Day 5 - WebAudio & Final

> **테마**: WebAudio 완성 + 최종 마무리 → **v3.2.4 완료**
> **목표 시간**: 10시간

### 오전 세션 (4시간) - WebAudio Engine

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v3.2.0.1~2 | AudioContext 초기화, 상태 관리 | `AudioEngine.ts` |
| 09:30-10:00 | v3.2.0.3~4 | 오디오 그래프, 라우팅 → **v3.2.0 완료** | 오디오 코어 |
| 10:00-10:30 | v3.2.1 | 오디오 노드 시스템 | `AudioNodeManager.ts` |
| 10:30-11:00 | v3.2.2 | 실시간 오디오 처리 | `RealtimeProcessor.ts` |
| 11:00-11:30 | v3.2.3 | 오디오 시각화 | `AudioVisualizer.ts` |
| 11:30-12:00 | v3.2.4.1~2 | 버퍼 풀, 노드 풀 | 메모리 최적화 |
| 12:00-13:00 | v3.2.4.3~4 | 성능 프로파일링 → **v3.2.4 완료 🎉** | WebAudio 완성 |

### 오후 세션 (3시간) - 테스트 & 검증

| 시간 | 작업 | 산출물 |
|------|------|--------|
| 14:00-15:00 | spring-patterns 전체 테스트 | 테스트 통과 |
| 15:00-16:00 | native-video-editor 전체 테스트 | 테스트 통과 |
| 16:00-17:00 | CI/CD 파이프라인 검증 | GitHub Actions 통과 |

### 저녁 세션 (3시간) - 문서화 & 완료

| 시간 | 작업 | 산출물 |
|------|------|--------|
| 19:00-20:00 | README 업데이트 (두 프로젝트) | 최신 문서 |
| 20:00-21:00 | PORTFOLIO.md 완성 | 포트폴리오 문서 |
| 21:00-22:00 | 최종 점검 & 커밋 | 완료! |

### Day 5 체크포인트 ✅
- [ ] native-video-editor: v3.2.0 완료 (Audio Core)
- [ ] native-video-editor: v3.2.4 완료 🎉 **(프로젝트 완성!)**
- [ ] 모든 테스트 통과
- [ ] CI/CD 파이프라인 동작
- [ ] 문서화 완료

---

## 🎯 프로젝트 완료 순서

```
Day 2 저녁 → spring-patterns v1.6.0 완료 🎉
Day 5 오전 → native-video-editor v3.2.4 완료 🎉
```

---

## 📋 일일 체크리스트

### 매일 아침 (09:00)
- [ ] 전날 작업 내용 복습 (10분)
- [ ] 오늘 목표 확인 (5분)
- [ ] 개발 환경 확인 (5분)

### 매일 저녁 (22:00)
- [ ] 코드 커밋 & 푸시
- [ ] 진행률 기록
- [ ] 내일 계획 조정

---

## ⚠️ 리스크 관리

### 시간 부족 시 우선순위
1. **필수**: spring-patterns v1.6.0 (취업 핵심)
2. **필수**: native-video-editor v3.0.0 (Production)
3. **선택**: native-video-editor v3.1.x (WebGL)
4. **선택**: native-video-editor v3.2.x (WebAudio)

### 막혔을 때 대처
- 30분 이상 막히면 → 스킵 후 다음 버전으로
- 저녁에 돌아와서 재시도
- 다음 날 아침에 fresh 하게 접근

---

## 📊 총 버전 수 및 커밋 포인트

| 프로젝트 | 패치 버전 수 | 주요 태그 |
|----------|-------------|----------|
| spring-patterns | 35+ | v0.1.0 ~ v1.6.0 |
| native-video-editor | 60+ | v0.1.0 ~ v3.2.4 |

**핵심 프로젝트 총 커밋 포인트**: ~95개

---

## 🔄 부가 프로젝트 (여유 시 추가)

### Day 6-7 (선택): C++ 프로젝트

시간 여유가 있다면 아래 프로젝트를 추가로 진행:

| Day | 프로젝트 | 범위 | 시간 |
|-----|----------|------|------|
| Day 6 | modern-irc | v1.0.0 ~ v3.0.0 | 10시간 |
| Day 7 | ray-tracer | v0.1.0 ~ v3.0.0 | 10시간 |

> **참고**: 이 프로젝트들은 네트워크/시스템/게임/그래픽스 회사 지원 시에만 필요합니다.
> 웹 백엔드/풀스택 포지션에는 핵심 2개 프로젝트로 충분합니다.
