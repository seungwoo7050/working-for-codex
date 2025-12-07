package com.example.training.config;

import org.springframework.cache.CacheManager;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.cache.concurrent.ConcurrentMapCacheManager;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;

/**
 * 캐시 설정
 * Spring Cache Abstraction을 사용한 인메모리 캐싱
 */
@Configuration
@EnableCaching
public class CachingConfig {

    /**
     * 캐시 매니저 빈
     * ConcurrentMapCacheManager: 인메모리 캐시 (개발/테스트용)
     * 운영 환경에서는 RedisCacheManager 사용 권장
     */
    @Bean
    @Primary
    public CacheManager cacheManager() {
        return new ConcurrentMapCacheManager(
                "products",        // 상품 목록 (TTL: 5분)
                "product",         // 개별 상품 (TTL: 30분)
                "categories",      // 카테고리 (TTL: 1시간)
                "hot-items",       // 인기 상품 (TTL: 1분)
                "popularIssues"    // 기존 인기 이슈 캐시
        );
    }
}
