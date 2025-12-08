# 🗓️ 7일 클론코딩 스케줄 가이드

> 3개 프로젝트를 각각 7일 안에 클론코딩할 수 있도록 구성한 상세 스케줄

---

## 📌 공통 가이드라인

### 하루 학습 시간 가정
- **풀타임**: 8-10시간/일
- **파트타임**: 4-5시간/일 (14일로 조정 필요)

### 학습 전략
1. **사전 학습**: Design 문서 먼저 읽기 (30분)
2. **코딩**: 핵심 기능 구현 (6-8시간)
3. **검증**: 테스트 & 완료 기준 확인 (1시간)
4. **커밋**: 명확한 커밋 메시지 작성

---

## 🎮 프로젝트 1: cpp-pvp-server (7일 스케줄)

> **난이도**: ⭐⭐⭐⭐⭐ (상급)  
> **기술 스택**: C++17, Boost.Asio/Beast, PostgreSQL, Redis, UDP, WebSocket

### Day 1: Bootstrap & 기본 서버 기초 (v0.1.0 ~ v1.0.0-p1)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v0.1.0, v1.0.0) | 이해 |
| 10:00-12:00 | v0.1.0 | CMakeLists.txt, vcpkg.json 설정 | 빌드 환경 |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v0.1.1 | 디렉토리 구조 생성, 빌드 스크립트 | `server/src/`, `server/include/` |
| 15:00-18:00 | v1.0.0-p1 | WebSocket 서버 기초 (Boost.Beast) | `ws_server.cpp` |
| 18:00-19:00 | v1.0.0-p2 | Connection Manager 시작 | `connection_manager.cpp` 시작 |

**완료 기준**:
- [ ] CMake 빌드 성공
- [ ] vcpkg 의존성 설치 완료
- [ ] WebSocket 서버 연결 테스트 (wscat)

### Day 2: 게임 서버 완성 (v1.0.0-p2 ~ v1.0.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-12:00 | v1.0.0-p2 | Connection Manager 완성 | `connection_manager.cpp` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v1.0.0-p3 | GameLoop (60 TPS) 구현 | `game_loop.cpp` |
| 15:00-17:00 | v1.0.0-p4 | GameSession 관리 | `game_session.cpp` |
| 17:00-18:00 | v1.0.0-p5 | 플레이어 이동 처리 | `player.cpp` |
| 18:00-19:00 | v1.0.0-p6 | PostgreSQL 연동 기초 | `postgres_client.cpp` |

**완료 기준**:
- [ ] 2명 동시 접속 가능
- [ ] 60 TPS 고정 틱레이트 유지
- [ ] WASD 이동 동기화

**커밋**:
```bash
git commit -m "feat(network): implement WebSocket server with boost.beast"
git commit -m "feat(core): implement 60 TPS game loop with fixed timestep"
git tag -a v1.0.0 -m "v1.0.0: Basic Game Server"
```

### Day 3: 전투 시스템 (v1.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v1.1.0-combat) | 이해 |
| 10:00-12:00 | v1.1.0-p1 | Combat 컴포넌트 | `combat.cpp` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v1.1.0-p2 | Projectile 시스템 (30m/s) | `projectile.cpp` |
| 15:00-17:00 | v1.1.0-p3 | 원-원 충돌 검출 | `collision.cpp` |
| 17:00-18:00 | v1.1.0-p4 | 데미지 계산 | `damage.cpp` |
| 18:00-19:00 | v1.1.0-p5 | HP/사망 처리 | `health.cpp` |

**완료 기준**:
- [ ] 스페이스바 → 투사체 발사
- [ ] 투사체 충돌 검출 정확
- [ ] HP 0 → 사망 → 3초 후 리스폰

**커밋**:
```bash
git commit -m "feat(combat): implement projectile spawning and trajectory"
git commit -m "feat(combat): add collision detection and damage system"
git tag -a v1.1.0 -m "v1.1.0: Combat System"
```

### Day 4: 매치메이킹 & 통계 (v1.2.0 ~ v1.3.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v1.2.0, v1.3.0) | 이해 |
| 10:00-12:00 | v1.2.0-p1 | MatchmakingService | `matchmaker.cpp` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.2.0-p2 | Redis 큐 연동 | `redis_client.cpp` |
| 14:30-16:00 | v1.2.0-p3 | ELO 계산 | `elo.cpp` |
| 16:00-17:30 | v1.3.0-p1 | 통계 집계 서비스 | `stats_service.cpp` |
| 17:30-19:00 | v1.3.0-p2 | 랭킹 API | `ranking.cpp` |

**완료 기준**:
- [ ] 대기열 참가 → 매칭 완료 < 30초
- [ ] 비슷한 ELO끼리 매칭
- [ ] TOP 100 랭킹 조회

**커밋**:
```bash
git commit -m "feat(matchmaking): implement ELO-based matchmaking"
git tag -a v1.2.0 -m "v1.2.0: Matchmaking"
git commit -m "feat(stats): implement player statistics and ranking"
git tag -a v1.3.0 -m "v1.3.0: Statistics & Ranking"
```

### Day 5: UDP 넷코드 (v1.4.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v1.4.0-udp-netcode) | 이해 |
| 10:00-12:30 | v1.4.0-p1 | UDP 소켓 래퍼 | `udp_socket.cpp` |
| 12:30-13:30 | - | 점심 | - |
| 13:30-16:00 | v1.4.0-p2 | UDP 게임 서버 코어 | `udp_game_server.cpp` |
| 16:00-18:00 | v1.4.0-p3 | 스냅샷/델타 동기화 | `snapshot_manager.cpp` |
| 18:00-19:00 | v1.4.0-p4 | 패킷 타입 시스템 | 패킷 구조 정의 |

**완료 기준**:
- [ ] UDP 소켓으로 패킷 송수신 가능
- [ ] 60 TPS 스냅샷 전송
- [ ] 델타 압축으로 대역폭 최적화

**커밋**:
```bash
git commit -m "feat(udp): implement UDP socket wrapper with Boost.Asio"
git commit -m "feat(udp): add snapshot-delta synchronization"
git tag -a v1.4.0 -m "v1.4.0: UDP Netcode Core"
```

### Day 6: 예측 & 분산 시스템 (v1.4.1 ~ v2.0.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-11:00 | v1.4.1-p1 | 클라이언트 예측 엔진 | `client_prediction.cpp` |
| 11:00-12:30 | v1.4.1-p2 | 리컨실리에이션 시스템 | `reconciliation.cpp` |
| 12:30-13:30 | - | 점심 | - |
| 13:30-15:00 | v1.4.1-p3 | 입력 버퍼링, 지연 보상 | `input_buffer.cpp`, `lag_compensation.cpp` |
| 15:00-16:30 | v2.0.0-p1 | SessionStore 인터페이스 | `session_store.h` |
| 16:30-18:00 | v2.0.0-p2 | InMemorySessionStore | `in_memory_session_store.cpp` |
| 18:00-19:00 | v2.0.0-p3 | RedisSessionStore | `redis_session_store.cpp` |

**완료 기준**:
- [ ] 클라이언트 예측으로 즉각적인 입력 반응
- [ ] 서버 상태 불일치 시 부드러운 보정
- [ ] Redis 세션 스토어 동작

**커밋**:
```bash
git commit -m "feat(prediction): implement client-side prediction and reconciliation"
git tag -a v1.4.1 -m "v1.4.1: Client Prediction & Reconciliation"
git commit -m "feat(storage): implement SessionStore with Redis"
git tag -a v2.0.0 -m "v2.0.0: Session Server Separation"
```

### Day 7: 안티치트 & 마무리 (v2.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v2.1-anticheat) | 이해 |
| 10:00-12:00 | v2.1.0-p1 | HitValidator | `hit_validator.cpp` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v2.1.0-p2 | MovementValidator | `movement_validator.cpp` |
| 14:30-16:00 | v2.1.0-p3 | AnomalyDetector | `anomaly_detector.cpp` |
| 16:00-17:30 | v2.1.0-p4 | BanService | `ban_service.cpp` |
| 17:30-19:00 | - | 전체 테스트 & 문서화 | README 업데이트 |

**완료 기준**:
- [ ] 히트 검증 동작 (서버 측 레이캐스트)
- [ ] 스피드핵, 텔레포트 탐지
- [ ] 밴 시스템 동작
- [ ] 모든 테스트 통과

**커밋**:
```bash
git commit -m "feat(anticheat): implement validation and ban system"
git tag -a v2.1.0 -m "v2.1.0: Anti-Cheat System"
```

---

## 🎬 프로젝트 2: native-video-editor (7일 스케줄)

> **난이도**: ⭐⭐⭐⭐ (중상급)  
> **기술 스택**: React 18, Node.js 20, TypeScript 5, C++17, FFmpeg C API, WebGL 2.0, WebAudio API

### Day 1: Bootstrap & 기본 인프라 (v0.1.0 ~ v1.0.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v0.1.0, v1.0.0) | 이해 |
| 10:00-11:30 | v0.1.0-p1 | 프로젝트 디렉토리 생성 | Monorepo 구조 |
| 11:30-13:00 | v0.1.0-p2 | Vite + React + TailwindCSS 설정 | `frontend/` |
| 13:00-14:00 | - | 점심 | - |
| 14:00-15:30 | v0.1.0-p3 | Express + TypeScript 설정 | `backend/` |
| 15:30-17:00 | v1.0.0-p1 | VideoUpload 컴포넌트 | `VideoUpload.tsx` |
| 17:00-18:30 | v1.0.0-p2 | useVideoUpload 훅 | `useVideoUpload.ts` |
| 18:30-19:00 | v1.0.0-p3 | multer 업로드 라우트 | `upload.routes.ts` |

**완료 기준**:
- [ ] Frontend: http://localhost:5173 실행
- [ ] Backend: http://localhost:3001 실행
- [ ] 파일 업로드 API 동작

**커밋**:
```bash
git commit -m "chore: initialize video-editor project structure"
git tag -a v0.1.0 -m "v0.1.0: Bootstrap"
```

### Day 2: 기본 인프라 완성 & 트림 (v1.0.0 ~ v1.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-11:00 | v1.0.0-p4 | VideoPlayer 컴포넌트 | `VideoPlayer.tsx` |
| 11:00-13:00 | v1.0.0-p5 | Timeline (Canvas) 컴포넌트 | `Timeline.tsx` |
| 13:00-14:00 | - | 점심 | - |
| 14:00-15:00 | v1.0.0-p6 | App.tsx 통합 | 컴포넌트 통합 |
| 15:00-16:30 | v1.1.0-p1 | StorageService 구현 | `storage.service.ts` |
| 16:30-18:00 | v1.1.0-p2 | FFmpegService (trim) | `ffmpeg.service.ts` |
| 18:00-19:00 | v1.1.0-p3 | edit.routes.ts (trim API) | `edit.routes.ts` |

**완료 기준**:
- [ ] 100MB 비디오 업로드/재생
- [ ] Canvas 타임라인 렌더링
- [ ] Trim API 동작

**커밋**:
```bash
git commit -m "feat(frontend): implement VideoPlayer and Timeline"
git tag -a v1.0.0 -m "v1.0.0: Basic Infrastructure"
git commit -m "feat(backend): add FFmpegService with trim operation"
```

### Day 3: 분할, 자막, 속도 (v1.1.0 ~ v1.2.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v1.1.0-p4 | FFmpegService (split) | `ffmpeg.service.ts` |
| 10:30-12:00 | v1.1.0-p5 | EditPanel 컴포넌트 | `EditPanel.tsx` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.2.0-p1 | SRT 생성 로직 | `ffmpeg.service.ts` |
| 14:30-16:00 | v1.2.0-p2 | FFmpeg 필터 체인 (speed + subtitle) | `ffmpeg.service.ts` |
| 16:00-17:30 | v1.2.0-p3 | SubtitleEditor 컴포넌트 | `SubtitleEditor.tsx` |
| 17:30-19:00 | v1.2.0-p4 | 속도 조절 UI | `ControlPanel.tsx` |

**완료 기준**:
- [ ] Split 동작 (30s 지점에서 분할)
- [ ] 자막 burn-in (UTF-8 한글/이모지)
- [ ] 0.5x ~ 2.0x 속도 조절

**커밋**:
```bash
git tag -a v1.1.0 -m "v1.1.0: Trim & Split"
git commit -m "feat(backend): add subtitle and speed control"
git tag -a v1.2.0 -m "v1.2.0: Subtitle & Speed"
```

### Day 4: WebSocket & 영속성 (v1.3.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v1.3.0) | 이해 |
| 10:00-12:00 | v1.3.0-p1 | WebSocketService 구현 | `ws.service.ts` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v1.3.0-p2 | DatabaseService (PostgreSQL) | `database.service.ts` |
| 15:00-16:30 | v1.3.0-p3 | Redis 캐싱 서비스 | `cache.service.ts` |
| 16:30-18:00 | v1.3.0-p4 | ProjectPanel 컴포넌트 | `ProjectPanel.tsx` |
| 18:00-19:00 | v1.3.0-p5 | 프로젝트 저장/로드 API | `projects.routes.ts` |

**완료 기준**:
- [ ] 실시간 진행률 (< 100ms 지연)
- [ ] 프로젝트 저장/로드 동작
- [ ] Redis 세션 관리

**커밋**:
```bash
git commit -m "feat(backend): add WebSocket progress and project persistence"
git tag -a v1.3.0 -m "v1.3.0: WebSocket & Persistence"
```

### Day 5: C++ 네이티브 애드온 (v2.0.0 ~ v2.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v2.0.0, v2.1.0) | 이해 |
| 10:00-12:00 | v2.0.0-p1 | CMake + node-gyp 설정 | `binding.gyp`, `CMakeLists.txt` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v2.0.0-p2 | FFmpeg RAII 래퍼 | `ffmpeg_wrapper.h/cpp` |
| 15:00-17:00 | v2.0.0-p3 | N-API 바인딩 | `video_processor.cpp` |
| 17:00-19:00 | v2.1.0-p1 | 썸네일 추출 | `thumbnail_extractor.cpp` |

**완료 기준**:
- [ ] 네이티브 애드온 컴파일 성공
- [ ] valgrind 메모리 릭 0
- [ ] 썸네일 추출 p99 < 50ms

**커밋**:
```bash
git commit -m "feat(native): implement FFmpeg RAII wrapper with N-API"
git tag -a v2.0.0 -m "v2.0.0: Native Addon Setup"
git commit -m "feat(native): add high-performance thumbnail extraction"
git tag -a v2.1.0 -m "v2.1.0: Thumbnail Extraction"
```

### Day 6: 메타데이터 & 모니터링 (v2.2.0 ~ v2.3.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-11:00 | v2.2.0-p1 | 메타데이터 분석 | `metadata_analyzer.cpp` |
| 11:00-13:00 | v2.2.0-p2 | 코덱 지원 (H.264/H.265/VP9/AV1) | 코덱 판별 로직 |
| 13:00-14:00 | - | 점심 | - |
| 14:00-16:00 | v2.3.0-p1 | Prometheus 메트릭 | `metrics.ts` |
| 16:00-18:00 | v2.3.0-p2 | Grafana 대시보드 | `monitoring/grafana/` |
| 18:00-19:00 | v2.3.0-p3 | 성능 벤치마크 | 테스트 수행 |

**완료 기준**:
- [ ] 메타데이터 추출 < 100ms
- [ ] 8+ 메트릭 타입 수집
- [ ] Grafana 대시보드 10개 패널

**커밋**:
```bash
git commit -m "feat(native): add metadata analyzer with codec detection"
git tag -a v2.2.0 -m "v2.2.0: Metadata Analysis"
git commit -m "feat(monitoring): integrate Prometheus metrics with Grafana"
git tag -a v2.3.0 -m "v2.3.0: Performance Monitoring"
```

### Day 7: 프로덕션 배포 & WebGL/WebAudio (v3.0.0 ~ v3.2.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v3.0.0-p1 | Docker Compose 배포 | `docker-compose.yml` |
| 10:30-12:00 | v3.0.0-p2 | Dockerfile (멀티스테이지) | `Dockerfile`, `Dockerfile.prod` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v3.1.0 | WebGL 코어 엔진 (기초) | `webgl/` 디렉토리 |
| 15:00-17:00 | v3.2.0 | WebAudio 엔진 (기초) | `webaudio/` 디렉토리 |
| 17:00-19:00 | - | 전체 테스트 & 문서화 | README 업데이트 |

**완료 기준**:
- [ ] Docker Compose 전체 스택 실행
- [ ] WebGL 기반 비디오 렌더링 (기초)
- [ ] WebAudio 오디오 처리 (기초)
- [ ] 모든 테스트 통과 (Backend 6/6, Frontend 2/2, Native 11/11)

**커밋**:
```bash
git commit -m "feat(deploy): add Docker Compose production deployment"
git tag -a v3.0.0 -m "v3.0.0: Production Deployment"
git commit -m "feat(frontend): add WebGL and WebAudio core engines"
git tag -a v3.2.0 -m "v3.2.0: Browser Advanced Features"
```

---

## ☕ 프로젝트 3: spring-patterns (7일 스케줄)

> **난이도**: ⭐⭐⭐⭐ (중상급)  
> **기술 스택**: Java 21, Spring Boot 3.3.5, WebFlux, R2DBC, PostgreSQL, Redis, Kafka, Elasticsearch

### Day 1: Bootstrap & CRUD (v0.1.0 ~ v1.0.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v0.1.0, v1.0.0) | 이해 |
| 10:00-11:00 | v0.1.0-p1 | Spring Boot 프로젝트 생성 | `Application.java` |
| 11:00-12:00 | v0.1.0-p2 | application.yml 설정 | H2 연결 설정 |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:00 | v0.1.0-p3 | HealthController 구현 | `/api/health` |
| 14:00-15:00 | v0.1.0-p4 | GlobalExceptionHandler | 예외 처리 |
| 15:00-16:30 | v1.0.0-p1 | JPA Entity 정의 | Issue, Team, User |
| 16:30-18:00 | v1.0.0-p2 | Repository 인터페이스 | `IssueRepository.java` |
| 18:00-19:00 | v1.0.0-p3 | Service Layer 시작 | `IssueService.java` |

**완료 기준**:
- [ ] `GET /api/health` → 200 OK
- [ ] CI 파이프라인 통과
- [ ] Entity 정의 완료

**커밋**:
```bash
git commit -m "chore: initialize Spring Boot project with Gradle"
git tag -a v0.1.0 -m "v0.1.0: Bootstrap & CI"
```

### Day 2: CRUD 완성 & RBAC (v1.0.0 ~ v1.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v1.0.0-p3 | Service Layer 완성 | `@Transactional` |
| 10:30-12:00 | v1.0.0-p4 | Controller Layer | `IssueController.java` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.0.0-p5 | DTO & Mapper | `dto/` 디렉토리 |
| 14:30-15:30 | v1.0.0-p6 | 페이징 & 정렬 | Pageable 적용 |
| 15:30-17:00 | v1.1.0-p1 | Team, TeamMember 엔티티 | `team/` 패키지 |
| 17:00-18:30 | v1.1.0-p2 | TeamService 구현 | CRUD |
| 18:30-19:00 | v1.1.0-p3 | 역할 기반 권한 검증 | OWNER/MANAGER/MEMBER |

**완료 기준**:
- [ ] Issue CRUD 전체 동작
- [ ] 페이징 & 정렬 동작
- [ ] 트랜잭션 롤백 테스트 통과

**커밋**:
```bash
git commit -m "feat(issue): implement IssueService with @Transactional"
git tag -a v1.0.0 -m "v1.0.0: Layered CRUD & Transaction"
git commit -m "feat(team): add Team entities and RBAC"
```

### Day 3: RBAC 완성 & 배치/캐시 (v1.1.0 ~ v1.2.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v1.1.0-p4 | 401/403/404 예외 구분 | `exception/` |
| 10:30-12:00 | v1.1.0-p5 | 통합 테스트 | `TeamTest.java` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.2.0-p1 | DailyIssueStats 엔티티 | `stats/` 패키지 |
| 14:30-16:00 | v1.2.0-p2 | @Scheduled 배치 작업 | `StatsScheduler.java` |
| 16:00-17:30 | v1.2.0-p3 | StatsService API | 집계 API |
| 17:30-19:00 | v1.2.0-p4 | @Cacheable 적용 | 캐싱 설정 |

**완료 기준**:
- [ ] OWNER/MANAGER/MEMBER 권한 분리
- [ ] 비멤버 접근 시 404 (존재 숨김)
- [ ] 캐시 히트 시 < 10ms 응답

**커밋**:
```bash
git commit -m "feat(rbac): implement role-based permission validation"
git tag -a v1.1.0 -m "v1.1.0: Team & RBAC"
git commit -m "feat(stats): add batch stats and caching"
git tag -a v1.2.0 -m "v1.2.0: Batch, Stats, Cache"
```

### Day 4: Elasticsearch & 비동기 이벤트 (v1.3.0 ~ v1.4.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v1.3.0-p1 | Spring Data Elasticsearch 설정 | `build.gradle` |
| 10:30-12:00 | v1.3.0-p2 | ProductDocument 정의 | `ProductDocument.java` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.3.0-p3 | ProductSearchService | 전문 검색 |
| 14:30-16:00 | v1.4.0-p1 | Order, OrderItem 엔티티 | `order/` 패키지 |
| 16:00-17:30 | v1.4.0-p2 | ApplicationEvent 정의 | `event/` |
| 17:30-19:00 | v1.4.0-p3 | @EventListener + @Async | 비동기 처리 |

**완료 기준**:
- [ ] 키워드 + 필터 검색 동작
- [ ] 주문 생성 → 즉시 응답 (< 200ms)
- [ ] 비동기 이벤트 처리 동작

**커밋**:
```bash
git commit -m "feat(search): implement Elasticsearch full-text search"
git tag -a v1.3.0 -m "v1.3.0: Elasticsearch Search"
git commit -m "feat(event): add async event processing"
git tag -a v1.4.0 -m "v1.4.0: Async Events"
```

### Day 5: 프로덕션 인프라 & API Gateway (v1.5.0 ~ v1.6.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v1.5.0-p1 | PostgreSQL 마이그레이션 | `application-prod.yml` |
| 10:30-12:00 | v1.5.0-p2 | Dockerfile (멀티스테이지) | `Dockerfile` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v1.5.0-p3 | docker-compose.yml | 전체 스택 |
| 14:30-16:00 | v1.6.0-p1 | Gateway 모듈 생성 | `gateway/` |
| 16:00-17:30 | v1.6.0-p2 | 라우팅 규칙 정의 | `application.yml` |
| 17:30-19:00 | v1.6.0-p3 | 공통 필터 (로깅, 인증) | `LoggingFilter.java` |

**완료 기준**:
- [ ] Docker Compose 전체 스택 실행
- [ ] PostgreSQL 트랜잭션 동작
- [ ] Gateway 라우팅 동작

**커밋**:
```bash
git commit -m "feat(db): add PostgreSQL and Docker configuration"
git tag -a v1.5.0 -m "v1.5.0: Docker & Production"
git commit -m "feat(gateway): implement Spring Cloud Gateway"
git tag -a v1.6.0 -m "v1.6.0: API Gateway"
```

### Day 6: WebFlux & Virtual Threads (v2.0.0 ~ v2.1.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:00 | - | Design 문서 읽기 (v2.0.0, v2.1.0) | 이해 |
| 10:00-12:00 | v2.0.0-p1 | WebFlux & R2DBC 의존성 | `build.gradle` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-15:00 | v2.0.0-p2 | R2DBC Repository 전환 | `ProductR2dbcRepository.java` |
| 15:00-17:00 | v2.0.0-p3 | 리액티브 컨트롤러 | `ReactiveProductController.java` |
| 17:00-18:30 | v2.1.0-p1 | Virtual Threads 설정 | `VirtualThreadConfig.java` |
| 18:30-19:00 | v2.1.0-p2 | 블로킹 코드 래핑 | `HybridProductService.java` |

**완료 기준**:
- [ ] WebFlux 기반 비동기 API 동작
- [ ] R2DBC Repository로 DB 접근
- [ ] Virtual Threads 활성화

**커밋**:
```bash
git commit -m "feat(reactive): migrate to WebFlux with R2DBC"
git tag -a v2.0.0 -m "v2.0.0: WebFlux & R2DBC"
git commit -m "feat(config): enable Virtual Threads"
git tag -a v2.1.0 -m "v2.1.0: Virtual Threads"
```

### Day 7: Rate Limiting & Caching (v2.2.0 ~ v2.3.0)

| 시간 | 버전 | 작업 내용 | 산출물 |
|------|------|----------|--------|
| 09:00-10:30 | v2.2.0-p1 | Rate Limiting 필터 | `RateLimitingFilter.java` |
| 10:30-12:00 | v2.2.0-p2 | Token Bucket 구현 | `TokenBucketRateLimiter.java` |
| 12:00-13:00 | - | 점심 | - |
| 13:00-14:30 | v2.2.0-p3 | Rate Limit 헤더 | X-RateLimit-* |
| 14:30-16:00 | v2.3.0-p1 | 캐시 매니저 설정 | `CachingConfig.java` |
| 16:00-17:30 | v2.3.0-p2 | @Cacheable 적용 | `CachedProductService.java` |
| 17:30-19:00 | - | 전체 테스트 & 문서화 | README 업데이트 |

**완료 기준**:
- [ ] Token Bucket 기반 Rate Limiter 동작
- [ ] 429 Too Many Requests 응답
- [ ] 캐시 동작 확인
- [ ] 모든 테스트 통과

**커밋**:
```bash
git commit -m "feat(filter): implement rate limiting"
git tag -a v2.2.0 -m "v2.2.0: Rate Limiting"
git commit -m "feat(cache): add caching with TTL"
git tag -a v2.3.0 -m "v2.3.0: Caching & Compression"
```

---

## 📊 스케줄 요약

| 프로젝트 | 최종 버전 | 핵심 역량 | 난이도 |
|----------|----------|----------|--------|
| **cpp-pvp-server** | v2.1.0 | C++ 시스템 프로그래밍, 네트워크, 게임 서버 | ⭐⭐⭐⭐⭐ |
| **native-video-editor** | v3.2.0 | 풀스택, C++ FFmpeg, WebGL/WebAudio | ⭐⭐⭐⭐ |
| **spring-patterns** | v2.3.0 | Java 백엔드, Spring 패턴, 리액티브 | ⭐⭐⭐⭐ |

### 주의사항
1. **시간 조정**: 이해도에 따라 ±1일 조정 가능
2. **우선순위**: 핵심 기능 먼저, 선택 기능은 나중에
3. **테스트**: 매일 마지막 1시간은 테스트/검증에 할애
4. **커밋**: 각 기능 완료 시 명확한 커밋 메시지 작성
