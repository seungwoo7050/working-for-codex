# Snapshot 009: WebFlux & R2DBC

## 버전 정보
- **버전**: v2.0.0
- **기반**: v1.6.0 (API Gateway)
- **날짜**: 2025-01

## 핵심 변경 사항

### 1. 리액티브 스택 도입
- **Spring WebFlux**: 비동기/논블로킹 웹 프레임워크
- **R2DBC**: 리액티브 데이터베이스 연결
- **Project Reactor**: Mono/Flux 기반 리액티브 스트림

### 2. 아키텍처 결정
- **하이브리드 아키텍처**: 기존 MVC/JPA와 새로운 WebFlux/R2DBC 공존
- **프로파일 분리**: `reactive` 프로파일로 리액티브 컴포넌트 격리
- **독립적 API 버전**: `/api/v2/products` (WebFlux) vs `/api/v1/*` (MVC)

### 3. 새로 추가된 컴포넌트
```
src/main/java/com/example/training/reactive/
├── controller/
│   ├── ReactiveProductController.java    # @Profile("reactive")
│   └── ReactiveExceptionHandler.java     # @Profile("reactive")
├── service/
│   └── ReactiveProductService.java       # @Profile("reactive")
├── repository/
│   └── ProductR2dbcRepository.java       # @Profile("reactive")
├── domain/
│   └── ProductEntity.java
├── dto/
│   ├── ProductRequest.java
│   └── ProductResponse.java
└── event/
    └── ProductEventPublisher.java        # @Profile("reactive")
```

### 4. 설정 파일
- `application.yml`: R2DBC 데이터소스 설정 추가
- `application-reactive.yml`: R2DBC SQL 초기화 설정
- `application-test.yml`: R2DBC 자동 구성 제외 (JPA 테스트 보호)
- `schema-r2dbc.sql`: R2DBC 전용 테이블 스키마

### 5. 의존성 추가 (build.gradle)
```groovy
implementation 'org.springframework.boot:spring-boot-starter-webflux'
implementation 'org.springframework.boot:spring-boot-starter-data-r2dbc'
runtimeOnly 'io.r2dbc:r2dbc-h2'
runtimeOnly 'org.postgresql:r2dbc-postgresql'
testImplementation 'io.projectreactor:reactor-test'
```

## 테스트 전략

### JPA/R2DBC 공존 문제 해결
1. **모든 Reactive 컴포넌트에 `@Profile("reactive")` 적용**
2. **테스트 환경에서 R2DBC 자동 구성 제외**:
   - `R2dbcAutoConfiguration`
   - `R2dbcDataAutoConfiguration`
   - `R2dbcRepositoriesAutoConfiguration`
3. **Reactive 테스트는 순수 Mockito 단위 테스트로 전환**

### 테스트 파일
- `ReactiveProductServiceTest.java`: Mockito + StepVerifier
- `ReactiveProductControllerTest.java`: WebTestClient + Mockito (순수 단위 테스트)

## API 엔드포인트

| 메서드 | 경로 | 설명 |
|--------|------|------|
| GET | `/api/v2/products` | 전체 상품 조회 |
| GET | `/api/v2/products/{id}` | 상품 상세 조회 |
| POST | `/api/v2/products` | 상품 생성 |
| PUT | `/api/v2/products/{id}` | 상품 수정 |
| DELETE | `/api/v2/products/{id}` | 상품 삭제 |
| GET | `/api/v2/products/search` | 가격 범위 검색 |
| GET | `/api/v2/products/stream` | SSE 실시간 스트림 |

## 실행 방법

### Reactive 프로파일 활성화
```bash
./gradlew bootRun --args='--spring.profiles.active=reactive'
```

### 테스트 실행
```bash
./gradlew test
```

## 주의사항

1. **H2 콘솔**: R2DBC와 JDBC가 다른 인메모리 DB를 사용할 수 있음
2. **트랜잭션**: R2DBC는 `@Transactional` 대신 `TransactionalOperator` 사용
3. **JPA 테스트**: 반드시 R2DBC 자동 구성 제외 필요
4. **운영 환경**: PostgreSQL R2DBC 드라이버 사용 권장

## 다음 버전
- v2.1.0: Reactive Security (WebFlux 보안)
