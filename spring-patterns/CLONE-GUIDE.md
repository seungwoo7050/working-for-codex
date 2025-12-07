# 📙 spring-patterns (backend/spring-patterns/) 클론코딩 가이드

> Spring Boot 핵심 패턴 훈련 - CRUD, 트랜잭션, RBAC, 배치, 검색

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **기술 스택** | Java 17, Spring Boot 3.3, Gradle, H2/PostgreSQL, JPA |
| **핵심 학습** | 레이어드 아키텍처, 트랜잭션, RBAC, 캐싱, 배치, 검색 |
| **예상 기간** | 4주 |
| **난이도** | ⭐⭐⭐⭐ |

## 버전 정규화

| 기존 | 정규화된 버전 | 기간 | 설명 |
|------|--------------|------|------|
| M2.0 | v0.1.0 | 1일 | Bootstrap & CI |
| M2.1 | v1.0.0 | 2-3일 | Layered CRUD & Transaction |
| M2.2 | v1.1.0 | 2일 | Team & RBAC |
| M2.3 | v1.2.0 | 2일 | Batch, Stats, Cache |
| M2.4 | v1.3.0 | 2일 | Elasticsearch Search |
| M2.5 | v1.4.0 | 2일 | Async Events |
| M2.6 | v1.5.0 | 2일 | Docker & Production |
| M2.7 | v1.6.0 | 1주 | API Gateway |

---

## v0.1.0: Spring Boot Bootstrap & CI (1일)

> 📅 **권장 기간**: 2025년 4월 1일 ~ 4월 2일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/java-spring/01-java-fundamentals.md
│   └── Java 기초, 객체지향, 예외 처리
├── prerequisite/java-spring/02-spring-boot-basics.md
│   └── Spring Boot 구조, DI, Bean
└── prerequisite/server-basic/04-http-flow-and-rest.md
    └── HTTP 프로토콜, REST API
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 0.1.1 | Spring Boot 프로젝트 생성 | `design/v0.1.0-bootstrap-ci.md` | `Application.java` |
| 0.1.2 | application.yml 설정 | 〃 | `src/main/resources/application.yml` |
| 0.1.3 | HealthController 구현 | 〃 | `src/main/java/.../common/HealthController.java` |
| 0.1.4 | GlobalExceptionHandler | 〃 | `src/main/java/.../common/GlobalExceptionHandler.java` |
| 0.1.5 | GitHub Actions CI | 〃 | `.github/workflows/ci.yml` |
| 0.1.6 | 기본 테스트 작성 | 〃 | `src/test/java/.../ApplicationTests.java` |

### 🔖 커밋 포인트
```bash
# 0.1.1 완료 [📅 2025-04-01 14:00:00]
GIT_AUTHOR_DATE="2025-04-01 14:00:00" GIT_COMMITTER_DATE="2025-04-01 14:00:00" \
git commit -m "chore: initialize Spring Boot project with Gradle"

# 0.1.2 완료 [📅 2025-04-01 15:30:00]
GIT_AUTHOR_DATE="2025-04-01 15:30:00" GIT_COMMITTER_DATE="2025-04-01 15:30:00" \
git commit -m "feat(config): add application.yml with profiles"

# 0.1.3 완료 [📅 2025-04-01 17:00:00]
GIT_AUTHOR_DATE="2025-04-01 17:00:00" GIT_COMMITTER_DATE="2025-04-01 17:00:00" \
git commit -m "feat(health): add HealthController for liveness probe"

# 0.1.4 완료 [📅 2025-04-02 19:00:00]
GIT_AUTHOR_DATE="2025-04-02 19:00:00" GIT_COMMITTER_DATE="2025-04-02 19:00:00" \
git commit -m "feat(error): add GlobalExceptionHandler"

# 0.1.5 완료 [📅 2025-04-02 20:30:00]
GIT_AUTHOR_DATE="2025-04-02 20:30:00" GIT_COMMITTER_DATE="2025-04-02 20:30:00" \
git commit -m "ci: add GitHub Actions workflow for build and test"

# 0.1.6 완료 → v0.1.0 태그 [📅 2025-04-02 21:30:00]
GIT_AUTHOR_DATE="2025-04-02 21:30:00" GIT_COMMITTER_DATE="2025-04-02 21:30:00" \
git commit -m "test: add context loading test"
GIT_COMMITTER_DATE="2025-04-02 21:45:00" git tag -a v0.1.0 -m "Bootstrap: Spring Boot project with CI"
```

### ✅ 완료 기준
- [ ] `GET /api/health` → 200 OK
- [ ] CI 파이프라인 통과
- [ ] Gradle 빌드 성공

---

## v1.0.0: Layered CRUD & Transaction (2-3일)

> 📅 **권장 기간**: 2025년 4월 3일 ~ 4월 10일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/java-spring/03-spring-jpa-basics.md
│   └── JPA Entity, Repository, 관계 매핑
└── prerequisite/server-basic/06-database-connection-pool.md
    └── 트랜잭션, ACID
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.0.1 | JPA Entity 정의 (Issue, Team, User) | `design/v1.0.0-layered-crud.md` | `src/main/java/.../issue/Issue.java` |
| 1.0.2 | Repository 인터페이스 | 〃 | `src/main/java/.../issue/IssueRepository.java` |
| 1.0.3 | Service Layer (트랜잭션) | 〃 | `src/main/java/.../issue/IssueService.java` |
| 1.0.4 | Controller Layer | 〃 | `src/main/java/.../issue/IssueController.java` |
| 1.0.5 | DTO & Mapper | 〃 | `src/main/java/.../issue/dto/` |
| 1.0.6 | 페이징 & 정렬 | 〃 | `IssueService.java` |
| 1.0.7 | 트랜잭션 롤백 테스트 | 〃 | `src/test/java/.../issue/IssueServiceTest.java` |

### 🔖 커밋 포인트
```bash
# 1.0.1 완료 [📅 2025-04-03 20:00:00]
GIT_AUTHOR_DATE="2025-04-03 20:00:00" GIT_COMMITTER_DATE="2025-04-03 20:00:00" \
git commit -m "feat(entity): define Issue, Team, User JPA entities"

# 1.0.2 완료 [📅 2025-04-04 21:30:00]
GIT_AUTHOR_DATE="2025-04-04 21:30:00" GIT_COMMITTER_DATE="2025-04-04 21:30:00" \
git commit -m "feat(repo): add Repository interfaces with JpaRepository"

# 1.0.3 완료 [📅 2025-04-06 15:00:00]
GIT_AUTHOR_DATE="2025-04-06 15:00:00" GIT_COMMITTER_DATE="2025-04-06 15:00:00" \
git commit -m "feat(service): implement IssueService with @Transactional"

# 1.0.4 완료 [📅 2025-04-07 20:00:00]
GIT_AUTHOR_DATE="2025-04-07 20:00:00" GIT_COMMITTER_DATE="2025-04-07 20:00:00" \
git commit -m "feat(controller): add IssueController with REST endpoints"

# 1.0.5 완료 [📅 2025-04-08 21:00:00]
GIT_AUTHOR_DATE="2025-04-08 21:00:00" GIT_COMMITTER_DATE="2025-04-08 21:00:00" \
git commit -m "feat(dto): add DTOs and MapStruct mappers"

# 1.0.6 완료 [📅 2025-04-09 19:30:00]
GIT_AUTHOR_DATE="2025-04-09 19:30:00" GIT_COMMITTER_DATE="2025-04-09 19:30:00" \
git commit -m "feat(issue): add pagination and sorting support"

# 1.0.7 완료 → v1.0.0 태그 [📅 2025-04-10 21:00:00]
GIT_AUTHOR_DATE="2025-04-10 21:00:00" GIT_COMMITTER_DATE="2025-04-10 21:00:00" \
git commit -m "test(service): add transaction rollback tests"
GIT_COMMITTER_DATE="2025-04-10 21:15:00" git tag -a v1.0.0 -m "MVP 1.0: Layered CRUD with Transaction"
```

### ✅ 완료 기준
- [ ] Issue CRUD 전체 동작
- [ ] 페이징 & 정렬 동작
- [ ] 트랜잭션 롤백 테스트 통과

---

## v1.1.0: Team & RBAC (2일)

> 📅 **권장 기간**: 2025년 4월 11일 ~ 4월 14일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.1.1 | Team, TeamMember 엔티티 | `design/v1.1.0-team-rbac.md` | `src/main/java/.../team/` |
| 1.1.2 | TeamService 구현 | 〃 | `src/main/java/.../team/TeamService.java` |
| 1.1.3 | 역할 기반 권한 검증 | 〃 | `TeamService.java` |
| 1.1.4 | 401/403/404 예외 구분 | 〃 | `common/exception/` |
| 1.1.5 | 통합 테스트 | 〃 | `src/test/java/.../team/` |

### 🔖 커밋 포인트
```bash
# 1.1.1 완료 [📅 2025-04-11 20:00:00]
GIT_AUTHOR_DATE="2025-04-11 20:00:00" GIT_COMMITTER_DATE="2025-04-11 20:00:00" \
git commit -m "feat(entity): add Team and TeamMember entities with roles"

# 1.1.2 완료 [📅 2025-04-12 21:00:00]
GIT_AUTHOR_DATE="2025-04-12 21:00:00" GIT_COMMITTER_DATE="2025-04-12 21:00:00" \
git commit -m "feat(team): implement TeamService with CRUD"

# 1.1.3 완료 [📅 2025-04-13 15:00:00]
GIT_AUTHOR_DATE="2025-04-13 15:00:00" GIT_COMMITTER_DATE="2025-04-13 15:00:00" \
git commit -m "feat(rbac): add role-based permission validation"

# 1.1.4 완료 [📅 2025-04-13 19:30:00]
GIT_AUTHOR_DATE="2025-04-13 19:30:00" GIT_COMMITTER_DATE="2025-04-13 19:30:00" \
git commit -m "feat(exception): implement domain exceptions for 401/403/404"

# 1.1.5 완료 → v1.1.0 태그 [📅 2025-04-14 20:00:00]
GIT_AUTHOR_DATE="2025-04-14 20:00:00" GIT_COMMITTER_DATE="2025-04-14 20:00:00" \
git commit -m "test(team): add integration tests for RBAC"
GIT_COMMITTER_DATE="2025-04-14 20:15:00" git tag -a v1.1.0 -m "MVP 1.1: Team & RBAC"
```

### ✅ 완료 기준
- [ ] OWNER/MANAGER/MEMBER 권한 분리
- [ ] 비멤버 접근 시 404 (존재 숨김)
- [ ] 마지막 OWNER 삭제 방지

---

## v1.2.0: Batch, Stats, Cache (2일)

> 📅 **권장 기간**: 2025년 4월 15일 ~ 4월 18일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.2.1 | DailyIssueStats 엔티티 | `design/v1.2.0-batch-stats-cache.md` | `src/main/java/.../stats/` |
| 1.2.2 | @Scheduled 배치 작업 | 〃 | `src/main/java/.../stats/StatsScheduler.java` |
| 1.2.3 | StatsService API | 〃 | `src/main/java/.../stats/StatsService.java` |
| 1.2.4 | @Cacheable 적용 | 〃 | `IssueService.java` |
| 1.2.5 | 캐시 테스트 | 〃 | `src/test/java/.../stats/` |

### 🔖 커밋 포인트
```bash
# 1.2.1 완료 [📅 2025-04-15 20:00:00]
GIT_AUTHOR_DATE="2025-04-15 20:00:00" GIT_COMMITTER_DATE="2025-04-15 20:00:00" \
git commit -m "feat(stats): add DailyIssueStats entity"

# 1.2.2 완료 [📅 2025-04-16 21:00:00]
GIT_AUTHOR_DATE="2025-04-16 21:00:00" GIT_COMMITTER_DATE="2025-04-16 21:00:00" \
git commit -m "feat(batch): implement scheduled stats aggregation"

# 1.2.3 완료 [📅 2025-04-17 19:00:00]
GIT_AUTHOR_DATE="2025-04-17 19:00:00" GIT_COMMITTER_DATE="2025-04-17 19:00:00" \
git commit -m "feat(stats): add StatsService with aggregation API"

# 1.2.4 완료 [📅 2025-04-17 21:00:00]
GIT_AUTHOR_DATE="2025-04-17 21:00:00" GIT_COMMITTER_DATE="2025-04-17 21:00:00" \
git commit -m "feat(cache): apply @Cacheable for popular issues"

# 1.2.5 완료 → v1.2.0 태그 [📅 2025-04-18 20:00:00]
GIT_AUTHOR_DATE="2025-04-18 20:00:00" GIT_COMMITTER_DATE="2025-04-18 20:00:00" \
git commit -m "test(cache): add cache hit/miss tests"
GIT_COMMITTER_DATE="2025-04-18 20:15:00" git tag -a v1.2.0 -m "MVP 1.2: Batch, Stats, Cache"
```

### ✅ 완료 기준
- [ ] 매일 자정 배치 작업 실행
- [ ] 캐시 히트 시 < 10ms 응답
- [ ] 통계 API 동작

---

## v1.3.0: Elasticsearch Search (2일)

> 📅 **권장 기간**: 2025년 4월 19일 ~ 4월 22일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.3.1 | Spring Data Elasticsearch 설정 | `design/v1.3.0-elasticsearch.md` | `build.gradle` |
| 1.3.2 | IssueDocument 정의 | 〃 | `src/main/java/.../search/IssueDocument.java` |
| 1.3.3 | SearchService 구현 | 〃 | `src/main/java/.../search/SearchService.java` |
| 1.3.4 | 전문 검색 API | 〃 | `src/main/java/.../search/SearchController.java` |
| 1.3.5 | 동기화 리스너 | 〃 | `src/main/java/.../search/IssueIndexListener.java` |

### 🔖 커밋 포인트
```bash
# 1.3.1 완료 [📅 2025-04-19 15:00:00]
GIT_AUTHOR_DATE="2025-04-19 15:00:00" GIT_COMMITTER_DATE="2025-04-19 15:00:00" \
git commit -m "chore(deps): add Spring Data Elasticsearch dependency"

# 1.3.2 완료 [📅 2025-04-19 20:00:00]
GIT_AUTHOR_DATE="2025-04-19 20:00:00" GIT_COMMITTER_DATE="2025-04-19 20:00:00" \
git commit -m "feat(search): define IssueDocument for Elasticsearch"

# 1.3.3 완료 [📅 2025-04-20 21:00:00]
GIT_AUTHOR_DATE="2025-04-20 21:00:00" GIT_COMMITTER_DATE="2025-04-20 21:00:00" \
git commit -m "feat(search): implement SearchService with full-text search"

# 1.3.4 완료 [📅 2025-04-21 19:30:00]
GIT_AUTHOR_DATE="2025-04-21 19:30:00" GIT_COMMITTER_DATE="2025-04-21 19:30:00" \
git commit -m "feat(search): add SearchController with search API"

# 1.3.5 완료 → v1.3.0 태그 [📅 2025-04-22 20:00:00]
GIT_AUTHOR_DATE="2025-04-22 20:00:00" GIT_COMMITTER_DATE="2025-04-22 20:00:00" \
git commit -m "feat(search): add event listener for ES sync"
GIT_COMMITTER_DATE="2025-04-22 20:15:00" git tag -a v1.3.0 -m "MVP 1.3: Elasticsearch Search"
```

### ✅ 완료 기준
- [ ] Issue 생성 시 ES 자동 색인
- [ ] 키워드 + 필터 검색 동작
- [ ] 재색인 API 동작

---

## v1.4.0: Async Events (2일)

> 📅 **권장 기간**: 2025년 4월 23일 ~ 4월 26일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.4.1 | Order, OrderItem 엔티티 | `design/v1.4.0-async-events.md` | `src/main/java/.../order/` |
| 1.4.2 | ApplicationEvent 정의 | 〃 | `src/main/java/.../order/event/` |
| 1.4.3 | @EventListener 구현 | 〃 | `src/main/java/.../order/OrderEventListener.java` |
| 1.4.4 | @Async 비동기 처리 | 〃 | `OrderEventListener.java` |
| 1.4.5 | 이벤트 테스트 | 〃 | `src/test/java/.../order/` |

### 🔖 커밋 포인트
```bash
# 1.4.1 완료 [📅 2025-04-23 20:00:00]
GIT_AUTHOR_DATE="2025-04-23 20:00:00" GIT_COMMITTER_DATE="2025-04-23 20:00:00" \
git commit -m "feat(order): add Order and OrderItem entities"

# 1.4.2 완료 [📅 2025-04-24 21:00:00]
GIT_AUTHOR_DATE="2025-04-24 21:00:00" GIT_COMMITTER_DATE="2025-04-24 21:00:00" \
git commit -m "feat(event): define OrderCreatedEvent and OrderCompletedEvent"

# 1.4.3 완료 [📅 2025-04-25 19:30:00]
GIT_AUTHOR_DATE="2025-04-25 19:30:00" GIT_COMMITTER_DATE="2025-04-25 19:30:00" \
git commit -m "feat(event): implement OrderEventListener"

# 1.4.4 완료 [📅 2025-04-25 21:00:00]
GIT_AUTHOR_DATE="2025-04-25 21:00:00" GIT_COMMITTER_DATE="2025-04-25 21:00:00" \
git commit -m "feat(async): enable async processing for events"

# 1.4.5 완료 → v1.4.0 태그 [📅 2025-04-26 20:00:00]
GIT_AUTHOR_DATE="2025-04-26 20:00:00" GIT_COMMITTER_DATE="2025-04-26 20:00:00" \
git commit -m "test(event): add async event processing tests"
GIT_COMMITTER_DATE="2025-04-26 20:15:00" git tag -a v1.4.0 -m "MVP 1.4: Async Events"
```

### ✅ 완료 기준
- [ ] 주문 생성 → 즉시 응답 (< 200ms)
- [ ] 비동기 이벤트 처리 동작
- [ ] 이벤트 리스너 테스트 통과

---

## v1.5.0: Docker & Production (2일)

> 📅 **권장 기간**: 2025년 4월 27일 ~ 4월 30일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.5.1 | PostgreSQL 마이그레이션 | `design/v1.5.0-production-infra.md` | `application-prod.yml` |
| 1.5.2 | Dockerfile (멀티스테이지) | 〃 | `Dockerfile` |
| 1.5.3 | docker-compose.yml | 〃 | `docker-compose.yml` |
| 1.5.4 | Redis 캐시 연동 | 〃 | `application-prod.yml` |
| 1.5.5 | 전체 스택 테스트 | 〃 | `docker-compose up` |

### 🔖 커밋 포인트
```bash
# 1.5.1 완료 [📅 2025-04-27 15:00:00]
GIT_AUTHOR_DATE="2025-04-27 15:00:00" GIT_COMMITTER_DATE="2025-04-27 15:00:00" \
git commit -m "feat(db): add PostgreSQL configuration for production"

# 1.5.2 완료 [📅 2025-04-28 20:00:00]
GIT_AUTHOR_DATE="2025-04-28 20:00:00" GIT_COMMITTER_DATE="2025-04-28 20:00:00" \
git commit -m "chore: add multi-stage Dockerfile"

# 1.5.3 완료 [📅 2025-04-29 19:00:00]
GIT_AUTHOR_DATE="2025-04-29 19:00:00" GIT_COMMITTER_DATE="2025-04-29 19:00:00" \
git commit -m "chore: add docker-compose with PostgreSQL and Redis"

# 1.5.4 완료 [📅 2025-04-29 21:00:00]
GIT_AUTHOR_DATE="2025-04-29 21:00:00" GIT_COMMITTER_DATE="2025-04-29 21:00:00" \
git commit -m "feat(cache): integrate Redis for distributed caching"

# 1.5.5 완료 → v1.5.0 태그 [📅 2025-04-30 20:00:00]
GIT_AUTHOR_DATE="2025-04-30 20:00:00" GIT_COMMITTER_DATE="2025-04-30 20:00:00" \
git commit -m "test: verify full stack with docker-compose"
GIT_COMMITTER_DATE="2025-04-30 20:15:00" git tag -a v1.5.0 -m "MVP 1.5: Docker & Production Ready"
```

### ✅ 완료 기준
- [ ] PostgreSQL 트랜잭션 동작
- [ ] Redis 분산 캐시 동작
- [ ] Docker Compose 전체 스택 실행

---

## v1.6.0: API Gateway (1주)

> 📅 **권장 기간**: 2025년 5월 1일 ~ 5월 7일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── Spring Cloud Gateway 공식 문서
│   └── https://docs.spring.io/spring-cloud-gateway/
├── WebFlux vs MVC 차이점
│   └── 리액티브 프로그래밍 기초
└── API Gateway 패턴
    └── 라우팅, 필터링, 로드밸런싱
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.6.1 | Gateway 모듈 생성 & 의존성 | `design/v1.6.0-api-gateway.md` | `gateway/build.gradle` |
| 1.6.2 | Gateway 기본 설정 | 〃 | `gateway/src/.../GatewayApplication.java` |
| 1.6.3 | 라우팅 규칙 정의 | 〃 | `gateway/src/.../resources/application.yml` |
| 1.6.4 | 공통 필터 (로깅, 인증) | 〃 | `gateway/src/.../filter/LoggingFilter.java` |
| 1.6.5 | Docker Compose 통합 | 〃 | `docker-compose.yml` |

### 🔖 커밋 포인트
```bash
# 1.6.1 완료 [📅 2025-05-01 20:00:00]
GIT_AUTHOR_DATE="2025-05-01 20:00:00" GIT_COMMITTER_DATE="2025-05-01 20:00:00" \
git commit -m "chore(gateway): create gateway module with Spring Cloud Gateway"

# 1.6.2 완료 [📅 2025-05-02 21:00:00]
GIT_AUTHOR_DATE="2025-05-02 21:00:00" GIT_COMMITTER_DATE="2025-05-02 21:00:00" \
git commit -m "feat(gateway): add GatewayApplication with basic configuration"

# 1.6.3 완료 [📅 2025-05-04 15:00:00]
GIT_AUTHOR_DATE="2025-05-04 15:00:00" GIT_COMMITTER_DATE="2025-05-04 15:00:00" \
git commit -m "feat(gateway): define routing rules for backend services"

# 1.6.4 완료 [📅 2025-05-05 20:00:00]
GIT_AUTHOR_DATE="2025-05-05 20:00:00" GIT_COMMITTER_DATE="2025-05-05 20:00:00" \
git commit -m "feat(gateway): add global filters for logging and CORS"

# 1.6.5 완료 → v1.6.0 태그 [📅 2025-05-07 20:00:00]
GIT_AUTHOR_DATE="2025-05-07 20:00:00" GIT_COMMITTER_DATE="2025-05-07 20:00:00" \
git commit -m "chore(docker): integrate gateway service into docker-compose"
GIT_COMMITTER_DATE="2025-05-07 20:15:00" git tag -a v1.6.0 -m "MVP 1.6: API Gateway with Spring Cloud Gateway"
```

### ✅ 완료 기준
- [ ] Gateway 8081 → Backend 8080 라우팅 동작
- [ ] 로깅 필터 동작 (요청/응답 로그)
- [ ] CORS 처리 Gateway에서 동작
- [ ] Docker Compose로 전체 스택 실행