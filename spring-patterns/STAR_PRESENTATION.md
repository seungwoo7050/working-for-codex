# Spring Patterns — STAR 방식 프로젝트 발표

> 행동 기반 역량 면접(BEI) 대응을 위한 STAR 구조 발표 자료

---

## 🎯 Situation (상황)

### 프로젝트 배경

**이전 프로젝트**: `raw-http-server` (순수 Java로 HTTP 서버 구현)

**학습 동기**:
- raw-http-server로 HTTP 프로토콜은 이해했으나, 실무에서는 프레임워크 사용
- Spring Framework의 핵심 패턴을 체계적으로 학습할 필요
- 단순 CRUD를 넘어 엔터프라이즈 수준 아키텍처 구현

**기술적 도전**:
1. **레이어드 아키텍처**: 책임 분리, 테스트 용이성
2. **인증/인가**: JWT + RBAC 구현
3. **분산 시스템**: 캐싱, 메시징, 검색
4. **리액티브**: 비동기 논블로킹 처리
5. **성능 최적화**: Rate Limiting, Virtual Threads

### 개인 역할

백엔드 개발자로서 Spring Boot 기반 전체 아키텍처 설계 및 구현

---

## 📋 Task (과업)

### 핵심 목표

| 영역 | 목표 | 측정 지표 |
|------|------|----------|
| **구조** | 클린 아키텍처 | 레이어 분리, 테스트 커버리지 |
| **보안** | 엔터프라이즈 보안 | JWT + RBAC 구현 |
| **확장성** | 분산 시스템 패턴 | Redis, Kafka, ES 통합 |
| **성능** | 고성능 처리 | WebFlux, Virtual Threads |

### 버전별 요구사항

| 버전 | 기능 | 학습 목표 |
|------|------|----------|
| v1.0.0 | Issue Tracker CRUD + JWT | 레이어드 아키텍처, Spring Security |
| v1.1.0 | Team RBAC | SpEL, @PreAuthorize |
| v1.2.0 | Batch + Cache | @Scheduled, @Cacheable, @Retryable |
| v1.3.0 | Elasticsearch | 검색 엔진 통합, dual-write |
| v1.4.0 | Kafka Events | 이벤트 드리븐 아키텍처 |
| v1.5.0 | Production Infra | Docker, PostgreSQL, Redis |
| v1.6.0 | API Gateway | Spring Cloud Gateway |
| v2.0.0 | WebFlux + R2DBC | 리액티브 프로그래밍 |
| v2.1.0 | Virtual Threads | Java 21 경량 스레드 |
| v2.2.0 | Rate Limiting | Token Bucket 알고리즘 |
| v2.3.0 | Caching & Compression | @Cacheable, Gzip |

---

## 🔧 Action (행동)

### 1단계: 기초 CRUD & 인증 (v1.0.0 ~ v1.1.0)

#### 레이어드 아키텍처 설계
```
Controller (HTTP 인터페이스)
    ↓
Service (비즈니스 로직)
    ↓
Repository (데이터 접근)
    ↓
Entity (도메인 모델)
```

**핵심 결정**: 의존성 방향
- 상위 레이어 → 하위 레이어만 의존
- 인터페이스 기반 결합 (Repository)
- 결과: 단위 테스트 용이, 교체 가능

#### JWT 인증 구현
```java
@Component
public class JwtAuthenticationFilter extends OncePerRequestFilter {
    @Override
    protected void doFilterInternal(...) {
        String token = extractToken(request);
        if (jwtProvider.validate(token)) {
            Authentication auth = jwtProvider.getAuthentication(token);
            SecurityContextHolder.getContext().setAuthentication(auth);
        }
        filterChain.doFilter(request, response);
    }
}
```

#### RBAC 구현
```java
@PreAuthorize("@teamAccessEvaluator.hasRole(#teamId, 'MANAGER')")
public void updateTeam(Long teamId, TeamDto dto) {
    // MANAGER 이상만 접근 가능
}

@Component
public class TeamAccessEvaluator {
    public boolean hasRole(Long teamId, String role) {
        String username = SecurityContextHolder.getContext()...;
        TeamMember member = teamMemberRepository.findByTeamAndUser(...);
        return member.getRole().hasPermission(Role.valueOf(role));
    }
}
```

### 2단계: 엔터프라이즈 패턴 (v1.2.0 ~ v1.6.0)

#### Redis 캐싱
```java
@Cacheable(value = "products", key = "#id")
public ProductDTO findById(Long id) {
    return productRepository.findById(id)
        .map(this::toDto)
        .orElseThrow();
}

@CacheEvict(value = "products", key = "#id")
public void updateProduct(Long id, ProductDTO dto) {
    // 캐시 무효화 후 업데이트
}
```

**성과**:
- 캐시 히트율 80%+ 달성
- DB 부하 5배 감소

#### Elasticsearch 통합
```java
@Document(indexName = "products")
public class ProductDocument {
    @Id
    private String id;
    @Field(type = FieldType.Text, analyzer = "standard")
    private String name;
    @Field(type = FieldType.Keyword)
    private String category;
}

// Dual-write 패턴
@Transactional
public void saveProduct(Product product) {
    productRepository.save(product);          // JPA
    productSearchRepository.save(toDocument(product));  // ES
}
```

#### Kafka 이벤트 드리븐
```java
// Producer
@Service
public class OrderEventPublisher {
    public void publishOrderCreated(Order order) {
        OrderEvent event = new OrderEvent(order.getId(), "CREATED");
        kafkaTemplate.send("order-events", event);
    }
}

// Consumer
@KafkaListener(topics = "order-events")
public void handleOrderEvent(OrderEvent event) {
    switch (event.type()) {
        case "CREATED" -> notificationService.sendOrderConfirmation(event);
        case "SHIPPED" -> trackingService.updateStatus(event);
    }
}
```

#### API Gateway
```java
// gateway/application.yml
spring:
  cloud:
    gateway:
      routes:
        - id: api-route
          uri: http://localhost:8080
          predicates:
            - Path=/api/**
          filters:
            - LoggingFilter

// LoggingFilter.java
public class LoggingFilter implements GlobalFilter {
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        log.info("Request: {} {}", exchange.getRequest().getMethod(), 
                 exchange.getRequest().getPath());
        return chain.filter(exchange);
    }
}
```

### 3단계: 리액티브 & 최적화 (v2.0.0 ~ v2.3.0)

#### WebFlux + R2DBC
```java
// Reactive Repository
public interface ProductRepository extends R2dbcRepository<Product, Long> {
    Flux<Product> findByCategory(String category);
}

// Reactive Service
public Mono<ProductDTO> findById(Long id) {
    return productRepository.findById(id)
        .map(this::toDto)
        .switchIfEmpty(Mono.error(new NotFoundException()));
}

// SSE Streaming
@GetMapping(value = "/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
public Flux<ProductDTO> streamProducts() {
    return productRepository.findAll()
        .map(this::toDto)
        .delayElements(Duration.ofMillis(100));
}
```

#### Java 21 Virtual Threads
```java
@Configuration
public class VirtualThreadConfig {
    @Bean
    public Executor virtualThreadExecutor() {
        return Executors.newVirtualThreadPerTaskExecutor();
    }
}

// 블로킹 작업을 Virtual Thread로 래핑
public Mono<ExternalData> fetchExternal() {
    return Mono.fromCallable(() -> externalApi.fetch())
        .subscribeOn(Schedulers.fromExecutor(virtualThreadExecutor));
}
```

**성과**: 
- 동일 리소스에서 10배 높은 동시성
- 컨텍스트 스위칭 비용 절감

#### Token Bucket Rate Limiting
```java
public class TokenBucketRateLimiter {
    private final int capacity;
    private final double refillRate;
    private double tokens;
    private long lastRefill;

    public synchronized boolean tryAcquire() {
        refill();
        if (tokens >= 1.0) {
            tokens -= 1.0;
            return true;
        }
        return false;
    }

    private void refill() {
        long now = System.currentTimeMillis();
        double elapsed = (now - lastRefill) / 1000.0;
        tokens = Math.min(capacity, tokens + elapsed * refillRate);
        lastRefill = now;
    }
}
```

---

## 📈 Result (결과)

### 구현 완료 기능

| 버전 | 기능 | 상태 |
|------|------|------|
| v0.1.0 | Bootstrap & CI | ✅ 완료 |
| v1.0.0 | Layered CRUD + JWT | ✅ 완료 |
| v1.1.0 | Team RBAC | ✅ 완료 |
| v1.2.0 | Batch Stats Cache | ✅ 완료 |
| v1.3.0 | Elasticsearch | ✅ 완료 |
| v1.4.0 | Kafka Events | ✅ 완료 |
| v1.5.0 | Production Infra | ✅ 완료 |
| v1.6.0 | API Gateway | ✅ 완료 |
| v2.0.0 | WebFlux & R2DBC | ✅ 완료 |
| v2.1.0 | Virtual Threads | ✅ 완료 |
| v2.2.0 | Rate Limiting | ✅ 완료 |
| v2.3.0 | Caching & Compression | ✅ 완료 |

### 기술적 성취

1. **엔터프라이즈 패턴**: 12개 버전에 걸쳐 핵심 패턴 구현
2. **분산 시스템**: Redis, Kafka, Elasticsearch 통합
3. **리액티브**: WebFlux + R2DBC 비동기 파이프라인
4. **최신 Java**: Java 21 Virtual Threads 적용

### 배운 점

1. **레이어 분리**: 테스트 용이성, 유지보수성 향상
2. **이벤트 드리븐**: 시스템 간 결합도 감소
3. **리액티브 vs 블로킹**: 상황에 맞는 선택 중요
4. **Virtual Threads**: 기존 블로킹 코드도 고성능 가능

---

## 💡 핵심 기술 역량 증명

### 1. Spring 생태계 이해
> "Security, Data, Cloud, WebFlux까지 전 영역 구현"

Spring의 각 모듈 역할과 연동 방식 이해

### 2. 분산 시스템 패턴
> "Redis 캐싱, Kafka 이벤트, ES 검색 통합"

MSA 환경에서 필수적인 인프라 연동 경험

### 3. 리액티브 프로그래밍
> "Mono/Flux, backpressure, 스케줄러"

비동기 논블로킹 처리의 이론과 실제 적용

### 4. 최신 Java 기능
> "Virtual Threads, Record, Pattern Matching"

Java 21의 새 기능을 실제 프로젝트에 적용

---

## 🎤 예상 질문 및 답변

### Q1: 레이어드 아키텍처의 장단점은?

**답변**: 
- **장점**: 관심사 분리, 테스트 용이, 교체 가능
- **단점**: 간단한 CRUD에도 보일러플레이트 발생
- **경험**: 프로젝트 규모가 커질수록 레이어 분리의 가치 체감

### Q2: WebFlux vs MVC 선택 기준은?

**답변**:
- **MVC**: 대부분의 CRUD 앱, 블로킹 I/O 위주
- **WebFlux**: 높은 동시성, 스트리밍, 논블로킹 필수
- **하이브리드**: Virtual Threads로 MVC에서도 고동시성 가능

### Q3: Kafka를 선택한 이유는?

**답변**:
- **내구성**: 디스크 기반 로그, 재처리 가능
- **확장성**: 파티션 기반 수평 확장
- **생태계**: Spring Kafka의 성숙한 통합

### Q4: Rate Limiting 구현 방식은?

**답변**:
- **Token Bucket**: 버스트 허용, 평균 속도 제어
- **대안**: Sliding Window (정확도), Leaky Bucket (일정 속도)
- **분산 환경**: Redis 기반 공유 카운터 필요
