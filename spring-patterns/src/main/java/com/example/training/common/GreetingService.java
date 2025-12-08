package com.example.training.common;

import java.util.concurrent.atomic.AtomicInteger;

import org.springframework.cache.annotation.Cacheable;
import org.springframework.stereotype.Service;
import org.springframework.util.StringUtils;

@Service
public class GreetingService {

    private final AtomicInteger invocationCounter = new AtomicInteger();

    @Cacheable("popularIssues")
    public String greet(String name) {
        String sanitized = StringUtils.hasText(name) ? name.trim() : "there";
        int count = invocationCounter.incrementAndGet();
        return "Hello, %s! (call #%d)".formatted(sanitized, count);
    }

    public int getInvocationCount() {
        return invocationCounter.get();
    }

    void resetInvocationCount() {
        invocationCounter.set(0);
    }
}