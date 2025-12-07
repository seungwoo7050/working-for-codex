package com.example.training.hybrid.adapter;

import org.springframework.stereotype.Component;
import reactor.core.publisher.Mono;
import reactor.core.scheduler.Scheduler;

import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutorService;

/**
 * 레거시 시스템 어댑터
 * 블로킹 레거시 API를 비동기로 변환
 */
@Component
public class LegacySystemAdapter {

    private final ExecutorService virtualThreadExecutor;
    private final Scheduler blockingScheduler;

    public LegacySystemAdapter(ExecutorService virtualThreadExecutor,
                                Scheduler blockingScheduler) {
        this.virtualThreadExecutor = virtualThreadExecutor;
        this.blockingScheduler = blockingScheduler;
    }

    /**
     * 블로킹 작업을 CompletableFuture로 실행 (Virtual Threads)
     *
     * @param task 블로킹 작업
     * @param <T> 반환 타입
     * @return CompletableFuture 결과
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

    /**
     * 블로킹 작업을 Mono로 래핑 (Reactor + Virtual Threads)
     *
     * @param task 블로킹 작업
     * @param <T> 반환 타입
     * @return Mono 결과
     */
    public <T> Mono<T> executeReactive(Callable<T> task) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler);
    }

    /**
     * 타임아웃이 있는 블로킹 작업 실행
     *
     * @param task 블로킹 작업
     * @param timeoutMillis 타임아웃 (밀리초)
     * @param <T> 반환 타입
     * @return Mono 결과 (타임아웃 시 에러)
     */
    public <T> Mono<T> executeWithTimeout(Callable<T> task, long timeoutMillis) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler)
                .timeout(java.time.Duration.ofMillis(timeoutMillis));
    }

    /**
     * 재시도 로직이 있는 블로킹 작업 실행
     *
     * @param task 블로킹 작업
     * @param maxRetries 최대 재시도 횟수
     * @param <T> 반환 타입
     * @return Mono 결과
     */
    public <T> Mono<T> executeWithRetry(Callable<T> task, int maxRetries) {
        return Mono.fromCallable(task)
                .subscribeOn(blockingScheduler)
                .retry(maxRetries);
    }
}
