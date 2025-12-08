package com.example.training.common;

import static org.assertj.core.api.Assertions.assertThat;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cache.CacheManager;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.cache.concurrent.ConcurrentMapCacheManager;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.junit.jupiter.SpringExtension;
import org.springframework.test.context.support.AnnotationConfigContextLoader;

@ExtendWith(SpringExtension.class)
@ContextConfiguration(classes = GreetingServiceTests.TestConfig.class, loader = AnnotationConfigContextLoader.class)
class GreetingServiceTests {

    @Autowired
    private GreetingService greetingService;

    @Autowired
    private CacheManager cacheManager;

    @AfterEach
    void tearDown() {
        cacheManager.getCacheNames().forEach(name -> {
            if (cacheManager.getCache(name) != null) {
                cacheManager.getCache(name).clear();
            }
        });
        greetingService.resetInvocationCount();
    }

    @Test
    void greetsNameAndCachesResult() {
        String first = greetingService.greet("Coder");
        String second = greetingService.greet("Coder");

        assertThat(second).isEqualTo(first);
        assertThat(greetingService.getInvocationCount()).isEqualTo(1);
    }

    @Test
    void handlesBlankNames() {
        String greeting = greetingService.greet("   ");

        assertThat(greeting).contains("there");
        assertThat(greetingService.getInvocationCount()).isEqualTo(1);
    }

    @Configuration
    @EnableCaching
    static class TestConfig {

        @Bean
        CacheManager cacheManager() {
            return new ConcurrentMapCacheManager("popularIssues");
        }

        @Bean
        GreetingService greetingService() {
            return new GreetingService();
        }
    }
}