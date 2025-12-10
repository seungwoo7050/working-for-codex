package com.example.training.cache.service;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.cache.CacheManager;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.util.Objects;

/**
 * 캐시 관리 서비스
 * 캐시 무효화, 통계, 헬스 체크 기능 제공
 */
@Service
public class CacheManagementService {

    private static final Logger log = LoggerFactory.getLogger(CacheManagementService.class);

    private final CacheManager cacheManager;

    public CacheManagementService(CacheManager cacheManager) {
        this.cacheManager = cacheManager;
    }

    /**
     * 특정 캐시 무효화
     */
    public void evictCache(String cacheName) {
        var cache = cacheManager.getCache(cacheName);
        if (cache != null) {
            cache.clear();
            log.info("Cache '{}' cleared", cacheName);
        } else {
            log.warn("Cache '{}' not found", cacheName);
        }
    }

    /**
     * 특정 캐시 키 무효화
     */
    public void evictCacheKey(String cacheName, String key) {
        var cache = cacheManager.getCache(cacheName);
        if (cache != null) {
            cache.evict(key);
            log.info("Cache key '{}' evicted from '{}'", key, cacheName);
        } else {
            log.warn("Cache '{}' not found", cacheName);
        }
    }

    /**
     * 전체 캐시 무효화
     */
    public void evictAllCaches() {
        cacheManager.getCacheNames().forEach(this::evictCache);
        log.info("All caches cleared");
    }

    /**
     * 캐시 이름 목록 조회
     */
    public java.util.Collection<String> getCacheNames() {
        return cacheManager.getCacheNames();
    }

    /**
     * 캐시 통계 정보 (ConcurrentMapCache는 제한적)
     */
    public CacheStats getCacheStats(String cacheName) {
        var cache = cacheManager.getCache(cacheName);
        if (cache == null) {
            return new CacheStats(cacheName, false, 0);
        }
        
        // ConcurrentMapCache는 직접 사이즈 접근 불가
        // 운영 환경에서는 CaffeineCacheManager나 RedisCacheManager 사용
        Object nativeCache = cache.getNativeCache();
        int size = 0;
        if (nativeCache instanceof java.util.concurrent.ConcurrentHashMap<?, ?> map) {
            size = map.size();
        }
        
        return new CacheStats(cacheName, true, size);
    }

    /**
     * 주기적 캐시 정리 (인기 상품 캐시 - 1분마다)
     */
    @Scheduled(fixedRate = 60000)
    public void evictHotItemsCache() {
        evictCache("hot-items");
    }

    /**
     * 캐시 통계 DTO
     */
    public record CacheStats(String cacheName, boolean exists, int estimatedSize) {}
}
