# Spring Patterns — 클론코딩 가이드

> 버전별 커밋 가이드 (주니어-미드레벨 현업 개발자 기준 현실적 타임라인)

---

## 커밋 방법

아래 명령어를 그대로 복사하여 터미널에 붙여넣으면 됩니다.  
`GIT_AUTHOR_DATE`와 `GIT_COMMITTER_DATE`가 동시에 설정됩니다.

---

## 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **개발 기간** | 2025-01-01 ~ 2025-04-11 (약 14주) |
| **난이도** | 중 (Spring 생태계 학습) |
| **작업 페이스** | 주 5일, 일 1커밋 기준 (복잡 기능 2-3일) |

---

## 레퍼런스

### 공식 문서
- [Spring Boot Reference](https://docs.spring.io/spring-boot/docs/current/reference/html/)
- [Spring Security Reference](https://docs.spring.io/spring-security/reference/)
- [Spring Data JPA](https://docs.spring.io/spring-data/jpa/reference/jpa.html)
- [Spring Data R2DBC](https://docs.spring.io/spring-data/r2dbc/reference/r2dbc.html)
- [Spring WebFlux](https://docs.spring.io/spring-framework/reference/web/webflux.html)
- [Spring Cloud Gateway](https://docs.spring.io/spring-cloud-gateway/reference/)
- [Spring Kafka](https://docs.spring.io/spring-kafka/reference/)

### 튜토리얼
- [Baeldung Spring Tutorials](https://www.baeldung.com/spring-tutorial)
- [Spring Guides](https://spring.io/guides)

### 라이브러리/인프라
- [Elasticsearch Java Client](https://www.elastic.co/guide/en/elasticsearch/client/java-api-client/current/index.html)
- [Spring Data Elasticsearch](https://docs.spring.io/spring-data/elasticsearch/reference/)
- [Apache Kafka Documentation](https://kafka.apache.org/documentation/)
- [Redis Documentation](https://redis.io/docs/)

### Java 21
- [JEP 444: Virtual Threads](https://openjdk.org/jeps/444)
- [Virtual Threads Guide](https://docs.oracle.com/en/java/javase/21/core/virtual-threads.html)

### 유사 프로젝트
- [Spring PetClinic](https://github.com/spring-projects/spring-petclinic)
- [JHipster](https://github.com/jhipster/jhipster)
- [Spring Boot Examples](https://github.com/ityouknow/spring-boot-examples)

---

## Phase 1: 기초 CRUD & 인증

### v0.1.0 — Bootstrap

```bash
GIT_AUTHOR_DATE="2024-08-01 20:30:00" GIT_COMMITTER_DATE="2024-08-01 20:30:00" git commit -m "feat: initialize Spring Boot 3.3 project"
```

```bash
GIT_AUTHOR_DATE="2024-08-02 21:00:00" GIT_COMMITTER_DATE="2024-08-02 21:00:00" git commit -m "chore: add GitHub Actions CI"
```

```bash
GIT_AUTHOR_DATE="2024-08-05 19:45:00" GIT_COMMITTER_DATE="2024-08-05 19:45:00" git commit -m "feat: add actuator health endpoint"
```

```bash
GIT_AUTHOR_DATE="2024-08-06 20:15:00" GIT_COMMITTER_DATE="2024-08-06 20:15:00" git commit -m "feat: configure H2 in-memory database"
```

---

### v1.0.0 — Layered CRUD + JWT

```bash
GIT_AUTHOR_DATE="2024-08-07 19:30:00" GIT_COMMITTER_DATE="2024-08-07 19:30:00" git commit -m "feat: implement Issue entity with JPA"
```

```bash
GIT_AUTHOR_DATE="2024-08-08 20:00:00" GIT_COMMITTER_DATE="2024-08-08 20:00:00" git commit -m "feat: add IssueRepository"
```

```bash
GIT_AUTHOR_DATE="2024-08-09 19:15:00" GIT_COMMITTER_DATE="2024-08-09 19:15:00" git commit -m "feat: implement IssueService CRUD"
```

```bash
GIT_AUTHOR_DATE="2024-08-12 21:00:00" GIT_COMMITTER_DATE="2024-08-12 21:00:00" git commit -m "feat: add IssueController REST API"
```

```bash
GIT_AUTHOR_DATE="2024-08-14 19:45:00" GIT_COMMITTER_DATE="2024-08-14 19:45:00" git commit -m "feat: implement JWT authentication"
```

```bash
GIT_AUTHOR_DATE="2024-08-15 20:30:00" GIT_COMMITTER_DATE="2024-08-15 20:30:00" git commit -m "feat: add JwtAuthenticationFilter"
```

```bash
GIT_AUTHOR_DATE="2024-08-16 19:15:00" GIT_COMMITTER_DATE="2024-08-16 19:15:00" git commit -m "test: add IssueController integration tests"
```

---

### v1.1.0 — Team RBAC

```bash
GIT_AUTHOR_DATE="2024-08-19 20:00:00" GIT_COMMITTER_DATE="2024-08-19 20:00:00" git commit -m "feat: implement Team entity"
```

```bash
GIT_AUTHOR_DATE="2024-08-20 19:30:00" GIT_COMMITTER_DATE="2024-08-20 19:30:00" git commit -m "feat: add Role enum for RBAC"
```

```bash
GIT_AUTHOR_DATE="2024-08-22 21:00:00" GIT_COMMITTER_DATE="2024-08-22 21:00:00" git commit -m "feat: implement TeamAccessEvaluator"
```

```bash
GIT_AUTHOR_DATE="2024-08-23 19:45:00" GIT_COMMITTER_DATE="2024-08-23 19:45:00" git commit -m "feat: add @PreAuthorize to endpoints"
```

```bash
GIT_AUTHOR_DATE="2024-08-26 20:15:00" GIT_COMMITTER_DATE="2024-08-26 20:15:00" git commit -m "feat: implement WorkspaceItem entity"
```

```bash
GIT_AUTHOR_DATE="2024-08-27 19:30:00" GIT_COMMITTER_DATE="2024-08-27 19:30:00" git commit -m "test: add RBAC integration tests"
```

---

## Phase 2: 엔터프라이즈 패턴

### v1.2.0 — Batch + Cache + Retry

```bash
GIT_AUTHOR_DATE="2024-08-28 20:00:00" GIT_COMMITTER_DATE="2024-08-28 20:00:00" git commit -m "feat: add @Scheduled batch statistics"
```

```bash
GIT_AUTHOR_DATE="2024-08-29 19:15:00" GIT_COMMITTER_DATE="2024-08-29 19:15:00" git commit -m "feat: implement Redis caching"
```

```bash
GIT_AUTHOR_DATE="2024-09-02 21:00:00" GIT_COMMITTER_DATE="2024-09-02 21:00:00" git commit -m "feat: add external API with @Retryable"
```

```bash
GIT_AUTHOR_DATE="2024-09-03 19:45:00" GIT_COMMITTER_DATE="2024-09-03 19:45:00" git commit -m "feat: implement @Recover fallback"
```

---

### v1.3.0 — Elasticsearch

```bash
GIT_AUTHOR_DATE="2024-09-05 20:30:00" GIT_COMMITTER_DATE="2024-09-05 20:30:00" git commit -m "feat: integrate Elasticsearch"
```

```bash
GIT_AUTHOR_DATE="2024-09-09 19:15:00" GIT_COMMITTER_DATE="2024-09-09 19:15:00" git commit -m "feat: implement Product search"
```

```bash
GIT_AUTHOR_DATE="2024-09-10 20:00:00" GIT_COMMITTER_DATE="2024-09-10 20:00:00" git commit -m "feat: add dual-write sync"
```

```bash
GIT_AUTHOR_DATE="2024-09-11 19:30:00" GIT_COMMITTER_DATE="2024-09-11 19:30:00" git commit -m "test: add search integration tests"
```

---

### v1.4.0 — Kafka Events

```bash
GIT_AUTHOR_DATE="2024-09-12 21:00:00" GIT_COMMITTER_DATE="2024-09-12 21:00:00" git commit -m "feat: add Kafka producer"
```

```bash
GIT_AUTHOR_DATE="2024-09-16 19:45:00" GIT_COMMITTER_DATE="2024-09-16 19:45:00" git commit -m "feat: implement Kafka consumer"
```

```bash
GIT_AUTHOR_DATE="2024-09-17 20:30:00" GIT_COMMITTER_DATE="2024-09-17 20:30:00" git commit -m "feat: add OrderEventPublisher"
```

```bash
GIT_AUTHOR_DATE="2024-09-18 19:15:00" GIT_COMMITTER_DATE="2024-09-18 19:15:00" git commit -m "feat: implement event-driven order processing"
```

---

### v1.5.0 — Production Infra

```bash
GIT_AUTHOR_DATE="2024-09-19 20:00:00" GIT_COMMITTER_DATE="2024-09-19 20:00:00" git commit -m "feat: migrate H2 to PostgreSQL"
```

```bash
GIT_AUTHOR_DATE="2024-09-23 19:30:00" GIT_COMMITTER_DATE="2024-09-23 19:30:00" git commit -m "feat: configure Redis connection pool"
```

```bash
GIT_AUTHOR_DATE="2024-09-25 21:00:00" GIT_COMMITTER_DATE="2024-09-25 21:00:00" git commit -m "feat: add Docker Compose stack"
```

```bash
GIT_AUTHOR_DATE="2024-09-26 19:45:00" GIT_COMMITTER_DATE="2024-09-26 19:45:00" git commit -m "feat: add multi-stage Dockerfile"
```

---

### v1.6.0 — API Gateway

```bash
GIT_AUTHOR_DATE="2024-09-30 20:30:00" GIT_COMMITTER_DATE="2024-09-30 20:30:00" git commit -m "feat: create gateway module"
```

```bash
GIT_AUTHOR_DATE="2024-10-01 19:15:00" GIT_COMMITTER_DATE="2024-10-01 19:15:00" git commit -m "feat: add LoggingFilter"
```

```bash
GIT_AUTHOR_DATE="2024-10-02 20:00:00" GIT_COMMITTER_DATE="2024-10-02 20:00:00" git commit -m "feat: configure CORS in gateway"
```

```bash
GIT_AUTHOR_DATE="2024-10-03 19:30:00" GIT_COMMITTER_DATE="2024-10-03 19:30:00" git commit -m "feat: add route configuration"
```

---

## Phase 3: 리액티브 & 최적화

### v2.0.0 — WebFlux + R2DBC

```bash
GIT_AUTHOR_DATE="2024-10-07 21:00:00" GIT_COMMITTER_DATE="2024-10-07 21:00:00" git commit -m "feat: add WebFlux module"
```

```bash
GIT_AUTHOR_DATE="2024-10-09 19:45:00" GIT_COMMITTER_DATE="2024-10-09 19:45:00" git commit -m "feat: implement R2DBC repository"
```

```bash
GIT_AUTHOR_DATE="2024-10-10 20:30:00" GIT_COMMITTER_DATE="2024-10-10 20:30:00" git commit -m "feat: add reactive ProductService"
```

```bash
GIT_AUTHOR_DATE="2024-10-11 19:15:00" GIT_COMMITTER_DATE="2024-10-11 19:15:00" git commit -m "feat: implement SSE streaming"
```

```bash
GIT_AUTHOR_DATE="2024-10-14 20:00:00" GIT_COMMITTER_DATE="2024-10-14 20:00:00" git commit -m "feat: add WebFlux error handling"
```

```bash
GIT_AUTHOR_DATE="2024-10-15 19:30:00" GIT_COMMITTER_DATE="2024-10-15 19:30:00" git commit -m "test: add WebFlux integration tests"
```

---

### v2.1.0 — Virtual Threads

```bash
GIT_AUTHOR_DATE="2024-10-17 21:00:00" GIT_COMMITTER_DATE="2024-10-17 21:00:00" git commit -m "feat: configure Java 21 Virtual Threads"
```

```bash
GIT_AUTHOR_DATE="2024-10-18 19:45:00" GIT_COMMITTER_DATE="2024-10-18 19:45:00" git commit -m "feat: add blocking wrapper for IO"
```

```bash
GIT_AUTHOR_DATE="2024-10-21 20:30:00" GIT_COMMITTER_DATE="2024-10-21 20:30:00" git commit -m "feat: implement hybrid MVC + WebFlux"
```

```bash
GIT_AUTHOR_DATE="2024-10-22 19:15:00" GIT_COMMITTER_DATE="2024-10-22 19:15:00" git commit -m "perf: Virtual Threads throughput test"
```

---

### v2.2.0 — Rate Limiting

```bash
GIT_AUTHOR_DATE="2024-10-24 20:00:00" GIT_COMMITTER_DATE="2024-10-24 20:00:00" git commit -m "feat: implement TokenBucketRateLimiter"
```

```bash
GIT_AUTHOR_DATE="2024-10-28 19:30:00" GIT_COMMITTER_DATE="2024-10-28 19:30:00" git commit -m "feat: add RateLimitingFilter"
```

```bash
GIT_AUTHOR_DATE="2024-10-29 21:00:00" GIT_COMMITTER_DATE="2024-10-29 21:00:00" git commit -m "feat: implement ClientKeyResolver"
```

```bash
GIT_AUTHOR_DATE="2024-10-30 19:45:00" GIT_COMMITTER_DATE="2024-10-30 19:45:00" git commit -m "feat: add X-RateLimit headers"
```

---

### v2.3.0 — Caching & Compression

```bash
GIT_AUTHOR_DATE="2024-11-04 20:30:00" GIT_COMMITTER_DATE="2024-11-04 20:30:00" git commit -m "feat: configure CacheManager"
```

```bash
GIT_AUTHOR_DATE="2024-11-05 19:15:00" GIT_COMMITTER_DATE="2024-11-05 19:15:00" git commit -m "feat: add @Cacheable to hot paths"
```

```bash
GIT_AUTHOR_DATE="2024-11-06 20:00:00" GIT_COMMITTER_DATE="2024-11-06 20:00:00" git commit -m "feat: implement @CacheEvict strategy"
```

```bash
GIT_AUTHOR_DATE="2024-11-08 19:30:00" GIT_COMMITTER_DATE="2024-11-08 19:30:00" git commit -m "feat: enable Gzip compression"
```

---

## 타임라인 요약

| 구간 | 버전 | 기간 | 커밋 수 | 비고 |
|------|------|------|--------|------|
| Phase 1 | v0.1.0 ~ v1.1.0 | 1/1 ~ 1/30 | 17 | CRUD, JWT, RBAC |
| Phase 2 | v1.2.0 ~ v1.6.0 | 2/3 ~ 3/12 | 20 | 엔터프라이즈 패턴 |
| Phase 3 | v2.0.0 ~ v2.3.0 | 3/17 ~ 4/11 | 18 | 리액티브, VT |
| **합계** | | **약 14주** | **55** | |

---

## 버전별 완료 체크리스트

### v1.0.0 완료 시
- [ ] Issue CRUD API 정상 동작
- [ ] JWT 토큰 발급/검증
- [ ] `/login` 엔드포인트 응답
- [ ] H2 콘솔 접속 가능

### v1.1.0 완료 시
- [ ] Team 생성 및 멤버 추가
- [ ] OWNER만 팀 삭제 가능
- [ ] MEMBER는 아이템 읽기만 가능
- [ ] @PreAuthorize 권한 체크 동작

### v1.2.0 완료 시
- [ ] @Scheduled 배치 실행 확인
- [ ] Redis 캐시 히트/미스 로그
- [ ] @Retryable 재시도 동작
- [ ] @Recover 폴백 실행

### v1.3.0 완료 시
- [ ] Elasticsearch 검색 응답
- [ ] multi_match 쿼리 정상
- [ ] JPA + ES dual-write 동기화

### v1.4.0 완료 시
- [ ] Kafka 프로듀서 메시지 발행
- [ ] 컨슈머 메시지 수신 로그
- [ ] 주문 이벤트 처리

### v1.5.0 완료 시
- [ ] PostgreSQL 연결 성공
- [ ] Docker Compose 4 서비스 실행
- [ ] 프로덕션 DB 마이그레이션

### v1.6.0 완료 시
- [ ] Gateway 라우팅 동작
- [ ] CORS 설정 확인
- [ ] 로깅 필터 출력

### v2.0.0 완료 시
- [ ] WebFlux Mono/Flux 응답
- [ ] SSE 스트림 동작
- [ ] R2DBC 비동기 쿼리

### v2.1.0 완료 시
- [ ] Virtual Threads 활성화 확인
- [ ] 블로킹 IO 처리 성능 향상

### v2.2.0 완료 시
- [ ] Rate Limiting 429 응답
- [ ] X-RateLimit 헤더 확인

### v2.3.0 완료 시
- [ ] @Cacheable 캐시 동작
- [ ] Gzip 압축 적용 확인

---

## 주요 구현 팁

### JWT 설정
```java
// SecurityConfig.java
@Bean
public SecurityFilterChain filterChain(HttpSecurity http) {
  return http
    .csrf().disable()
    .addFilterBefore(jwtAuthFilter, UsernamePasswordAuthenticationFilter.class)
    .build();
}
```

### @PreAuthorize SpEL
```java
@PreAuthorize("@teamAccessEvaluator.hasRole(#teamId, 'OWNER')")
public void deleteTeam(Long teamId) { }
```

### R2DBC 리액티브
```java
public interface ProductRepository extends R2dbcRepository<Product, Long> {
  Flux<Product> findByCategory(String category);
}
```

### Virtual Threads
```java
@Configuration
public class VirtualThreadConfig {
  @Bean
  public AsyncTaskExecutor applicationTaskExecutor() {
    return new TaskExecutorAdapter(
      Executors.newVirtualThreadPerTaskExecutor()
    );
  }
}
```
