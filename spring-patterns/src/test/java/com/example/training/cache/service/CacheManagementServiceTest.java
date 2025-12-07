package com.example.training.cache.service;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.springframework.cache.CacheManager;
import org.springframework.cache.concurrent.ConcurrentMapCacheManager;

import static org.assertj.core.api.Assertions.assertThat;

/**
 * CacheManagementService 단위 테스트
 */
class CacheManagementServiceTest {

    private CacheManagementService cacheManagementService;
    private CacheManager cacheManager;

    @BeforeEach
    void setUp() {
        cacheManager = new ConcurrentMapCacheManager("products", "product", "categories", "hot-items");
        cacheManagementService = new CacheManagementService(cacheManager);
    }

    @Test
    @DisplayName("캐시 이름 목록을 조회할 수 있어야 한다")
    void shouldGetCacheNames() {
        // when
        var cacheNames = cacheManagementService.getCacheNames();

        // then
        assertThat(cacheNames).contains("products", "product", "categories", "hot-items");
    }

    @Test
    @DisplayName("특정 캐시를 무효화할 수 있어야 한다")
    void shouldEvictSpecificCache() {
        // given
        var cache = cacheManager.getCache("products");
        cache.put("testKey", "testValue");

        // when
        cacheManagementService.evictCache("products");

        // then
        var result = cache.get("testKey");
        assertThat(result).isNull();
    }

    @Test
    @DisplayName("특정 캐시 키를 무효화할 수 있어야 한다")
    void shouldEvictSpecificCacheKey() {
        // given
        var cache = cacheManager.getCache("products");
        cache.put("key1", "value1");
        cache.put("key2", "value2");

        // when
        cacheManagementService.evictCacheKey("products", "key1");

        // then
        assertThat(cache.get("key1")).isNull();
        assertThat(cache.get("key2")).isNotNull();
    }

    @Test
    @DisplayName("전체 캐시를 무효화할 수 있어야 한다")
    void shouldEvictAllCaches() {
        // given
        cacheManager.getCache("products").put("key1", "value1");
        cacheManager.getCache("product").put("key2", "value2");

        // when
        cacheManagementService.evictAllCaches();

        // then
        assertThat(cacheManager.getCache("products").get("key1")).isNull();
        assertThat(cacheManager.getCache("product").get("key2")).isNull();
    }

    @Test
    @DisplayName("캐시 통계를 조회할 수 있어야 한다")
    void shouldGetCacheStats() {
        // given
        var cache = cacheManager.getCache("products");
        cache.put("key1", "value1");
        cache.put("key2", "value2");

        // when
        var stats = cacheManagementService.getCacheStats("products");

        // then
        assertThat(stats.cacheName()).isEqualTo("products");
        assertThat(stats.exists()).isTrue();
        assertThat(stats.estimatedSize()).isEqualTo(2);
    }

    @Test
    @DisplayName("존재하지 않는 캐시 통계 조회 시 exists가 false여야 한다")
    void shouldReturnNotExistsForUnknownCache() {
        // when
        var stats = cacheManagementService.getCacheStats("unknown-cache");

        // then
        assertThat(stats.exists()).isFalse();
    }

    @Test
    @DisplayName("존재하지 않는 캐시 무효화 시 에러가 발생하지 않아야 한다")
    void shouldHandleEvictingNonExistentCache() {
        // when & then - no exception
        cacheManagementService.evictCache("non-existent-cache");
    }
}
