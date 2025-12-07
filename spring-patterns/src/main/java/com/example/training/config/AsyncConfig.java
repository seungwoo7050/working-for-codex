package com.example.training.config;

import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.annotation.AsyncConfigurer;
import org.springframework.scheduling.annotation.EnableAsync;

import java.util.concurrent.Executor;
import java.util.concurrent.Executors;

/**
 * @Async 메서드용 Virtual Threads 설정
 */
@Configuration
@EnableAsync
public class AsyncConfig implements AsyncConfigurer {

    /**
     * @Async 메서드가 Virtual Thread에서 실행되도록 설정
     */
    @Override
    public Executor getAsyncExecutor() {
        return Executors.newVirtualThreadPerTaskExecutor();
    }
}
