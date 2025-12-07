# 📅 5일 클론코딩 마스터 스케줄 (핵심 집중)

> **목표**: 2개 핵심 프로젝트 완전 클론코딩 + 2개 부가 프로젝트 (선택)
> **기간**: 5일 (Day 1 ~ Day 5) + 부가 2일 (선택)
> **전략**: 취업에 직결되는 핵심 프로젝트에 집중
> 
> ⚠️ **수정사항**: C++ 프로젝트(ray-tracer, modern-irc)는 부가 프로젝트로 분류

---

## 📊 포트폴리오 경쟁력 분석 결과

### 핵심 프로젝트 (필수)
| 프로젝트 | 최종 버전 | 예상 시간 | 취업 연관성 | 우선순위 |
|----------|----------|-----------|------------|----------|
| **spring-patterns** | v1.6.0 | ~18시간 | ⭐⭐⭐⭐⭐ | 1 (백엔드 필수) |
| **native-video-editor** | v3.2.4 | ~35시간 | ⭐⭐⭐⭐⭐ | 2 (풀스택 종합) |

### 부가 프로젝트 (선택/여유 시)
| 프로젝트 | 최종 버전 | 예상 시간 | 취업 연관성 | 비고 |
|----------|----------|-----------|------------|------|
| **modern-irc** | v3.0.0 | ~15시간 | ⭐⭐ | 네트워크/시스템 회사 지원 시 |
| **ray-tracer** | v3.0.0 | ~12시간 | ⭐ | 게임/그래픽스 회사 지원 시만 |

**핵심 프로젝트 총 예상 시간**: ~53시간 → **하루 평균 10.6시간 (5일)**

### 왜 C++ 프로젝트를 부가로 분류했는가?

| 프로젝트 | 장점 | 한계 |
|----------|------|------|
| ray-tracer | 수학적 사고력, 알고리즘 | 그래픽스 외 회사에서 무관 |
| modern-irc | 네트워크 프로그래밍, RFC 이해 | 대부분 웹 회사에서 저평가 |

> 💡 **결론**: spring-patterns + native-video-editor로 **백엔드 + 풀스택 역량**을 완벽히 증명 가능

---

## 📊 프로젝트별 소요시간 분석

| 프로젝트 | 총 버전 | 예상 시간 | 난이도 | 우선순위 |
|----------|---------|-----------|--------|----------|
| **spring-patterns** | v0.1.0 ~ v1.6.0 (8버전) | ~18시간 | ⭐⭐⭐⭐ | 1 (기초) |
| **ray-tracer** | v0.1.0 ~ v3.0.0 (4버전) | ~12시간 | ⭐⭐⭐ | 2 (독립) |
| **modern-irc** | v1.0.0 ~ v3.0.0 (3버전) | ~15시간 | ⭐⭐⭐ | 3 (네트워크) |
| **native-video-editor** | v0.1.0 ~ v3.2.4 (15+버전) | ~35시간 | ⭐⭐⭐⭐⭐ | 4 (종합) |

**총 예상 시간**: ~80시간 → **하루 평균 11.5시간**

> ⚠️ **중요**: native-video-editor는 v3.0.0 이후 WebGL(v3.1.x) 및 WebAudio(v3.2.x) 고급 기능이 포함되어 있습니다.

---

## 🔥 Day 1 (일요일) - Foundation Day

> **테마**: Spring Boot 기초 + Ray Tracer 시작
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Spring Patterns

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-10:00 | v0.1.1 | Spring Boot 프로젝트 생성 | `Application.java` |
| 10:00-10:30 | v0.1.2 | application.yml 설정 | 설정 파일 완성 |
| 10:30-11:00 | v0.1.3 | HealthController 구현 | `/api/health` 동작 |
| 11:00-11:30 | v0.1.4 | GlobalExceptionHandler | 에러 처리 |
| 11:30-12:00 | v0.1.5 | GitHub Actions CI | CI 파이프라인 |
| 12:00-13:00 | v0.1.6 | 기본 테스트 작성 → **v0.1.0 완료** | 테스트 통과 |

### 오후 세션 (3시간) - Spring Patterns 계속

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-15:00 | v1.0.1 | JPA Entity 정의 | `Issue.java`, `User.java` |
| 15:00-15:30 | v1.0.2 | Repository 인터페이스 | `IssueRepository.java` |
| 15:30-16:30 | v1.0.3 | Service Layer + 트랜잭션 | `IssueService.java` |
| 16:30-17:00 | v1.0.4 | Controller Layer | `IssueController.java` |

### 저녁 세션 (3시간) - Ray Tracer 시작

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v0.1.1 | CMakeLists.txt 설정 | 빌드 시스템 |
| 19:30-20:00 | v0.1.2 | 디렉토리 구조 | `src/`, `tests/` |
| 20:00-20:30 | v0.1.3 | PPM 출력 테스트 → **v0.1.0 완료** | 이미지 출력 |
| 20:30-21:00 | v1.0.1 | Vec3 클래스 | `vec3.h` |
| 21:00-21:30 | v1.0.2 | Ray 클래스 | `ray.h` |
| 21:30-22:00 | v1.0.3 | Sphere Hittable | `sphere.h` |

### Day 1 체크포인트 ✅
- [ ] spring-patterns: v0.1.0 완료, v1.0.0 50%
- [ ] ray-tracer: v0.1.0 완료, v1.0.0 30%

---

## 🔥 Day 2 (월요일) - Backend + Graphics Day

> **테마**: Spring CRUD 완성 + Ray Tracer 기본 렌더링
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Spring Patterns

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v1.0.5 | DTO & Mapper | `IssueDto.java` |
| 09:30-10:00 | v1.0.6 | 페이징 & 정렬 | Pageable 지원 |
| 10:00-10:30 | v1.0.7 | 트랜잭션 롤백 테스트 → **v1.0.0 완료** | 테스트 통과 |
| 10:30-11:30 | v1.1.1 | Team, TeamMember 엔티티 | `Team.java` |
| 11:30-12:30 | v1.1.2 | TeamService 구현 | `TeamService.java` |
| 12:30-13:00 | v1.1.3 | 역할 기반 권한 검증 | RBAC 동작 |

### 오후 세션 (3시간) - Spring Patterns 계속

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v1.1.4 | 401/403/404 예외 구분 | 예외 처리 |
| 14:30-15:00 | v1.1.5 | 통합 테스트 → **v1.1.0 완료** | 테스트 통과 |
| 15:00-15:30 | v1.2.1 | DailyIssueStats 엔티티 | `DailyIssueStats.java` |
| 15:30-16:00 | v1.2.2 | @Scheduled 배치 작업 | `StatsScheduler.java` |
| 16:00-16:30 | v1.2.3 | StatsService API | 통계 API |
| 16:30-17:00 | v1.2.4-5 | @Cacheable + 테스트 → **v1.2.0 완료** | 캐시 동작 |

### 저녁 세션 (3시간) - Ray Tracer 계속

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | v1.0.4 | HittableList | `hittable_list.h` |
| 19:30-20:00 | v1.0.5 | Camera 클래스 | `camera.h` |
| 20:00-20:30 | v1.0.6 | Lambertian Material | `lambertian.h` |
| 20:30-21:00 | v1.0.7 | Metal Material | `metal.h` |
| 21:00-21:30 | v1.0.8 | Dielectric Material | `dielectric.h` |
| 21:30-22:00 | v1.0.9 | Anti-aliasing → **v1.0.0 완료** | 렌더링 출력 |

### Day 2 체크포인트 ✅
- [ ] spring-patterns: v1.2.0 완료
- [ ] ray-tracer: v1.0.0 완료

---

## 🔥 Day 3 (화요일) - Elasticsearch + IRC 시작

> **테마**: Spring 검색 + IRC 네트워크 프로그래밍
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Spring Patterns

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v1.3.1 | Spring Data ES 설정 | 의존성 추가 |
| 09:30-10:00 | v1.3.2 | ProductDocument 정의 | `ProductDocument.java` |
| 10:00-11:00 | v1.3.3 | ProductSearchService | 검색 서비스 |
| 11:00-11:30 | v1.3.4 | 전문 검색 API | 검색 컨트롤러 |
| 11:30-12:00 | v1.3.5 | ES 동기화 → **v1.3.0 완료** | 검색 동작 |
| 12:00-13:00 | v1.4.1 | Order, OrderItem 엔티티 | `Order.java` |

### 오후 세션 (3시간) - Spring Patterns + modern-irc

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v1.4.2-3 | ApplicationEvent + Listener | 이벤트 정의 |
| 14:30-15:00 | v1.4.4-5 | @Async + 테스트 → **v1.4.0 완료** | 비동기 동작 |
| 15:00-16:00 | irc-v1.0.1 | TCP 소켓 서버 | `Socket.cpp` |
| 16:00-17:00 | irc-v1.0.2 | poll() 이벤트 루프 | `Poller.cpp` |

### 저녁 세션 (3시간) - modern-irc

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | irc-v1.0.3 | 클라이언트 연결 관리 | `Client.cpp` |
| 19:30-20:00 | irc-v1.0.4 | IRC 메시지 파싱 | `Message.cpp` |
| 20:00-20:30 | irc-v1.0.5 | 인증 (PASS/NICK/USER) | `AuthCommands.cpp` |
| 20:30-21:30 | irc-v1.0.6 | 채널 시스템 | `Channel.cpp` |
| 21:30-22:00 | irc-v1.0.7 | PRIVMSG → **irc v1.0.0 완료** | 메시지 동작 |

### Day 3 체크포인트 ✅
- [ ] spring-patterns: v1.4.0 완료
- [ ] modern-irc: v1.0.0 완료

---

## 🔥 Day 4 (수요일) - Docker + IRC Advanced

> **테마**: 프로덕션 인프라 + IRC 고급 기능
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Spring Patterns

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | v1.5.1 | PostgreSQL 마이그레이션 | `application-prod.yml` |
| 09:30-10:30 | v1.5.2 | Dockerfile (멀티스테이지) | `Dockerfile` |
| 10:30-11:30 | v1.5.3 | docker-compose.yml | 오케스트레이션 |
| 11:30-12:00 | v1.5.4 | Redis 캐시 연동 | Redis 동작 |
| 12:00-13:00 | v1.5.5 | 전체 스택 테스트 → **v1.5.0 완료** | Docker 실행 |

### 오후 세션 (3시간) - Spring Gateway + modern-irc

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | v1.6.1 | Gateway 모듈 생성 | `gateway/build.gradle` |
| 14:30-15:00 | v1.6.2 | Gateway 기본 설정 | `GatewayApplication.java` |
| 15:00-15:30 | v1.6.3 | 라우팅 규칙 정의 | 라우팅 동작 |
| 15:30-16:00 | v1.6.4 | 공통 필터 | `LoggingFilter.java` |
| 16:00-17:00 | v1.6.5 | Docker 통합 → **v1.6.0 완료** 🎉 | Gateway 완성 |

### 저녁 세션 (3시간) - modern-irc

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | irc-v2.0.1 | 채널 모드 (+i,+t,+k,+o,+l) | 모드 동작 |
| 19:30-20:30 | irc-v2.0.2 | 오퍼레이터 명령 | `OperatorCommands.cpp` |
| 20:30-21:00 | irc-v2.0.3 | 정보 명령 (WHOIS,WHO) | `InfoCommands.cpp` |
| 21:00-21:30 | irc-v2.0.4 | RFC 에러 코드 | 에러 응답 |
| 21:30-22:00 | irc-v2.0.5 | 토픽 영속화 → **irc v2.0.0 완료** | 토픽 저장 |

### Day 4 체크포인트 ✅
- [ ] spring-patterns: v1.6.0 완료 🎉 (프로젝트 완성!)
- [ ] modern-irc: v2.0.0 완료

---

## 🔥 Day 5 (목요일) - IRC Production + Video Editor 시작

> **테마**: IRC 완성 + 풀스택 비디오 에디터 시작
> **목표 시간**: 10시간

### 오전 세션 (4시간) - modern-irc 완성

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | irc-v3.0.1 | 설정 파일 시스템 | `Config.cpp` |
| 09:30-10:30 | irc-v3.0.2 | 로깅 시스템 | `Logger.cpp` |
| 10:30-11:00 | irc-v3.0.3 | Rate Limiting | Flood 보호 |
| 11:00-12:00 | irc-v3.0.4 | 관리자 명령 | `AdminCommands.cpp` |
| 12:00-13:00 | irc-v3.0.5 | 서버 통계 → **irc v3.0.0 완료** 🎉 | 통계 API |

### 오후 세션 (3시간) - native-video-editor + Ray Tracer

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | nve-v0.1.1 | 프로젝트 디렉토리 생성 | 구조 설정 |
| 14:30-15:00 | nve-v0.1.2 | Vite + React + Tailwind | 프론트엔드 설정 |
| 15:00-15:30 | nve-v0.1.3 | Express + TypeScript | 백엔드 설정 |
| 15:30-16:00 | nve-v0.1.4 | 로컬 실행 확인 → **nve v0.1.0 완료** | 개발 환경 |
| 16:00-17:00 | rt-v2.0.1-2 | AABB + BVH 노드 | 가속 구조 |

### 저녁 세션 (3시간) - native-video-editor

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | nve-v1.0.1 | VideoUpload 컴포넌트 | `VideoUpload.tsx` |
| 19:30-20:00 | nve-v1.0.2 | useVideoUpload 훅 | `useVideoUpload.ts` |
| 20:00-20:30 | nve-v1.0.3 | multer 업로드 라우트 | `upload.routes.ts` |
| 20:30-21:00 | nve-v1.0.4 | VideoPlayer 컴포넌트 | `VideoPlayer.tsx` |
| 21:00-21:30 | nve-v1.0.5 | Timeline 컴포넌트 | `Timeline.tsx` |
| 21:30-22:00 | nve-v1.0.6 | App.tsx 통합 → **nve v1.0.0 완료** | 기본 UI |

### Day 5 체크포인트 ✅
- [ ] modern-irc: v3.0.0 완료 🎉 (프로젝트 완성!)
- [ ] native-video-editor: v1.0.0 완료
- [ ] ray-tracer: v2.0.0 20%

---

## 🔥 Day 6 (금요일) - Video Editor Core + Ray Tracer 완성

> **테마**: 비디오 편집 기능 + 레이트레이서 완성
> **목표 시간**: 10시간

### 오전 세션 (4시간) - native-video-editor

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | nve-v1.1.1 | StorageService | `storage.service.ts` |
| 09:30-10:00 | nve-v1.1.2 | FFmpegService (trim/split) | `ffmpeg.service.ts` |
| 10:00-10:30 | nve-v1.1.3 | edit.routes.ts | 편집 API |
| 10:30-11:00 | nve-v1.1.4-5 | EditPanel + 훅 → **nve v1.1.0 완료** | Trim/Split |
| 11:00-11:30 | nve-v1.2.1-2 | SRT + FFmpeg 필터 | 자막 처리 |
| 11:30-12:00 | nve-v1.2.3-4 | SubtitleEditor + Speed → **nve v1.2.0 완료** | 자막/속도 |
| 12:00-13:00 | nve-v1.3.1-2 | WebSocket + 훅 | 실시간 연결 |

### 오후 세션 (3시간) - native-video-editor + Ray Tracer

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | nve-v1.3.3-4 | PostgreSQL + DB Service | 데이터베이스 |
| 14:30-15:00 | nve-v1.3.5-7 | Redis + Routes + Panel → **nve v1.3.0 완료** | 영속성 |
| 15:00-15:30 | rt-v2.0.3-4 | SolidColor + Checker 텍스처 | 텍스처 |
| 15:30-16:00 | rt-v2.0.5 | Image Texture | 이미지 텍스처 |
| 16:00-16:30 | rt-v2.0.6-7 | Quad + Motion Blur | 프리미티브 |
| 16:30-17:00 | rt-v2.0.8 | Volume → **rt v2.0.0 완료** | 볼류메트릭 |

### 저녁 세션 (3시간) - native-video-editor

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | nve-v2.0.1 | binding.gyp 설정 | 빌드 설정 |
| 19:30-20:00 | nve-v2.0.2 | RAII 래퍼 헤더 | `ffmpeg_raii.h` |
| 20:00-20:30 | nve-v2.0.3 | 메모리 풀 구현 | 메모리 관리 |
| 20:30-21:00 | nve-v2.0.4-5 | N-API 바인딩 → **nve v2.0.0 완료** | 네이티브 빌드 |
| 21:00-21:30 | nve-v2.1.1-3 | 썸네일 추출 | 고성능 썸네일 |
| 21:30-22:00 | nve-v2.1.4-5 | 캐시 + 테스트 → **nve v2.1.0 완료** | 썸네일 캐시 |

### Day 6 체크포인트 ✅
- [ ] ray-tracer: v2.0.0 완료
- [ ] native-video-editor: v2.1.0 완료

---

## 🔥 Day 7 (토요일) - Final Sprint

> **테마**: 모든 프로젝트 완성 + 테스트 + 문서화
> **목표 시간**: 10시간

### 오전 세션 (4시간) - Ray Tracer 완성 + Video Editor

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | rt-v3.0.1-2 | PDF 기본 + CosinePDF | PDF 클래스 |
| 09:30-10:00 | rt-v3.0.3-4 | HittablePDF + MixturePDF | 혼합 PDF |
| 10:00-10:30 | rt-v3.0.5-6 | DiffuseLight + Importance → **rt v3.0.0 완료** 🎉 | Monte Carlo |
| 10:30-11:00 | nve-v2.2.1-3 | 메타데이터 분석 | 코덱 분석 |
| 11:00-11:30 | nve-v2.2.4-5 | 통합 + 테스트 → **nve v2.2.0 완료** | 메타데이터 |
| 11:30-12:00 | nve-v2.3.1-2 | Prometheus 설정 | 메트릭 수집 |
| 12:00-13:00 | nve-v2.3.3-5 | 모니터링 → **nve v2.3.0 완료** | 대시보드 |

### 오후 세션 (3시간) - Video Editor v3.0.0 + WebGL 시작

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 14:00-14:30 | nve-v3.0.1-2 | Docker 설정 | Dockerfile |
| 14:30-15:00 | nve-v3.0.3-4 | docker-compose | 오케스트레이션 |
| 15:00-15:30 | nve-v3.0.5-7 | 문서화 + 테스트 → **nve v3.0.0 완료** | 배포 준비 |
| 15:30-16:00 | nve-v3.1.0.1-2 | WebGL 컨텍스트 초기화, 확장 지원 | `WebGLEngine.ts` |
| 16:00-17:00 | nve-v3.1.0.3-4 | WebGL 디버깅, 메모리 관리 → **nve v3.1.0 완료** | WebGL 코어 |

### 저녁 세션 (3시간) - WebGL 고급 기능

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 19:00-19:30 | nve-v3.1.1 | 셰이더 시스템 | `ShaderManager.ts` |
| 19:30-20:00 | nve-v3.1.2 | 텍스처 관리 | `TextureManager.ts` |
| 20:00-20:30 | nve-v3.1.3 | 렌더링 파이프라인 | `RenderPipeline.ts` |
| 20:30-21:00 | nve-v3.1.4 | WebGL 성능 최적화 → **nve v3.1.4 완료** | GPU 최적화 |
| 21:00-22:00 | 테스트 | spring-patterns, ray-tracer, modern-irc 테스트 | 테스트 통과 |

### Day 7 체크포인트 ✅
- [ ] ray-tracer: v3.0.0 완료 🎉 (프로젝트 완성!)
- [ ] native-video-editor: v3.1.4 완료 (WebGL 완성)
- [ ] spring-patterns, ray-tracer, modern-irc 테스트 통과

---

## 🔥 Day 8 (일요일) - WebAudio & Final Sprint

> **테마**: WebAudio 완성 + 최종 마무리
> **목표 시간**: 8시간

### 오전 세션 (4시간) - WebAudio 기본

| 시간 | 버전 | 작업 | 산출물 |
|------|------|------|--------|
| 09:00-09:30 | nve-v3.2.0.1-2 | AudioContext 초기화, 상태 관리 | `AudioEngine.ts` |
| 09:30-10:00 | nve-v3.2.0.3-4 | 오디오 그래프, 라우팅 → **nve v3.2.0 완료** | 오디오 코어 |
| 10:00-10:30 | nve-v3.2.1 | 오디오 노드 시스템 | `AudioNodeManager.ts` |
| 10:30-11:00 | nve-v3.2.2 | 실시간 오디오 처리 | `RealtimeProcessor.ts` |
| 11:00-11:30 | nve-v3.2.3 | 오디오 시각화 | `AudioVisualizer.ts` |
| 11:30-12:00 | nve-v3.2.4.1-2 | 버퍼 풀, 노드 풀 | 메모리 최적화 |
| 12:00-13:00 | nve-v3.2.4.3-4 | 성능 프로파일링 → **nve v3.2.4 완료** 🎉 | WebAudio 완성 |

### 오후 세션 (4시간) - 최종 마무리

| 시간 | 작업 | 산출물 |
|------|------|--------|
| 14:00-15:00 | native-video-editor 전체 테스트 | 테스트 통과 |
| 15:00-16:00 | CI/CD 파이프라인 최종 검증 | GitHub Actions 통과 |
| 16:00-17:00 | README 업데이트 | 최신 문서 |
| 17:00-18:00 | PORTFOLIO.md 완성 | 포트폴리오 문서 |

### Day 8 체크포인트 ✅
- [ ] native-video-editor: v3.2.4 완료 🎉 (프로젝트 완성!)
- [ ] 모든 테스트 통과
- [ ] CI/CD 파이프라인 동작
- [ ] 문서화 완료

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

## 🎯 프로젝트 완료 순서

```
Day 4 저녁 → spring-patterns v1.6.0 완료 🎉
Day 5 오전 → modern-irc v3.0.0 완료 🎉
Day 7 오전 → ray-tracer v3.0.0 완료 🎉
Day 7 저녁 → native-video-editor v3.1.4 완료 (WebGL)
Day 8 오전 → native-video-editor v3.2.4 완료 🎉 (WebAudio)
```

---

## ⚠️ 리스크 관리

### 시간 부족 시 우선순위
1. **필수**: spring-patterns (취업 핵심)
2. **중요**: modern-irc (네트워크 역량)
3. **선택**: ray-tracer (그래픽스 이해)
4. **고급**: native-video-editor (종합 역량)

### 막혔을 때 대처
- 30분 이상 막히면 → 스킵 후 다음 버전으로
- 저녁에 돌아와서 재시도
- 다음 날 아침에 fresh 하게 접근

---

## 📊 총 버전 수 및 커밋 포인트

| 프로젝트 | 패치 버전 수 | 주요 태그 |
|----------|-------------|----------|
| spring-patterns | 35+ | v0.1.0, v1.0.0 ~ v1.6.0 |
| ray-tracer | 20+ | v0.1.0, v1.0.0 ~ v3.0.0 |
| modern-irc | 17+ | v1.0.0 ~ v3.0.0 |
| native-video-editor | 60+ | v0.1.0, v1.0.0 ~ v3.2.4 |

**총 커밋 포인트**: ~132개
