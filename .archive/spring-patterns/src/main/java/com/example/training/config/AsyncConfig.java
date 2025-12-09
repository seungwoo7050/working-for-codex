package com.example.training.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.AsyncConfigurer;
import org.springframework.scheduling.annotation.EnableAsync;

import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] 비동기 처리 설정 - Virtual Threads 연동
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: @Async 메서드가 Virtual Thread에서 실행되도록 설정
 * - 관련 클론 가이드 단계: [CG-v2.1.0] 2.1.2 Virtual Threads 적용
 * 
 * [LEARN] C 개발자를 위한 설명:
 * 기존 Java: Thread Pool (pthread_create + 풀 관리)
 * - 스레드 수 제한 (보통 10~200개)
 * - 블로킹 I/O시 스레드 낭비
 * 
 * Virtual Threads (Java 21+):
 * - 경량 스레드 (수십만 개 생성 가능)
 * - JVM이 캐리어 스레드 위에 스케줄링
 * - 블로킹 I/O시 자동으로 언마운트 → 캐리어 스레드 재활용
 * 
 * C 관점: coroutine/fiber와 유사하지만 JVM이 자동 관리
 * ═══════════════════════════════════════════════════════════════════════════════
 */

// ─────────────────────────────────────────────────────────────────────────────
// [Order 1] 스프링 설정 클래스 선언
// ─────────────────────────────────────────────────────────────────────────────
@Configuration
// [LEARN] @Configuration: 이 클래스가 스프링 설정 클래스임을 선언
// 내부의 @Bean 메서드들이 스프링 컨테이너에 빈으로 등록됨

@EnableAsync
// [LEARN] @EnableAsync: @Async 어노테이션을 활성화
// 이 설정 없이 @Async를 사용하면 동기식으로 실행됨!
public class AsyncConfig implements AsyncConfigurer {
    // [LEARN] AsyncConfigurer 인터페이스:
    // 비동기 실행에 사용할 Executor를 커스터마이징할 수 있게 해줌

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] Virtual Thread Executor 설정
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * @Async 메서드가 Virtual Thread에서 실행되도록 설정
     */
    @Override
    public Executor getAsyncExecutor() {
        return Executors.newVirtualThreadPerTaskExecutor();
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] newVirtualThreadPerTaskExecutor():
    // - 각 작업마다 새로운 Virtual Thread 생성
    // - 풀 크기 제한 없음 (수십만 개 가능)
    // - 기존: newFixedThreadPool(10) → 10개 스레드 재사용
    //
    // C 관점 비교:
    // 기존 Thread Pool:
    //   pthread_t pool[10];  // 고정 크기 풀
    //   pthread_create(&pool[i], ...);  // 미리 생성
    //   // 작업 큐에서 꺼내서 재사용
    //
    // Virtual Threads:
    //   // 작업마다 경량 스레드 생성 (오버헤드 극소)
    //   // JVM이 자동으로 OS 스레드에 스케줄링
    //
    // 사용 예:
    //   @Async
    //   public void sendEmail(String to) {
    //       // 이 메서드는 Virtual Thread에서 비동기 실행
    //   }
    // ─────────────────────────────────────────────────────────────────────────
}
