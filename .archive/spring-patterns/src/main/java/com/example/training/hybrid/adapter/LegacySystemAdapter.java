package com.example.training.hybrid.adapter;

import org.springframework.stereotype.Component;
import reactor.core.publisher.Mono;
import reactor.core.scheduler.Scheduler;

import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;

// [FILE]
// - 목적: 블로킹 레거시 시스템을 리액티브 파이프라인에 안전하게 통합
// - 주요 역할: 블로킹 API → CompletableFuture/Mono 변환
// - 관련 클론 가이드 단계: [CG-v2.1.0] 2.1.3 레거시 시스템 어댑터
// - 권장 읽는 순서: Order 1(executeAsync) → Order 2(executeReactive) → Order 3(executeWithRetry)
//
// [LEARN] C 개발자를 위한 설명:
// 현실의 시스템은 "모든 것을 새로 만들 수 없다".
// 기존 블로킹 라이브러리/API를 리액티브 시스템에 통합해야 하는 경우가 많다.
//
// 문제 상황:
// - 레거시 JDBC 드라이버 (블로킹)
// - 레거시 HTTP 클라이언트 (블로킹)
// - 레거시 파일 I/O (블로킹)
//
// 해결책 (이 클래스의 역할):
// 1. 블로킹 호출을 별도 스레드(Virtual Thread)에서 실행
// 2. 결과를 Mono/CompletableFuture로 래핑하여 비동기 반환
// 3. 이벤트 루프 스레드는 블로킹되지 않음
//
// C 관점 비유:
// // 잘못된 방식: 메인 이벤트 루프에서 블로킹
// while (1) {
//     epoll_wait(epfd, events, ...);
//     legacy_blocking_call();  // 전체 서버 멈춤!
// }
//
// // 올바른 방식: 워커 스레드로 위임
// while (1) {
//     epoll_wait(epfd, events, ...);
//     dispatch_to_worker(legacy_blocking_call);  // 비동기 위임
// }

@Component
public class LegacySystemAdapter {

    private final ExecutorService virtualThreadExecutor;
    // [LEARN] Virtual Thread Executor: 블로킹 작업용 스레드 풀
    // 각 작업마다 경량 Virtual Thread 생성
    
    private final Scheduler blockingScheduler;
    // [LEARN] Reactor Scheduler: Mono/Flux 실행 컨텍스트 지정

    public LegacySystemAdapter(ExecutorService virtualThreadExecutor,
                                Scheduler blockingScheduler) {
        this.virtualThreadExecutor = virtualThreadExecutor;
        this.blockingScheduler = blockingScheduler;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] CompletableFuture 방식 (Java 표준 API)
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * 블로킹 작업을 CompletableFuture로 실행
     * Spring WebFlux 외부에서도 사용 가능
     */
    public <T> CompletableFuture<T> executeAsync(Callable<T> task) {
        return CompletableFuture.supplyAsync(() -> {
            try {
                return task.call();
            } catch (Exception e) {
                throw new RuntimeException("Blocking task failed", e);
            }
        }, virtualThreadExecutor);
    }
    // [LEARN] CompletableFuture: Java 8+ 비동기 결과 컨테이너
    // C의 pthread_create() + 결과 포인터와 유사
    // supplyAsync()에 Executor를 지정하면 해당 스레드 풀에서 실행

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] Mono 방식 (Reactor/WebFlux 통합)
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * 블로킹 작업을 Mono로 래핑
     * WebFlux 파이프라인에 직접 연결 가능
     */
    public <T> Mono<T> executeReactive(Callable<T> task) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler);
    }
    // [LEARN] Mono.fromCallable(): 블로킹 람다 → Mono 변환
    // subscribeOn(): 실행 스레드 지정 (이벤트 루프가 아닌 Virtual Thread)

    /**
     * 타임아웃이 있는 블로킹 작업 실행
     */
    public <T> Mono<T> executeWithTimeout(Callable<T> task, long timeoutMillis) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler)
                .timeout(java.time.Duration.ofMillis(timeoutMillis));
    }
    // [LEARN] timeout(): 지정 시간 초과 시 TimeoutException 발생
    // 레거시 시스템이 응답하지 않을 때 무한 대기 방지

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 재시도 로직
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * 재시도 로직이 있는 블로킹 작업 실행
     */
    public <T> Mono<T> executeWithRetry(Callable<T> task, int maxRetries) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler)
                .retry(maxRetries);
    }
    // [LEARN] retry(n): 에러 발생 시 n번까지 재시도
    // 네트워크 일시 장애, 일시적 부하 등에 대응
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: 어댑터 패턴, 블로킹→리액티브 변환
//
// 레거시 통합 패턴 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 왜 별도 스레드에서 실행?
//    - WebFlux 이벤트 루프는 소수의 스레드로 많은 요청 처리
//    - 블로킹 호출이 이벤트 루프를 막으면 전체 처리량 급감
//    - 별도 스레드에서 블로킹 → 이벤트 루프는 계속 동작
//
// 2. Virtual Thread의 장점:
//    - 기존 Platform Thread: 스택 1-2MB, 최대 수천 개
//    - Virtual Thread: 스택 수 KB, 수십만 개 가능
//    - 블로킹 호출이 많아도 스레드 고갈 없음
//
// 3. 사용 예시:
//    // 레거시 JDBC 호출
//    Mono<User> user = adapter.executeReactive(
//        () -> legacyUserDao.findById(id)
//    );
//
//    // WebFlux 파이프라인에 연결
//    return user.map(UserResponse::from);
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. hybrid/service/HybridProductService.java - 실제 사용 예시
// 2. hybrid/client/ExternalApiClient.java - HTTP 호출 래핑 예시
// ═══════════════════════════════════════════════════════════════════════════════
