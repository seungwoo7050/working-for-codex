# 📋 패치 인덱스 (Patch Index)

> CLONE-GUIDE.md의 모든 패치와 design 문서 간의 명시적 매핑

## 개요

이 문서는 CLONE-GUIDE.md에 정의된 모든 패치가 어느 design 문서에서 설명되는지 명확하게 매핑합니다.

---

## 패치 → 설계 문서 매핑

### v0.1.0 (Bootstrap & CI)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 0.1.1 | Spring Boot 프로젝트 생성 | v0.1.0-bootstrap-ci.md §3.1 | Application.java, build.gradle |
| 0.1.2 | application.yml 설정 | v0.1.0-bootstrap-ci.md §3.2 | application*.yml |
| 0.1.3 | HealthController 구현 | v0.1.0-bootstrap-ci.md §3.2 | HealthController.java |
| 0.1.4 | GlobalExceptionHandler | v0.1.0-bootstrap-ci.md §3.2 | GlobalExceptionHandler.java |
| 0.1.5 | GitHub Actions CI | v0.1.0-bootstrap-ci.md §3.3 | .github/workflows/ci.yml |
| 0.1.6 | 기본 테스트 작성 | v0.1.0-bootstrap-ci.md §3.4 | ApplicationTest.java |

---

### v1.0.0 (Layered CRUD & Transaction)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.0.1 | JPA Entity 정의 | v1.0.0-layered-crud.md §3.2 | Issue, User, Project, Comment |
| 1.0.2 | Repository 인터페이스 | v1.0.0-layered-crud.md §3.3 | *Repository.java |
| 1.0.3 | Service Layer | v1.0.0-layered-crud.md §3.4 | *Service.java, @Transactional |
| 1.0.4 | Controller Layer | v1.0.0-layered-crud.md §3.5 | *Controller.java |
| 1.0.5 | DTO & Mapper | v1.0.0-layered-crud.md §3.6 | *Request.java, *Response.java |
| 1.0.6 | 페이징 & 정렬 | v1.0.0-layered-crud.md §3.7 | Pageable 지원 |
| 1.0.7 | 트랜잭션 롤백 테스트 | v1.0.0-layered-crud.md §4 | *ServiceTest.java |

**추가 구현 (암묵적):**
- JWT 인증: JwtUtil, JwtAuthenticationFilter, SecurityConfig
- Auth API: AuthController, AuthService

---

### v1.1.0 (Team & RBAC)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.1.1 | Team, TeamMember 엔티티 | v1.1.0-team-rbac.md §3.1 | Team, TeamMember, TeamRole, WorkspaceItem |
| 1.1.2 | TeamService 구현 | v1.1.0-team-rbac.md §3.2 | TeamService.java |
| 1.1.3 | 역할 기반 권한 검증 | v1.1.0-team-rbac.md §3.3 | assertCanManageMembers() |
| 1.1.4 | 401/403/404 예외 구분 | v1.1.0-team-rbac.md §2.3 | ForbiddenException.java |
| 1.1.5 | 통합 테스트 | v1.1.0-team-rbac.md §4 | TeamIntegrationTest.java |

---

### v1.2.0 (Batch, Stats, Cache)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.2.1 | DailyIssueStats 엔티티 | v1.2.0-batch-stats-cache.md §3.1 | DailyIssueStats.java |
| 1.2.2 | @Scheduled 배치 작업 | v1.2.0-batch-stats-cache.md §3.2 | StatsScheduler.java |
| 1.2.3 | StatsService API | v1.2.0-batch-stats-cache.md §3.3 | StatsService, StatsController |
| 1.2.4 | @Cacheable 적용 | v1.2.0-batch-stats-cache.md §4 | CacheConfig, IssueService 캐싱 |
| 1.2.5 | 캐시 테스트 | v1.2.0-batch-stats-cache.md §4 | IssueServiceCacheTest.java |

---

### v1.3.0 (Elasticsearch Search)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.3.1 | Spring Data Elasticsearch 설정 | v1.3.0-elasticsearch.md §3.1 | build.gradle 의존성 |
| 1.3.2 | ProductDocument 정의 | v1.3.0-elasticsearch.md §3.2 | ProductDocument.java |
| 1.3.3 | ProductSearchService 구현 | v1.3.0-elasticsearch.md §4 | ProductSearchService.java |
| 1.3.4 | 전문 검색 API | v1.3.0-elasticsearch.md §4 | ProductSearchController.java |
| 1.3.5 | Product 저장 시 ES 동기화 | v1.3.0-elasticsearch.md §5 | ProductService (인덱싱 포함) |

---

### v1.4.0 (Async Events)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.4.1 | Order, OrderItem 엔티티 | v1.4.0-async-events.md §3.1 | Order, OrderItem, Notification |
| 1.4.2 | ApplicationEvent 정의 | v1.4.0-async-events.md §4.1 | OrderEvent.java |
| 1.4.3 | @EventListener 구현 | v1.4.0-async-events.md §4.2 | NotificationService |
| 1.4.4 | @Async 비동기 처리 | v1.4.0-async-events.md §4.2 | KafkaConfig, OrderService |
| 1.4.5 | 이벤트 테스트 | v1.4.0-async-events.md §5 | OrderKafkaIntegrationTest.java |

---

### v1.5.0 (Docker & Production)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.5.1 | PostgreSQL 마이그레이션 | v1.5.0-production-infra.md §3 | application-prod.yml |
| 1.5.2 | Dockerfile | v1.5.0-production-infra.md §5 | Dockerfile (멀티스테이지) |
| 1.5.3 | docker-compose.yml | v1.5.0-production-infra.md §5 | docker-compose.yml |
| 1.5.4 | Redis 캐시 연동 | v1.5.0-production-infra.md §4 | CacheConfig (Redis) |
| 1.5.5 | 전체 스택 테스트 | v1.5.0-production-infra.md §5 | docker-compose up |

---

### v1.6.0 (API Gateway)

| 패치 ID | 패치 이름 | 설계 문서 | 핵심 산출물 |
|---------|-----------|-----------|-------------|
| 1.6.1 | Gateway 모듈 생성 & 의존성 | v1.6.0-api-gateway.md §3.1 | gateway/build.gradle |
| 1.6.2 | Gateway 기본 설정 | v1.6.0-api-gateway.md §3.2 | GatewayApplication.java |
| 1.6.3 | 라우팅 규칙 정의 | v1.6.0-api-gateway.md §3.3 | gateway/application.yml |
| 1.6.4 | 공통 필터 | v1.6.0-api-gateway.md §3.4 | LoggingFilter, CorsConfig |
| 1.6.5 | Docker Compose 통합 | v1.6.0-api-gateway.md §4 | docker-compose.yml |

---

## 버전별 누적 기능 요약

| 버전 | 새로운 기능 | 누적 패치 수 |
|------|-------------|--------------|
| v0.1.0 | Bootstrap, CI | 6 |
| v1.0.0 | CRUD, JWT, Transaction | 13 |
| v1.1.0 | Team, RBAC | 18 |
| v1.2.0 | Batch, Cache | 23 |
| v1.3.0 | Elasticsearch | 28 |
| v1.4.0 | Kafka Events | 33 |
| v1.5.0 | Docker, Production | 38 |
| v1.6.0 | API Gateway | 43 |

---

## 관련 문서

- `CLONE-GUIDE.md` - 전체 패치 가이드 및 커밋 포인트
- `snapshots/INDEX.md` - 스냅샷 인덱스
- 각 `v*.md` 파일 - 버전별 상세 설계
