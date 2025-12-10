package com.example.training.hybrid;

import com.example.training.hybrid.adapter.LegacySystemAdapter;
import com.example.training.hybrid.service.HybridProductService;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import reactor.core.scheduler.Scheduler;
import reactor.core.scheduler.Schedulers;
import reactor.test.StepVerifier;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

import static org.assertj.core.api.Assertions.assertThat;

/**
 * Virtual Threads 통합 테스트
 */
@ExtendWith(MockitoExtension.class)
class VirtualThreadsTest {

    @Test
    void virtualThreadExecutor_shouldRunTasks() {
        ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();
        AtomicInteger counter = new AtomicInteger(0);

        for (int i = 0; i < 1000; i++) {
            executor.submit(counter::incrementAndGet);
        }

        // 잠시 대기 후 확인
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        assertThat(counter.get()).isEqualTo(1000);
        executor.shutdown();
    }

    @Test
    void hybridProductService_wrapBlocking_shouldWork() {
        Scheduler scheduler = Schedulers.fromExecutor(
                Executors.newVirtualThreadPerTaskExecutor()
        );
        HybridProductService service = new HybridProductService(scheduler);

        StepVerifier.create(
                service.wrapBlocking(() -> {
                    Thread.sleep(10);
                    return "result";
                })
        )
        .expectNext("result")
        .verifyComplete();
    }

    @Test
    void hybridProductService_zipBlocking_shouldRunInParallel() {
        Scheduler scheduler = Schedulers.fromExecutor(
                Executors.newVirtualThreadPerTaskExecutor()
        );
        HybridProductService service = new HybridProductService(scheduler);

        long start = System.currentTimeMillis();

        StepVerifier.create(
                service.zipBlocking(
                        () -> {
                            Thread.sleep(100);
                            return "A";
                        },
                        () -> {
                            Thread.sleep(100);
                            return "B";
                        },
                        (a, b) -> a + b
                )
        )
        .expectNext("AB")
        .verifyComplete();

        long duration = System.currentTimeMillis() - start;
        // 병렬 실행이므로 200ms 미만이어야 함
        assertThat(duration).isLessThan(200);
    }

    @Test
    void legacySystemAdapter_executeReactive_shouldWork() {
        ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();
        Scheduler scheduler = Schedulers.fromExecutor(executor);
        LegacySystemAdapter adapter = new LegacySystemAdapter(executor, scheduler);

        StepVerifier.create(
                adapter.executeReactive(() -> {
                    Thread.sleep(10);
                    return 42;
                })
        )
        .expectNext(42)
        .verifyComplete();

        executor.shutdown();
    }

    @Test
    void legacySystemAdapter_executeAsync_shouldReturnCompletableFuture() throws Exception {
        ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();
        Scheduler scheduler = Schedulers.fromExecutor(executor);
        LegacySystemAdapter adapter = new LegacySystemAdapter(executor, scheduler);

        var future = adapter.executeAsync(() -> {
            Thread.sleep(10);
            return "async-result";
        });

        assertThat(future.get()).isEqualTo("async-result");

        executor.shutdown();
    }

    @Test
    void virtualThreads_shouldHandleHighConcurrency() throws InterruptedException {
        ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();
        AtomicInteger completed = new AtomicInteger(0);
        int taskCount = 10000;

        for (int i = 0; i < taskCount; i++) {
            executor.submit(() -> {
                try {
                    Thread.sleep(1);  // Simulate blocking I/O
                    completed.incrementAndGet();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });
        }

        // 충분한 대기 시간
        Thread.sleep(5000);

        assertThat(completed.get()).isEqualTo(taskCount);
        executor.shutdown();
    }

    @Test
    void virtualThread_isVirtual_shouldBeTrue() {
        ExecutorService executor = Executors.newVirtualThreadPerTaskExecutor();
        AtomicInteger virtualCount = new AtomicInteger(0);

        for (int i = 0; i < 10; i++) {
            executor.submit(() -> {
                if (Thread.currentThread().isVirtual()) {
                    virtualCount.incrementAndGet();
                }
            });
        }

        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }

        assertThat(virtualCount.get()).isEqualTo(10);
        executor.shutdown();
    }
}
