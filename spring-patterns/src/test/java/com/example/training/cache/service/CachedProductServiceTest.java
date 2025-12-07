package com.example.training.cache.service;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.cache.CacheManager;
import org.springframework.test.context.ActiveProfiles;

import java.math.BigDecimal;
import java.util.Optional;

import static org.assertj.core.api.Assertions.assertThat;

/**
 * CachedProductService 테스트
 * 캐싱 동작 및 무효화 검증
 */
@SpringBootTest
@ActiveProfiles("test")
class CachedProductServiceTest {

    @Autowired
    private CachedProductService productService;

    @Autowired
    private CacheManager cacheManager;

    @BeforeEach
    void setUp() {
        // 테스트 전 캐시 초기화
        productService.clearAllCaches();
    }

    @Test
    @DisplayName("상품 조회 시 캐시에 저장되어야 한다")
    void shouldCacheProductById() {
        // given
        var product = productService.createProduct("Cached Product", new BigDecimal("150.00"));
        var id = product.id();

        // when - 첫 번째 조회 (캐시 미스)
        long startTime1 = System.currentTimeMillis();
        Optional<CachedProductService.ProductDTO> result1 = productService.findById(id);
        long duration1 = System.currentTimeMillis() - startTime1;

        // when - 두 번째 조회 (캐시 히트)
        long startTime2 = System.currentTimeMillis();
        Optional<CachedProductService.ProductDTO> result2 = productService.findById(id);
        long duration2 = System.currentTimeMillis() - startTime2;

        // then
        assertThat(result1).isPresent();
        assertThat(result2).isPresent();
        assertThat(result1.get().name()).isEqualTo("Cached Product");
        
        // 캐시 히트가 더 빨라야 함 (100ms 지연이 있으므로)
        assertThat(duration2).isLessThan(duration1);
    }

    @Test
    @DisplayName("상품 목록 조회 시 캐시에 저장되어야 한다")
    void shouldCacheAllProducts() {
        // given
        productService.createProduct("Product A", new BigDecimal("100.00"));

        // when - 첫 번째 조회
        long startTime1 = System.currentTimeMillis();
        var result1 = productService.findAll();
        long duration1 = System.currentTimeMillis() - startTime1;

        // when - 두 번째 조회 (캐시 히트)
        long startTime2 = System.currentTimeMillis();
        var result2 = productService.findAll();
        long duration2 = System.currentTimeMillis() - startTime2;

        // then
        assertThat(result1).isNotEmpty();
        assertThat(result2).isNotEmpty();
        assertThat(duration2).isLessThan(duration1);
    }

    @Test
    @DisplayName("검색 결과가 캐시되어야 한다")
    void shouldCacheSearchResults() {
        // given
        productService.createProduct("Searchable Item", new BigDecimal("200.00"));

        // when - 첫 번째 검색
        long startTime1 = System.currentTimeMillis();
        var result1 = productService.search("Searchable");
        long duration1 = System.currentTimeMillis() - startTime1;

        // when - 두 번째 검색 (동일 키워드)
        long startTime2 = System.currentTimeMillis();
        var result2 = productService.search("Searchable");
        long duration2 = System.currentTimeMillis() - startTime2;

        // then
        assertThat(result1).hasSize(1);
        assertThat(result2).hasSize(1);
        assertThat(duration2).isLessThan(duration1);
    }

    @Test
    @DisplayName("상품 생성 시 products 캐시가 무효화되어야 한다")
    void shouldEvictCacheOnCreate() {
        // given - 캐시 웜업
        productService.findAll();

        // when - 새 상품 생성
        productService.createProduct("New Product", new BigDecimal("300.00"));

        // then - 캐시가 무효화되어 새 데이터 반영
        var result = productService.findAll();
        assertThat(result.stream().anyMatch(p -> p.name().equals("New Product"))).isTrue();
    }

    @Test
    @DisplayName("상품 수정 시 관련 캐시가 무효화되어야 한다")
    void shouldEvictCacheOnUpdate() {
        // given
        var product = productService.createProduct("Original Name", new BigDecimal("100.00"));
        productService.findById(product.id()); // 캐시 웜업

        // when
        productService.updateProduct(product.id(), "Updated Name", new BigDecimal("150.00"));

        // then - 업데이트된 데이터가 반영되어야 함
        var result = productService.findById(product.id());
        assertThat(result).isPresent();
        assertThat(result.get().name()).isEqualTo("Updated Name");
        assertThat(result.get().price()).isEqualByComparingTo(new BigDecimal("150.00"));
    }

    @Test
    @DisplayName("상품 삭제 시 관련 캐시가 무효화되어야 한다")
    void shouldEvictCacheOnDelete() {
        // given
        var product = productService.createProduct("To Delete", new BigDecimal("100.00"));
        productService.findById(product.id()); // 캐시 웜업

        // when
        boolean deleted = productService.deleteProduct(product.id());

        // then
        assertThat(deleted).isTrue();
        var result = productService.findById(product.id());
        assertThat(result).isEmpty();
    }

    @Test
    @DisplayName("전체 캐시 무효화가 동작해야 한다")
    void shouldClearAllCaches() {
        // given - 캐시 웜업
        productService.findAll();
        productService.search("Sample");

        // when
        productService.clearAllCaches();

        // then - 캐시가 비워졌으므로 다시 느린 조회
        long startTime = System.currentTimeMillis();
        productService.findAll();
        long duration = System.currentTimeMillis() - startTime;

        assertThat(duration).isGreaterThanOrEqualTo(100L); // 느린 조회 확인
    }
}
