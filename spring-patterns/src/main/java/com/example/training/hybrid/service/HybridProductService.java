package com.example.training.hybrid.service;

import org.springframework.stereotype.Service;
import reactor.core.publisher.Mono;
import reactor.core.scheduler.Scheduler;

import java.util.concurrent.Callable;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] HybridProductService - 블로킹 ↔ 리액티브 브릿지
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 기존 블로킹 코드를 리액티브 스트림으로 안전하게 래핑
 * - 관련 클론 가이드 단계: [CG-v2.1.0] 2.1.3 하이브리드 아키텍처
 * 
 * [LEARN] 하이브리드 패턴이 필요한 이유:
 * 
 * 문제 상황:
 * - WebFlux(리액티브)를 도입했지만 레거시 블로킹 코드가 남아있음
 * - 블로킹 코드를 리액티브 파이프라인에서 직접 호출하면?
 *   → 이벤트 루프 스레드가 블로킹 → 전체 서버 처리량 급감
 * 
 * 해결책:
 * - 블로킹 코드를 별도 스케줄러(Virtual Thread)로 오프로드
 * - 이벤트 루프는 계속 동작, 블로킹 작업은 별도 스레드에서 처리
 * 
 * C 관점:
 *   // 잘못된 방식: 메인 epoll 루프에서 블로킹
 *   while (1) {
 *       epoll_wait(...);
 *       blocking_db_call();  // 루프 멈춤!
 *   }
 * 
 *   // 올바른 방식: 블로킹 작업을 워커 스레드로 위임
 *   while (1) {
 *       epoll_wait(...);
 *       pthread_create(&worker, blocking_db_call, ...);  // 위임
 *   }
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Service
public class HybridProductService {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 블로킹 작업용 스케줄러 주입
    // ─────────────────────────────────────────────────────────────────────────
    private final Scheduler blockingScheduler;
    // [LEARN] Scheduler: Reactor에서 작업을 실행할 스레드 풀
    // VirtualThreadConfig에서 Virtual Thread 기반으로 설정됨

    public HybridProductService(Scheduler blockingScheduler) {
        this.blockingScheduler = blockingScheduler;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 블로킹 → 리액티브 래핑
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 블로킹 작업을 리액티브로 래핑
     * Virtual Thread에서 실행되어 이벤트 루프 차단 방지
     *
     * @param blockingCall 블로킹 호출
     * @param <T> 반환 타입
     * @return 비동기 결과
     */
    public <T> Mono<T> wrapBlocking(Callable<T> blockingCall) {
        return Mono.fromCallable(blockingCall)
                .subscribeOn(blockingScheduler);
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] Mono.fromCallable():
    // - Callable(블로킹 람다)을 Mono로 변환
    // - 구독(subscribe) 시점에 실행
    //
    // .subscribeOn(blockingScheduler):
    // - 어떤 스레드에서 실행할지 지정
    // - blockingScheduler = Virtual Thread 기반
    // - 이벤트 루프 스레드가 아닌 Virtual Thread에서 블로킹 작업 수행
    //
    // 사용 예:
    //   Mono<Product> productMono = hybridService.wrapBlocking(
    //       () -> legacyDao.findById(id)  // 블로킹 JDBC 호출
    //   );
    // ─────────────────────────────────────────────────────────────────────────

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 병렬 블로킹 호출 (2개)
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 여러 블로킹 호출을 병렬로 실행
     */
    public <T1, T2, R> Mono<R> zipBlocking(
            Callable<T1> call1,
            Callable<T2> call2,
            java.util.function.BiFunction<T1, T2, R> combiner) {
        return Mono.zip(
                Mono.fromCallable(call1).subscribeOn(blockingScheduler),
                Mono.fromCallable(call2).subscribeOn(blockingScheduler),
                combiner
        );
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] Mono.zip(): 여러 Mono를 병렬 실행 후 결과 결합
    //
    // 순차 호출 (느림):
    //   Product p = dao1.findProduct(id);  // 100ms
    //   Review r = dao2.findReviews(id);   // 100ms
    //   // 총 200ms
    //
    // 병렬 호출 (빠름):
    //   Mono.zip(
    //       wrapBlocking(() -> dao1.findProduct(id)),  // 100ms
    //       wrapBlocking(() -> dao2.findReviews(id)),  // 100ms 동시
    //       (product, reviews) -> new ProductDetail(product, reviews)
    //   );  // 총 100ms (병렬)
    //
    // C 관점: 두 스레드를 생성하고 pthread_join으로 대기
    // ─────────────────────────────────────────────────────────────────────────

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 병렬 블로킹 호출 (3개)
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 세 개의 블로킹 호출을 병렬로 실행
     */
    public <T1, T2, T3> Mono<reactor.util.function.Tuple3<T1, T2, T3>> zipBlocking3(
            Callable<T1> call1,
            Callable<T2> call2,
            Callable<T3> call3) {
        return Mono.zip(
                Mono.fromCallable(call1).subscribeOn(blockingScheduler),
                Mono.fromCallable(call2).subscribeOn(blockingScheduler),
                Mono.fromCallable(call3).subscribeOn(blockingScheduler)
        );
    }
    // [LEARN] Tuple3: 세 개 결과를 담는 불변 컨테이너
    // 사용 예:
    //   zipBlocking3(
    //       () -> productDao.find(id),
    //       () -> reviewDao.find(id),
    //       () -> inventoryDao.find(id)
    //   ).map(tuple -> {
    //       Product p = tuple.getT1();
    //       Reviews r = tuple.getT2();
    //       Inventory i = tuple.getT3();
    //       return new FullProduct(p, r, i);
    //   });
}
