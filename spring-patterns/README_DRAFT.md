# spring-patterns

Spring Boot로 엔터프라이즈 패턴 연습한 프로젝트.  
CRUD부터 Kafka, Elasticsearch, WebFlux, Virtual Threads까지.

## 왜 만들었나

raw-http-server 만들고 나서 "이제 프레임워크 좀 써볼까" 해서 시작함.  
Spring 튜토리얼 따라만 치면 재미없어서 버전별로 기능 추가해가면서 진행.

## 기술 스택

- Java 21 (Virtual Threads)
- Spring Boot 3.3.5
- Spring Data JPA / R2DBC
- Spring Security (JWT)
- Spring Cloud Gateway
- PostgreSQL, Redis
- Elasticsearch 8.11
- Apache Kafka

## 버전별 기능

| 버전 | 기능 |
|------|------|
| v1.0 | Issue CRUD + JWT 인증 |
| v1.1 | Team RBAC (OWNER/MANAGER/MEMBER) |
| v1.2 | @Scheduled 배치, @Cacheable, @Retryable |
| v1.3 | Elasticsearch 검색 |
| v1.4 | Kafka 이벤트 드리븐 |
| v1.5 | PostgreSQL, Docker Compose |
| v1.6 | Spring Cloud Gateway |
| v2.0 | WebFlux + R2DBC |
| v2.1 | Java 21 Virtual Threads |
| v2.2 | Token Bucket Rate Limiting |
| v2.3 | @Cacheable 최적화, Gzip |

## 구조

```
src/main/java/
├── config/         # Security, Cache, Kafka, VirtualThread
├── issue/          # 기본 CRUD (v1.0)
├── team/           # RBAC (v1.1)
├── search/         # Elasticsearch (v1.3)
├── event/          # Kafka (v1.4)
├── reactive/       # WebFlux (v2.0)
├── ratelimit/      # Rate Limiting (v2.2)
└── cache/          # Caching (v2.3)

gateway/            # Spring Cloud Gateway 모듈
```

## 실행

```bash
# 인프라 (PostgreSQL, Redis, Kafka, ES)
docker-compose up -d

# 앱 실행
./gradlew bootRun

# 테스트
./gradlew test
```

## API

```
# Issue
POST   /api/issues
GET    /api/issues/{id}
PUT    /api/issues/{id}
DELETE /api/issues/{id}

# Auth
POST   /api/auth/login
POST   /api/auth/register

# Search
GET    /api/search/products?q=keyword

# Reactive
GET    /api/reactive/products        (Flux)
GET    /api/reactive/products/stream (SSE)
```

## 설정

`application.yml`:

```yaml
spring:
  datasource:
    url: jdbc:postgresql://localhost:5432/patterns
  redis:
    host: localhost
  kafka:
    bootstrap-servers: localhost:9092
  elasticsearch:
    uris: http://localhost:9200
```

## 학습 포인트

- **v1.1**: SpEL 기반 `@PreAuthorize`, `PermissionEvaluator`
- **v1.2**: `@Retryable` + `@Recover` 조합
- **v1.3**: JPA-ES dual-write 동기화
- **v1.4**: Kafka `@KafkaListener` 비동기 처리
- **v2.0**: `Mono`/`Flux` 체이닝, `WebTestClient`
- **v2.1**: Virtual Threads로 블로킹 IO 처리

## TODO

- [ ] Spring Batch 대용량 처리
- [ ] Saga 패턴 (분산 트랜잭션)
- [ ] GraphQL
- [ ] gRPC

## License

MIT
