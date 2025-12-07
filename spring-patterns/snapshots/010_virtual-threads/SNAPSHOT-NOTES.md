# Snapshot 010: Virtual Threads

## 버전 정보
- **버전**: v2.1.0
- **기반**: v2.0.0 (WebFlux & R2DBC)
- **날짜**: 2025-01

## 핵심 변경 사항

### 1. Java 21 업그레이드
- `sourceCompatibility = '21'`
- Virtual Threads (Project Loom) 지원

### 2. Virtual Threads 설정
- **spring.threads.virtual.enabled=true**: Tomcat Virtual Threads 활성화
- **VirtualThreadConfig**: Virtual Thread Executor 및 Reactor Scheduler
- **AsyncConfig**: @Async 메서드에 Virtual Threads 적용

### 3. 하이브리드 서비스 레이어
- **HybridProductService**: 블로킹 코드를 리액티브로 래핑
- **LegacySystemAdapter**: 레거시 시스템 통합
- **ExternalApiClient**: 병렬 HTTP 호출 (Rate Limiting 포함)

## 새로 추가된 파일

```
src/main/java/com/example/training/
├── config/
│   ├── VirtualThreadConfig.java    # Virtual Thread 설정
│   └── AsyncConfig.java            # @Async 설정
└── hybrid/
    ├── service/
    │   └── HybridProductService.java
    ├── adapter/
    │   └── LegacySystemAdapter.java
    └── client/
        └── ExternalApiClient.java

src/test/java/com/example/training/hybrid/
└── VirtualThreadsTest.java
```

## 설정 변경

### build.gradle
```groovy
java {
    sourceCompatibility = '21'
}
```

### application.yml
```yaml
spring:
  threads:
    virtual:
      enabled: true
```

## 주요 패턴

### 1. 블로킹 코드 래핑
```java
public <T> Mono<T> wrapBlocking(Callable<T> blockingCall) {
    return Mono.fromCallable(blockingCall)
            .subscribeOn(blockingScheduler);
}
```

### 2. 병렬 블로킹 호출
```java
public <T1, T2, R> Mono<R> zipBlocking(
        Callable<T1> call1,
        Callable<T2> call2,
        BiFunction<T1, T2, R> combiner) {
    return Mono.zip(
        Mono.fromCallable(call1).subscribeOn(blockingScheduler),
        Mono.fromCallable(call2).subscribeOn(blockingScheduler),
        combiner
    );
}
```

### 3. Rate Limited HTTP 호출
```java
public Mono<String> getWithRateLimit(String url) {
    return Mono.fromCallable(() -> {
        rateLimiter.acquire();
        try {
            return executeRequest(url);
        } finally {
            rateLimiter.release();
        }
    }).subscribeOn(blockingScheduler);
}
```

## Virtual Threads vs Platform Threads

| 항목 | Platform Threads | Virtual Threads |
|------|------------------|-----------------|
| 생성 비용 | ~1MB 스택 | ~KB 스택 |
| 동시성 한계 | 수천 개 | 수백만 개 |
| 블로킹 I/O | 스레드 점유 | 자동 언마운트 |
| 사용 시점 | CPU 집약적 | I/O 집약적 |

## 테스트 검증

- `virtualThreadExecutor_shouldRunTasks`: VT 실행기 동작 확인
- `hybridProductService_wrapBlocking_shouldWork`: 래핑 동작 확인
- `hybridProductService_zipBlocking_shouldRunInParallel`: 병렬 실행 확인
- `virtualThreads_shouldHandleHighConcurrency`: 10,000 동시 작업 처리
- `virtualThread_isVirtual_shouldBeTrue`: VT 여부 확인

## 실행 방법

```bash
./gradlew bootRun
./gradlew test
```

## 주의사항

1. **Java 21 필수**: Virtual Threads는 Java 21+ 기능
2. **synchronized 최소화**: VT 핀닝 방지
3. **ThreadLocal 주의**: 메모리 누수 가능성
4. **CPU 작업 분리**: Platform Threads 사용 권장

## 다음 버전
- v2.2.0: Rate Limiting (API 속도 제한)
