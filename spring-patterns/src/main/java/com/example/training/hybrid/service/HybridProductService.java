package com.example.training.hybrid.service;

import org.springframework.stereotype.Service;
import reactor.core.publisher.Mono;
import reactor.core.scheduler.Scheduler;

import java.util.concurrent.Callable;

/**
 * 하이브리드 상품 서비스
 * 블로킹 레거시 코드를 리액티브 스트림으로 래핑
 * Virtual Threads를 사용하여 블로킹 호출 처리
 */
@Service
public class HybridProductService {

    private final Scheduler blockingScheduler;

    public HybridProductService(Scheduler blockingScheduler) {
        this.blockingScheduler = blockingScheduler;
    }

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

    /**
     * 여러 블로킹 호출을 병렬로 실행
     *
     * @param call1 첫 번째 블로킹 호출
     * @param call2 두 번째 블로킹 호출
     * @param combiner 결과 결합 함수
     * @param <T1> 첫 번째 반환 타입
     * @param <T2> 두 번째 반환 타입
     * @param <R> 결합된 결과 타입
     * @return 비동기 결합 결과
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

    /**
     * 세 개의 블로킹 호출을 병렬로 실행
     *
     * @param call1 첫 번째 블로킹 호출
     * @param call2 두 번째 블로킹 호출
     * @param call3 세 번째 블로킹 호출
     * @param <T1> 첫 번째 반환 타입
     * @param <T2> 두 번째 반환 타입
     * @param <T3> 세 번째 반환 타입
     * @return 튜플3 결과
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
}
