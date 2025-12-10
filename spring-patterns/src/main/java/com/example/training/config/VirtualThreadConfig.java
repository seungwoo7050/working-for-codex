package com.example.training.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import reactor.core.scheduler.Scheduler;
import reactor.core.scheduler.Schedulers;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] Virtual Threads 실행기 설정
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: Java 21 Virtual Threads를 Spring과 Reactor에서 활용
 * - 관련 클론 가이드 단계: [CG-v2.1.0] 2.1.2 Virtual Threads 설정
 * 
 * [LEARN] C 개발자를 위한 Virtual Threads 개념:
 * 
 * 전통적인 스레드 (Platform Thread):
 *   pthread_create() → OS 스레드 1개 생성
 *   스택 크기: 1~2MB
 *   컨텍스트 스위칭: 수 마이크로초
 *   최대 수천 개
 * 
 * Virtual Thread (Java 21+):
 *   경량 스레드 (JVM이 관리)
 *   스택 크기: 수 KB (동적)
 *   컨텍스트 스위칭: 수십 나노초
 *   수십만~수백만 개 가능
 * 
 * 블로킹 I/O 시 동작:
 *   Platform Thread: 스레드가 잠자며 대기 (리소스 낭비)
 *   Virtual Thread: JVM이 캐리어 스레드에서 언마운트 → 다른 작업 처리
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Configuration
public class VirtualThreadConfig {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] Virtual Thread Executor 빈
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * Virtual Thread Executor
     * 각 작업마다 새로운 Virtual Thread 생성
     * 블로킹 작업에 적합
     */
    @Bean
    public ExecutorService virtualThreadExecutor() {
        return Executors.newVirtualThreadPerTaskExecutor();
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] newVirtualThreadPerTaskExecutor():
    // - 제출된 각 작업마다 새로운 Virtual Thread 생성
    // - 스레드 풀 크기 제한 없음
    //
    // C 관점 비교:
    //   // 기존: 스레드 풀
    //   pthread_t pool[100];
    //   for (int i = 0; i < 100; i++) pthread_create(&pool[i], ...);
    //
    //   // Virtual: 제한 없이 생성
    //   for (int i = 0; i < 100000; i++) {
    //       // 각각 경량 스레드 (오버헤드 극소)
    //   }
    //
    // 사용 예:
    //   @Autowired ExecutorService executor;
    //   executor.submit(() -> blockingIoOperation());
    // ─────────────────────────────────────────────────────────────────────────

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] Reactor 스케줄러 (WebFlux 연동)
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 블로킹 작업용 Reactor 스케줄러
     * Reactor 스트림에서 블로킹 코드를 Virtual Thread로 실행
     */
    @Bean
    public Scheduler blockingScheduler(ExecutorService virtualThreadExecutor) {
        return Schedulers.fromExecutor(virtualThreadExecutor);
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] Reactor + Virtual Thread 조합:
    // 
    // WebFlux는 논블로킹이지만, 레거시 블로킹 코드를 호출해야 할 때:
    //   Mono.fromCallable(() -> legacyBlockingCall())
    //       .subscribeOn(blockingScheduler)  // Virtual Thread에서 실행
    //
    // 이점:
    // 1) 이벤트 루프 스레드를 블로킹하지 않음
    // 2) Virtual Thread 덕분에 수천 개의 동시 블로킹 호출 가능
    //
    // C 관점: epoll 이벤트 루프 + 블로킹 작업용 스레드 풀 조합
    // ─────────────────────────────────────────────────────────────────────────
}
