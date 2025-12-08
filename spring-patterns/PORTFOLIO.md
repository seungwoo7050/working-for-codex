# 🚀 Spring Patterns - 포트폴리오 발표자료

> **STAR 방식으로 정리한 Spring 백엔드 패턴 훈련 프로젝트**

---

## 📌 프로젝트 개요

| 항목 | 내용 |
|------|------|
| **프로젝트명** | Spring Patterns Training |
| **기간** | 약 4주 (v0.1.0 ~ v2.3.0) |
| **기술 스택** | Java 21, Spring Boot 3.3.5, WebFlux, R2DBC, PostgreSQL, Redis, Kafka, Elasticsearch |
| **역할** | 백엔드 개발 (개인 프로젝트) |
| **GitHub** | [spring-patterns](https://github.com/seungwoo7050/study-portpolio-overall/tree/main/backend/spring-patterns) |

---

## 🎯 STAR 1: 레이어드 아키텍처 & CRUD 시스템 구축

### Situation (상황)
Spring Boot를 활용한 실무 수준의 백엔드 시스템을 구축하기 위해, 이슈 트래커(Issue Tracker) 도메인을 선정하여 레이어드 아키텍처 기반의 CRUD 시스템을 설계해야 했습니다.

### Task (과제)
- **목표**: 확장 가능한 레이어드 아키텍처 설계
- **요구사항**: 
  - Issue, User, Project, Comment 도메인 모델링
  - JWT 기반 인증/인가 시스템
  - Team RBAC (OWNER/MANAGER/MEMBER) 권한 관리
  - 트랜잭션 관리 및 예외 처리 체계화

### Action (행동)
```
📁 구현한 아키텍처 구조
├── Controller Layer    → REST API 엔드포인트
├── Service Layer      → 비즈니스 로직 + @Transactional
├── Repository Layer   → JPA Repository
└── Domain Layer       → Entity + DTO + Exception
```

**핵심 구현 내용:**
1. **JWT 인증 시스템**: JwtUtil, JwtAuthenticationFilter를 통한 토큰 기반 인증
2. **RBAC 권한 관리**: TeamRole enum과 권한 검증 메서드로 세분화된 접근 제어
3. **예외 처리 체계화**: GlobalExceptionHandler + 커스텀 예외 (UnauthorizedException, ForbiddenException, ResourceNotFoundException)
4. **트랜잭션 관리**: @Transactional의 propagation, rollbackFor 옵션 활용

### Result (결과)
- ✅ **8개 도메인** 모델 구현 (Issue, User, Project, Comment, Team, TeamMember, WorkspaceItem, DailyIssueStats)
- ✅ **15+ REST API** 엔드포인트 구현
- ✅ **RBAC 권한 시스템** 완성 (OWNER/MANAGER/MEMBER 3단계)
- ✅ **통합 테스트** 커버리지 확보

---

## 🎯 STAR 2: 비동기 이벤트 & 검색 시스템

### Situation (상황)
시스템 확장성을 위해 비동기 이벤트 처리와 전문 검색 기능이 필요했습니다. 동기식 처리의 한계를 극복하고, 대용량 데이터 검색 성능을 개선해야 했습니다.

### Task (과제)
- **Kafka**: 주문 이벤트 비동기 처리
- **Elasticsearch**: 상품 전문 검색
- **Spring Scheduler**: 일일 통계 배치 작업
- **외부 API 연동**: WebClient 기반 비동기 호출

### Action (행동)
**1. Kafka 이벤트 아키텍처**
```java
// 이벤트 발행
@Service
public class OrderService {
    @Transactional
    public Order createOrder(OrderRequest request) {
        Order order = orderRepository.save(order);
        kafkaTemplate.send("order-events", new OrderCreatedEvent(order));
        return order;
    }
}

// 이벤트 소비
@KafkaListener(topics = "order-events")
public void handleOrderEvent(OrderCreatedEvent event) {
    notificationService.sendNotification(event);
}
```

**2. Elasticsearch 검색**
```java
@Document(indexName = "products")
public class ProductDocument {
    @Field(type = FieldType.Text, analyzer = "standard")
    private String name;
    
    @Field(type = FieldType.Text)
    private String description;
}
```

### Result (결과)
- ✅ **Kafka Producer/Consumer** 구현으로 비동기 이벤트 처리
- ✅ **Elasticsearch 전문 검색** (키워드 + 필터 조합)
- ✅ **@Scheduled 배치** - 매일 자정 통계 집계
- ✅ **외부 API 병렬 호출** - WebClient + 타임아웃 처리

---

## 🎯 STAR 3: 리액티브 전환 & 성능 최적화

### Situation (상황)
동기식 MVC 아키텍처에서 높은 동시성 처리가 필요해졌습니다. 또한 Java 21의 Virtual Threads를 활용하여 블로킹 코드와 리액티브 코드를 효율적으로 통합해야 했습니다.

### Task (과제)
- **WebFlux & R2DBC**: 리액티브 스택으로 전환
- **Virtual Threads**: 블로킹 코드 최적화
- **Rate Limiting**: API 요청 제한
- **Caching & Compression**: 응답 성능 개선

### Action (행동)
**1. WebFlux 리액티브 컨트롤러**
```java
@RestController
@RequestMapping("/api/reactive/products")
public class ReactiveProductController {
    
    @GetMapping(produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    public Flux<ProductResponse> streamProducts() {
        return productService.findAllAsStream()
            .delayElements(Duration.ofMillis(100));
    }
    
    @GetMapping("/{id}")
    public Mono<ProductResponse> getProduct(@PathVariable Long id) {
        return productService.findById(id);
    }
}
```

**2. Virtual Threads 하이브리드 서비스**
```java
@Service
public class HybridProductService {
    
    private final ExecutorService virtualThreadExecutor = 
        Executors.newVirtualThreadPerTaskExecutor();
    
    public Mono<ProductResponse> getProductWithLegacyData(Long id) {
        return Mono.fromCallable(() -> legacyAdapter.fetchData(id))
            .subscribeOn(Schedulers.fromExecutor(virtualThreadExecutor))
            .flatMap(legacyData -> 
                reactiveService.enrichProduct(id, legacyData));
    }
}
```

**3. Token Bucket Rate Limiter**
```java
public class TokenBucketRateLimiter implements RateLimiter {
    private final int capacity;
    private final int refillRate;
    private AtomicInteger tokens;
    
    @Override
    public RateLimitResult tryAcquire(String clientKey) {
        // Token Bucket 알고리즘 직접 구현
    }
}
```

### Result (결과)
- ✅ **WebFlux + R2DBC** 리액티브 스택 전환 완료
- ✅ **SSE (Server-Sent Events)** 실시간 스트리밍
- ✅ **Virtual Threads** 하이브리드 아키텍처 구현
- ✅ **Token Bucket Rate Limiter** 직접 구현 (100 req/min 제한)
- ✅ **Spring Cache + Gzip 압축** 성능 최적화

---

## 🎯 STAR 4: 프로덕션 인프라 구축

### Situation (상황)
개발 환경에서 H2 인메모리 DB를 사용했지만, 프로덕션 환경을 위해 PostgreSQL, Redis, Kafka, Elasticsearch를 포함한 완전한 인프라가 필요했습니다.

### Task (과제)
- Docker Compose로 멀티 서비스 환경 구성
- API Gateway 분리 아키텍처 구현
- 프로덕션 레벨 설정 관리

### Action (행동)
**Docker Compose 아키텍처**
```yaml
services:
  app:
    build: .
    depends_on:
      - postgres
      - redis
      - kafka
      - elasticsearch
    
  gateway:
    build: ./gateway
    ports:
      - "8080:8080"
    
  postgres:
    image: postgres:15
    
  redis:
    image: redis:7
    
  kafka:
    image: confluentinc/cp-kafka:7.5.0
    
  elasticsearch:
    image: elasticsearch:8.11.0
```

**Spring Cloud Gateway 라우팅**
```yaml
spring:
  cloud:
    gateway:
      routes:
        - id: api-route
          uri: http://app:8081
          predicates:
            - Path=/api/**
          filters:
            - AddRequestHeader=X-Request-Source, gateway
            - name: RequestRateLimiter
```

### Result (결과)
- ✅ **Docker Compose** 6개 서비스 통합 (App, Gateway, PostgreSQL, Redis, Kafka, ES)
- ✅ **멀티스테이지 Dockerfile** - 최적화된 이미지 빌드
- ✅ **API Gateway** - 라우팅, 로깅, CORS 필터
- ✅ **프로파일 기반 설정** - local/test/prod 환경 분리

---

## 📊 기술 스택 요약

### Backend
| 기술 | 버전 | 용도 |
|------|------|------|
| Java | 21 | Virtual Threads 지원 |
| Spring Boot | 3.3.5 | 프레임워크 |
| Spring WebFlux | - | 리액티브 웹 |
| Spring Data JPA | - | ORM (v1.x) |
| Spring Data R2DBC | - | 리액티브 DB (v2.x) |
| Spring Security | - | 인증/인가 |
| Spring Cache | - | 캐싱 |

### Data & Messaging
| 기술 | 용도 |
|------|------|
| PostgreSQL | 프로덕션 DB |
| H2 | 개발/테스트 DB |
| Redis | 캐시 |
| Kafka | 이벤트 메시징 |
| Elasticsearch | 전문 검색 |

### DevOps
| 기술 | 용도 |
|------|------|
| Docker | 컨테이너화 |
| Docker Compose | 멀티 서비스 오케스트레이션 |
| GitHub Actions | CI/CD |
| Gradle | 빌드 도구 |

---

## 🚀 지원 가능 회사 티어별 분석

> **Spring Boot Java 백엔드 개발자 포지션** (2025년 12월 기준, 사람인 채용공고 419건 분석)

### 🏆 Tier 1: 대기업/대형 IT 기업

| 회사 | 주요 포지션 | 연봉 범위 | 특징 |
|------|-------------|-----------|------|
| **롯데렌탈** | 웹개발, 백엔드 개발자 | 4,000~6,000만원 | 대기업 안정성, 복리후생 우수 |
| **더존비즈온** | 웹개발, 풀스택 개발자 | 3,500~5,500만원 | ERP 전문 기업, 안정적 성장 |
| **신성통상** | 패션시스템 개발, 백엔드 | 3,800~5,200만원 | 패션 플랫폼, 대기업 계열사 |

**지원 전략**: 대기업 프로세스 경험 강조, 안정성과 성장 가능성 어필

---

### 🥈 Tier 2: 중견 IT 기업

| 회사 | 주요 포지션 | 연봉 범위 | 특징 |
|------|-------------|-----------|------|
| **엔젤로보틱스** | 각 분야 개발자, 전문연구요원 | 3,200~5,000만원 | 로보틱스 전문, 연구개발 중심 |
| **코아맥스테크놀로지** | 솔루션 개발자, 자사 솔루션 | 3,500~5,500만원 | 산업용 솔루션, 기술 전문성 |
| **미소정보기술** | JAVA/Python/AI 플랫폼 개발자 | 4,000~6,000만원 | AI 플랫폼, 고연봉 가능 |
| **케이사인** | 보안통합인증개발, 백엔드 | 3,800~5,200만원 | 보안 솔루션 전문 |
| **아이앤텍** | 전자고지 솔루션 개발, 유지관리 | 3,200~4,800만원 | 금융권 솔루션, 안정적 |
| **비즈톡** | WEB 시스템 운영, 유지보수 | 3,500~5,000만원 | 커뮤니케이션 플랫폼 |
| **모비젠** | JAVA/Python 백엔드 서버 개발자 | 3,800~5,500만원 | 데이터 분석 플랫폼 |

**지원 전략**: 기술 전문성 강조, 도메인 지식과 실무 경험 어필

---

### 🥉 Tier 3: 스타트업/중소기업

| 회사 | 주요 포지션 | 연봉 범위 | 특징 |
|------|-------------|-----------|------|
| **픽셀소프트웨어** | Backend 개발자 | 3,000~4,500만원 | 게임/엔터테인먼트 |
| **아웃라이어** | Backend API 개발자 | 3,200~4,800만원 | SaaS 플랫폼 |
| **위펀** | 백엔드 개발 엔지니어 | 4,000~6,000만원 | 핀테크/금융 |
| **하마랩** | Back-End 개발자 | 3,500~5,000만원 | 헬스케어 플랫폼 |
| **레이팩트** | 자바 웹개발자 | 3,200~4,500만원 | SI/SM 전문 |
| **단꿈아이** | 플랫폼 백엔드 개발자 | 3,500~5,000만원 | 교육 플랫폼 |
| **스마트파이브** | Web 개발자 | 3,000~4,200만원 | 스마트 솔루션 |
| **브릭** | 백엔드 개발자 | 3,200~4,500만원 | 플랫폼 서비스 |
| **와플원** | Java 웹개발자 | 3,300~4,800만원 | SI/SM 기업 |
| **푸드테크** | 전부문 채용 (개발 포함) | 3,000~4,500만원 | 푸드테크 스타트업 |
| **한싹** | JAVA 개발자 | 3,000~4,200만원 | IT 솔루션 |
| **대국지앤에스** | 솔루션 개발, 데이터 엔지니어 | 3,200~4,800만원 | 산업용 솔루션 |

**지원 전략**: 성장 잠재력과 학습 의지 강조, 빠른 업무 적응력 어필

---

### 📈 채용 시장 트렌드 (2025년)

#### 🔥 Hot Keywords
- **Spring Boot 3.x + Java 21**: 최신 버전 경험 필수
- **WebFlux + R2DBC**: 리액티브 프로그래밍
- **Virtual Threads**: Java 21 새로운 기능
- **Docker + Kubernetes**: 컨테이너화 경험
- **Kafka + Elasticsearch**: 이벤트 기반 아키텍처

#### 💰 연봉 인사이트
- **신입/주니어**: 3,000~3,800만원
- **중급 (3~5년)**: 3,800~5,500만원
- **시니어 (5년+)**: 5,000~7,000만원+
- **대기업**: 기본급 + 성과급 + 복리후생
- **스타트업**: 기본급 + 스톡옵션 + 성장 잠재력

#### 🎯 지원 시 강조 포인트
1. **기술 스택 매칭**: 프로젝트의 기술 스택과 채용 공고 비교
2. **프로젝트 경험**: 실제 서비스 구축 경험 강조
3. **문제 해결 능력**: 아키텍처 설계와 최적화 경험
4. **학습 능력**: 새로운 기술 빠른 습득력 (Java 21, WebFlux 등)

---

## 📈 프로젝트 발전 과정

```
v0.1.0 ─► v1.0.0 ─► v1.1.0 ─► v1.2.0 ─► v1.3.0 ─► v1.4.0
Bootstrap   CRUD    RBAC     Cache     Search   Events
   │          │        │        │         │        │
   └──────────┴────────┴────────┴─────────┴────────┘
                         │
                    MVC + JPA
                         │
                         ▼
v1.5.0 ─► v1.6.0 ─► v2.0.0 ─► v2.1.0 ─► v2.2.0 ─► v2.3.0
Docker   Gateway   WebFlux   Virtual   Rate     Cache
                   R2DBC    Threads   Limit    Compress
   │         │        │        │         │        │
   └─────────┴────────┴────────┴─────────┴────────┘
                         │
              WebFlux + R2DBC + Virtual Threads
```

---

## 🎓 학습 및 성장 포인트

### 기술적 역량
1. **아키텍처 진화 경험**: MVC → WebFlux 점진적 마이그레이션
2. **최신 기술 활용**: Java 21 Virtual Threads 실전 적용
3. **분산 시스템 이해**: Kafka, Redis, Elasticsearch 통합
4. **성능 최적화**: Rate Limiting, Caching, Compression

### 문제 해결 역량
1. **블로킹 ↔ 논블로킹 통합**: boundedElastic + Virtual Threads 조합
2. **이벤트 순서 보장**: Kafka 파티션 키 전략
3. **동시성 제어**: Token Bucket 알고리즘 직접 구현

### 협업 역량
1. **상세한 문서화**: 14개 설계 문서 + 12개 스냅샷
2. **학습 가이드**: `[LEARN]` 주석으로 코드 내 설명
3. **CI/CD 구축**: GitHub Actions 자동화

---

## 📞 연락처

- **GitHub**: https://github.com/seungwoo7050
- **Email**: [이메일 주소]

---

*마지막 업데이트: 2025-12-09*
*빌드 상태: ✅ 성공*
*채용 정보 기준: 2025년 12월*
