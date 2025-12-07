package com.example.training.cache.controller;

import com.example.training.cache.service.CacheManagementService;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Collection;
import java.util.List;
import java.util.stream.Collectors;

/**
 * 캐시 관리 컨트롤러
 * 캐시 상태 조회 및 관리 API
 */
@RestController
@RequestMapping("/api/cache/admin")
public class CacheAdminController {

    private final CacheManagementService cacheManagementService;

    public CacheAdminController(CacheManagementService cacheManagementService) {
        this.cacheManagementService = cacheManagementService;
    }

    /**
     * 캐시 목록 조회
     */
    @GetMapping
    public Collection<String> getCacheNames() {
        return cacheManagementService.getCacheNames();
    }

    /**
     * 전체 캐시 통계 조회
     */
    @GetMapping("/stats")
    public List<CacheManagementService.CacheStats> getAllCacheStats() {
        return cacheManagementService.getCacheNames().stream()
                .map(cacheManagementService::getCacheStats)
                .collect(Collectors.toList());
    }

    /**
     * 특정 캐시 통계 조회
     */
    @GetMapping("/stats/{cacheName}")
    public ResponseEntity<CacheManagementService.CacheStats> getCacheStats(
            @PathVariable String cacheName) {
        var stats = cacheManagementService.getCacheStats(cacheName);
        if (!stats.exists()) {
            return ResponseEntity.notFound().build();
        }
        return ResponseEntity.ok(stats);
    }

    /**
     * 특정 캐시 무효화
     */
    @DeleteMapping("/{cacheName}")
    public ResponseEntity<Void> evictCache(@PathVariable String cacheName) {
        cacheManagementService.evictCache(cacheName);
        return ResponseEntity.noContent().build();
    }

    /**
     * 특정 캐시 키 무효화
     */
    @DeleteMapping("/{cacheName}/keys/{key}")
    public ResponseEntity<Void> evictCacheKey(
            @PathVariable String cacheName,
            @PathVariable String key) {
        cacheManagementService.evictCacheKey(cacheName, key);
        return ResponseEntity.noContent().build();
    }

    /**
     * 전체 캐시 무효화
     */
    @DeleteMapping
    public ResponseEntity<Void> evictAllCaches() {
        cacheManagementService.evictAllCaches();
        return ResponseEntity.noContent().build();
    }
}
