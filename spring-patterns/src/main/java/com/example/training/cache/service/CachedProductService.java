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
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] CachedProductService - Spring Cache 적용 예시
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: @Cacheable, @CacheEvict 어노테이션 사용법 학습
 * - 관련 클론 가이드 단계: [CG-v1.2.0] 1.2.1 캐시 적용
 * 
 * [LEARN] Spring Cache Abstraction:
 * 캐시 로직을 어노테이션으로 선언적으로 적용
 * 
 * 어노테이션 없이 (수동 캐시):
 *   public Product findById(Long id) {
 *       Product cached = cache.get("product:" + id);
 *       if (cached != null) return cached;        // 캐시 히트
 *       Product product = repository.findById(id); // DB 조회
 *       cache.put("product:" + id, product);       // 캐시 저장
 *       return product;
 *   }
 * 
 * @Cacheable 사용 (자동 캐시):
 *   @Cacheable(value = "product", key = "#id")
 *   public Product findById(Long id) {
 *       return repository.findById(id);  // 첫 호출만 실행
 *   }
 * 
 * C 관점: AOP(관점 지향 프로그래밍)로 메서드 앞뒤에 캐시 로직 자동 삽입
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Service
public class CachedProductService {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 인메모리 저장소 (데모용)
    // ─────────────────────────────────────────────────────────────────────────
    private final ConcurrentHashMap<Long, ProductDTO> products = new ConcurrentHashMap<>();
    // [LEARN] ConcurrentHashMap: 스레드 안전 해시맵
    // 실제 서비스에서는 JPA Repository 사용
    
    private final AtomicLong idGenerator = new AtomicLong(1);
    // [LEARN] AtomicLong: 스레드 안전 ID 생성기

    public CachedProductService() {
        // 초기 데이터
        createProduct("Sample Product 1", new BigDecimal("100.00"));
        createProduct("Sample Product 2", new BigDecimal("200.00"));
        createProduct("Sample Product 3", new BigDecimal("300.00"));
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] @Cacheable - 캐시 조회/저장
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * ID로 상품 조회 (캐시 적용)
     */
    @Cacheable(value = "product", key = "#id")
    public Optional<ProductDTO> findById(Long id) {
        simulateSlowQuery();
        return Optional.ofNullable(products.get(id));
    }
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] @Cacheable 동작:
    // 1. 캐시에서 key(#id)로 조회
    // 2. 캐시 히트 → 캐시된 값 반환 (메서드 실행 안 함)
    // 3. 캐시 미스 → 메서드 실행 → 결과를 캐시에 저장 → 반환
    //
    // 속성:
    // - value = "product": 캐시 이름 (CachingConfig에서 정의)
    // - key = "#id": 캐시 키 (SpEL 표현식)
    //   - #파라미터명 으로 메서드 파라미터 참조
    //   - #root.method.name: 메서드 이름
    //   - #result: 반환값 (unless 조건에서 사용)
    // ─────────────────────────────────────────────────────────────────────────

    /**
     * 전체 상품 목록 조회 (캐시 적용)
     */
    @Cacheable(value = "products", key = "'all'")
    public List<ProductDTO> findAll() {
        simulateSlowQuery();
        return new ArrayList<>(products.values());
    }
    // [LEARN] key = "'all'": 문자열 리터럴은 작은따옴표로 감싸야 함
    // 캐시 키: products::all

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
    // [LEARN] 동적 캐시 키: 'search:' + 검색어
    // search("phone") → 캐시 키: products::search:phone

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] @CacheEvict - 캐시 무효화
    // ─────────────────────────────────────────────────────────────────────────
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
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] @CacheEvict: 캐시 무효화
    // 
    // 왜 필요한가?
    // - 상품 생성 → 목록 조회 결과가 변경됨
    // - 기존 캐시가 오래된 데이터 → 무효화 필요
    //
    // allEntries = true:
    // - "products" 캐시의 모든 엔트리 삭제
    // - 개별 키 삭제: @CacheEvict(value = "products", key = "'all'")
    // ─────────────────────────────────────────────────────────────────────────

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
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] @Caching: 여러 캐시 어노테이션 조합
    // 
    // 상품 수정 시:
    // 1. 개별 상품 캐시 무효화: product::1
    // 2. 목록 캐시 전체 무효화: products::*
    //
    // 왜 둘 다 필요한가?
    // - findById(1) 캐시에 오래된 상품 정보
    // - findAll() 캐시에 오래된 목록 정보
    // ─────────────────────────────────────────────────────────────────────────

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
    // [LEARN] 여러 캐시 한 번에 무효화
    // 관리자 기능, 장애 복구 시 사용

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
    // [LEARN] 캐시 효과 테스트:
    // - 첫 호출: 100ms 지연 (캐시 미스)
    // - 두 번째 호출: 즉시 반환 (캐시 히트)

    /**
     * 상품 DTO
     */
    public record ProductDTO(Long id, String name, BigDecimal price) {}
    // [LEARN] Java Record (Java 16+):
    // - 불변 데이터 객체 간편 선언
    // - 자동 생성: 생성자, getter, equals, hashCode, toString
    // - C의 struct와 유사하지만 불변(immutable)
}
