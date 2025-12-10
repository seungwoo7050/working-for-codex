package com.example.training.cache.controller;

import com.example.training.cache.service.CacheManagementService;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

import java.util.Collection;
import java.util.List;

import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

/**
 * CacheAdminController 단위 테스트
 */
@ExtendWith(MockitoExtension.class)
class CacheAdminControllerTest {

    @Mock
    private CacheManagementService cacheManagementService;

    @InjectMocks
    private CacheAdminController controller;

    @Test
    @DisplayName("캐시 이름 목록 조회")
    void shouldGetCacheNames() {
        // given
        when(cacheManagementService.getCacheNames())
                .thenReturn(List.of("products", "product", "categories"));

        // when
        Collection<String> result = controller.getCacheNames();

        // then
        assertThat(result).containsExactly("products", "product", "categories");
    }

    @Test
    @DisplayName("전체 캐시 통계 조회")
    void shouldGetAllCacheStats() {
        // given
        when(cacheManagementService.getCacheNames())
                .thenReturn(List.of("products", "product"));
        when(cacheManagementService.getCacheStats("products"))
                .thenReturn(new CacheManagementService.CacheStats("products", true, 10));
        when(cacheManagementService.getCacheStats("product"))
                .thenReturn(new CacheManagementService.CacheStats("product", true, 5));

        // when
        var result = controller.getAllCacheStats();

        // then
        assertThat(result).hasSize(2);
        assertThat(result.get(0).cacheName()).isEqualTo("products");
        assertThat(result.get(0).estimatedSize()).isEqualTo(10);
    }

    @Test
    @DisplayName("특정 캐시 통계 조회")
    void shouldGetSpecificCacheStats() {
        // given
        when(cacheManagementService.getCacheStats("products"))
                .thenReturn(new CacheManagementService.CacheStats("products", true, 15));

        // when
        ResponseEntity<CacheManagementService.CacheStats> result = controller.getCacheStats("products");

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.OK);
        assertThat(result.getBody().cacheName()).isEqualTo("products");
        assertThat(result.getBody().estimatedSize()).isEqualTo(15);
    }

    @Test
    @DisplayName("존재하지 않는 캐시 404")
    void shouldReturn404ForNonExistentCache() {
        // given
        when(cacheManagementService.getCacheStats("unknown"))
                .thenReturn(new CacheManagementService.CacheStats("unknown", false, 0));

        // when
        ResponseEntity<CacheManagementService.CacheStats> result = controller.getCacheStats("unknown");

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NOT_FOUND);
    }

    @Test
    @DisplayName("특정 캐시 무효화")
    void shouldEvictSpecificCache() {
        // when
        ResponseEntity<Void> result = controller.evictCache("products");

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
        verify(cacheManagementService).evictCache("products");
    }

    @Test
    @DisplayName("특정 캐시 키 무효화")
    void shouldEvictSpecificCacheKey() {
        // when
        ResponseEntity<Void> result = controller.evictCacheKey("products", "item-123");

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
        verify(cacheManagementService).evictCacheKey("products", "item-123");
    }

    @Test
    @DisplayName("전체 캐시 무효화")
    void shouldEvictAllCaches() {
        // when
        ResponseEntity<Void> result = controller.evictAllCaches();

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
        verify(cacheManagementService).evictAllCaches();
    }
}
