package com.example.training.cache.service;

import org.springframework.cache.annotation.CacheEvict;
import org.springframework.cache.annotation.Cacheable;
import org.springframework.cache.annotation.Caching;
import org.springframework.stereotype.Service;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 캐시 적용 상품 서비스
 * 데모용 인메모리 저장소와 캐싱 어노테이션 적용
 */
@Service
public class CachedProductService {

    private final ConcurrentHashMap<Long, ProductDTO> products = new ConcurrentHashMap<>();
    private final AtomicLong idGenerator = new AtomicLong(1);

    public CachedProductService() {
        // 초기 데이터
        createProduct("Sample Product 1", new BigDecimal("100.00"));
        createProduct("Sample Product 2", new BigDecimal("200.00"));
        createProduct("Sample Product 3", new BigDecimal("300.00"));
    }

    /**
     * ID로 상품 조회 (캐시 적용)
     */
    @Cacheable(value = "product", key = "#id")
    public Optional<ProductDTO> findById(Long id) {
        simulateSlowQuery();
        return Optional.ofNullable(products.get(id));
    }

    /**
     * 전체 상품 목록 조회 (캐시 적용)
     */
    @Cacheable(value = "products", key = "'all'")
    public List<ProductDTO> findAll() {
        simulateSlowQuery();
        return new ArrayList<>(products.values());
    }

    /**
     * 상품 검색 (캐시 적용)
     */
    @Cacheable(value = "products", key = "'search:' + #keyword")
    public List<ProductDTO> search(String keyword) {
        simulateSlowQuery();
        return products.values().stream()
                .filter(p -> p.name().toLowerCase().contains(keyword.toLowerCase()))
                .toList();
    }

    /**
     * 상품 생성 (관련 캐시 무효화)
     */
    @CacheEvict(value = "products", allEntries = true)
    public ProductDTO createProduct(String name, BigDecimal price) {
        Long id = idGenerator.getAndIncrement();
        ProductDTO product = new ProductDTO(id, name, price);
        products.put(id, product);
        return product;
    }

    /**
     * 상품 수정 (관련 캐시 무효화)
     */
    @Caching(evict = {
            @CacheEvict(value = "product", key = "#id"),
            @CacheEvict(value = "products", allEntries = true)
    })
    public Optional<ProductDTO> updateProduct(Long id, String name, BigDecimal price) {
        ProductDTO existing = products.get(id);
        if (existing == null) {
            return Optional.empty();
        }
        ProductDTO updated = new ProductDTO(id, name, price);
        products.put(id, updated);
        return Optional.of(updated);
    }

    /**
     * 상품 삭제 (관련 캐시 무효화)
     */
    @Caching(evict = {
            @CacheEvict(value = "product", key = "#id"),
            @CacheEvict(value = "products", allEntries = true)
    })
    public boolean deleteProduct(Long id) {
        return products.remove(id) != null;
    }

    /**
     * 전체 캐시 무효화
     */
    @CacheEvict(value = {"product", "products", "categories", "hot-items"}, allEntries = true)
    public void clearAllCaches() {
        // 캐시만 무효화, 데이터는 유지
    }

    /**
     * 느린 쿼리 시뮬레이션 (캐시 효과 확인용)
     */
    private void simulateSlowQuery() {
        try {
            Thread.sleep(100);  // 100ms 지연
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    /**
     * 상품 DTO
     */
    public record ProductDTO(Long id, String name, BigDecimal price) {}
}
