package com.example.training.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import reactor.core.scheduler.Scheduler;
import reactor.core.scheduler.Schedulers;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Virtual Threads 설정
 * Java 21 Virtual Threads를 활용한 실행기 구성
 */
@Configuration
public class VirtualThreadConfig {

    /**
     * Virtual Thread Executor
     * 각 작업마다 새로운 Virtual Thread 생성
     * 블로킹 작업에 적합
     */
    @Bean
    public ExecutorService virtualThreadExecutor() {
        return Executors.newVirtualThreadPerTaskExecutor();
    }

    /**
     * 블로킹 작업용 Reactor 스케줄러
     * Reactor 스트림에서 블로킹 코드를 Virtual Thread로 실행
     */
    @Bean
    public Scheduler blockingScheduler(ExecutorService virtualThreadExecutor) {
        return Schedulers.fromExecutor(virtualThreadExecutor);
    }
}
