# 📚 스냅샷 인덱스 (Snapshots Index)

> CLONE-GUIDE.md의 패치 단위를 기준으로 생성된 단계별 스냅샷 목록

## 개요

이 디렉토리에는 프로젝트의 각 마이너 버전(v0.1.0 ~ v1.4.0)에 해당하는 독립적인 스냅샷이 포함되어 있습니다.
각 스냅샷은 해당 시점까지 누적된 기능을 포함합니다.

## 스냅샷 목록

| # | 디렉토리 | 대응 버전 | 패치 범위 | 주요 기능 | 빌드 가능 |
|---|----------|-----------|-----------|-----------|-----------|
| 1 | `001_bootstrap/` | v0.1.0 | 0.1.1 ~ 0.1.6 | Spring Boot 초기화, HealthController, GlobalExceptionHandler, CI | ✅ |
| 2 | `002_layered-crud/` | v1.0.0 | 1.0.1 ~ 1.0.7 | Issue CRUD, JWT 인증, 레이어드 아키텍처, 페이징 | ✅ |
| 3 | `003_team-rbac/` | v1.1.0 | 1.1.1 ~ 1.1.5 | Team/TeamMember, RBAC (OWNER/MANAGER/MEMBER), WorkspaceItem | ✅ |
| 4 | `004_batch-stats-cache/` | v1.2.0 | 1.2.1 ~ 1.2.5 | DailyIssueStats, @Scheduled 배치, @Cacheable, ExternalApiService | ⚠️ |
| 5 | `005_elasticsearch/` | v1.3.0 | 1.3.1 ~ 1.3.5 | Product 도메인, Elasticsearch 검색, 인덱스 동기화 | ⚠️ |
| 6 | `006_async-events/` | v1.4.0 | 1.4.1 ~ 1.4.5 | Order 도메인, Kafka Producer/Consumer, 비동기 이벤트 | ⚠️ |
| 7 | `007_production-infra/` | v1.5.0 | 1.5.1 ~ 1.5.5 | PostgreSQL, Redis 캐시, Docker Compose, 멀티스테이지 빌드 | ⚠️ |
| 8 | `008_api-gateway/` | v1.6.0 | 1.6.1 ~ 1.6.5 | Spring Cloud Gateway, 라우팅, 로깅 필터, CORS | ⚠️ |

### 범례
- ✅ 완전 빌드/테스트 가능
- ⚠️ 일부 외부 의존성 필요 (Elasticsearch, Kafka 등)

---

## 스냅샷 상세

### 001_bootstrap (v0.1.0)

**포함된 기능:**
- Spring Boot 3.3.5 프로젝트 초기화
- `GET /api/health` 헬스체크 엔드포인트
- GlobalExceptionHandler (기본 예외 처리)
- 컨텍스트 로딩 테스트

**빌드 & 테스트:**
```bash
cd snapshots/001_bootstrap
./gradlew build test
./gradlew bootRun
curl http://localhost:8080/api/health
```

---

### 002_layered-crud (v1.0.0)

**포함된 기능:**
- 001_bootstrap의 모든 기능
- Issue, User, Project, Comment 도메인
- JpaRepository 기반 데이터 접근
- @Transactional 서비스 계층
- REST Controller 및 DTO
- JWT 기반 인증 (JwtUtil, JwtAuthenticationFilter)
- Spring Security 설정
- 페이징 & 정렬

**빌드 & 테스트:**
```bash
cd snapshots/002_layered-crud
./gradlew build test
./gradlew bootRun
# API 테스트: 회원가입 → 로그인 → 프로젝트 생성 → 이슈 생성
```

---

### 003_team-rbac (v1.1.0)

**포함된 기능:**
- 002_layered-crud의 모든 기능
- Team, TeamMember, TeamRole 도메인
- WorkspaceItem 도메인
- RBAC (역할 기반 접근 제어)
  - OWNER: 모든 권한
  - MANAGER: 멤버 관리 + 아이템 편집
  - MEMBER: 조회만 가능
- 404 vs 403 예외 분리
- 마지막 OWNER 보호 로직

**빌드 & 테스트:**
```bash
cd snapshots/003_team-rbac
./gradlew build test
```

---

### 004_batch-stats-cache (v1.2.0)

**포함된 기능:**
- 003_team-rbac의 모든 기능
- DailyIssueStats 통계 도메인
- @Scheduled 배치 작업 (매일 새벽 3시)
- @Cacheable 인기 이슈 캐싱
- ExternalApiService (WebClient + 재시도)

**제한 사항:**
- 완전한 소스 코드 포함을 위해 추가 파일 복사 필요
- 자세한 내용은 `004_batch-stats-cache/SNAPSHOT-NOTES.md` 참조

---

### 005_elasticsearch (v1.3.0)

**포함된 기능:**
- 004_batch-stats-cache의 모든 기능
- Product RDB 엔티티
- ProductDocument (Elasticsearch 문서)
- ProductSearchService (전문 검색)
- RDB ↔ ES 동기화

**제한 사항:**
- 로컬 Elasticsearch 인스턴스 필요
- `docker run -d --name es -p 9200:9200 -e "discovery.type=single-node" elasticsearch:8.11.0`

---

### 006_async-events (v1.4.0)

**포함된 기능:**
- 005_elasticsearch의 모든 기능
- Order, OrderItem, Notification 도메인
- OrderEvent (Kafka 메시지)
- Kafka Producer (OrderService)
- Kafka Consumer (NotificationService)
- 비동기 이벤트 처리

**제한 사항:**
- 로컬 Kafka 인스턴스 필요
- docker-compose로 Kafka + Zookeeper 실행 필요

---

### 007_production-infra (v1.5.0)

**포함된 기능:**
- 006_async-events의 모든 기능
- PostgreSQL 16 데이터베이스 (prod 프로파일)
- Redis 7 분산 캐시 (prod 프로파일)
- 멀티스테이지 Dockerfile
- docker-compose.yml (전체 스택)

**제한 사항:**
- Docker 환경 필요
- PostgreSQL, Redis 컨테이너 실행 필요

**빌드 & 테스트:**
```bash
cd snapshots/007_production-infra
./gradlew build test

# Docker로 전체 스택 실행
docker-compose up -d
```

---

### 008_api-gateway (v1.6.0)

**포함된 기능:**
- 007_production-infra의 모든 기능
- Spring Cloud Gateway 모듈
- 라우팅 규칙 (/api/** → Backend)
- 로깅 필터 (요청/응답 로깅)
- CORS 설정 (Gateway에서 중앙 처리)

**제한 사항:**
- Docker 환경 필요
- Backend 서비스가 먼저 실행되어야 함

**빌드 & 테스트:**
```bash
cd snapshots/008_api-gateway

# Backend 빌드
./gradlew build test

# Gateway 빌드
cd gateway
./gradlew build

# Docker로 전체 스택 실행
docker-compose up -d

# Gateway 통해 접근
curl http://localhost:8081/api/health
```

---

## 버전 누적 다이어그램

```
v0.1.0 (001_bootstrap)
    │
    └─ + Issue CRUD, JWT
           │
           v
       v1.0.0 (002_layered-crud)
           │
           └─ + Team, RBAC
                  │
                  v
              v1.1.0 (003_team-rbac)
                  │
                  └─ + Stats, Cache, Batch
                         │
                         v
                     v1.2.0 (004_batch-stats-cache)
                         │
                         └─ + Elasticsearch
                                │
                                v
                            v1.3.0 (005_elasticsearch)
                                │
                                └─ + Kafka Events
                                       │
                                       v
                                   v1.4.0 (006_async-events)
                                       │
                                       └─ + Docker, PostgreSQL, Redis
                                              │
                                              v
                                          v1.5.0 (007_production-infra)
                                              │
                                              └─ + API Gateway
                                                     │
                                                     v
                                                 v1.6.0 (최종 - 루트 디렉토리)
```

---

## 사용 방법

### 특정 버전으로 학습하기

1. 원하는 스냅샷 디렉토리로 이동
2. `./gradlew build`로 빌드 확인
3. `./gradlew bootRun`으로 실행
4. 해당 버전의 design 문서 참조하여 학습

### 점진적 학습 경로

1. `001_bootstrap` → Spring Boot 기초
2. `002_layered-crud` → 레이어드 아키텍처, JPA, JWT
3. `003_team-rbac` → RBAC 패턴
4. `004_batch-stats-cache` → 스케줄링, 캐싱
5. `005_elasticsearch` → 검색 엔진 통합
6. `006_async-events` → 이벤트 기반 아키텍처
7. `007_production-infra` → Docker, PostgreSQL, Redis
8. `008_api-gateway` → Spring Cloud Gateway, 마이크로서비스 준비

---

## 관련 문서

- `CLONE-GUIDE.md` - 전체 패치 가이드
- `design/` - 각 버전별 상세 설계 문서
- `README.md` - 프로젝트 개요
