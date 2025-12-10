package com.example.training.config;

import org.springframework.cache.CacheManager;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.cache.concurrent.ConcurrentMapCacheManager;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] 캐시 설정 - Spring Cache Abstraction
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 인메모리 캐싱으로 DB 조회 성능 최적화
 * - 관련 클론 가이드 단계: [CG-v1.2.0] 1.2.1 캐시 설정
 * 
 * [LEARN] C 개발자를 위한 캐싱 개념:
 * 캐시 = 자주 조회하는 데이터를 메모리에 저장해두는 것
 * 
 * C 관점:
 *   static struct Product* cache[1000];  // 해시맵
 *   Product* get_product(int id) {
 *       if (cache[id] != NULL) return cache[id];  // 캐시 히트
 *       Product* p = db_query(id);                 // DB 조회
 *       cache[id] = p;                             // 캐시 저장
 *       return p;
 *   }
 * 
 * Spring Cache:
 *   @Cacheable("product")
 *   public Product getProduct(Long id) {
 *       return repository.findById(id);  // 첫 호출만 실행, 이후 캐시
 *   }
 * ═══════════════════════════════════════════════════════════════════════════════
 */

// ─────────────────────────────────────────────────────────────────────────────
// [Order 1] 캐시 설정 클래스
// ─────────────────────────────────────────────────────────────────────────────
@Configuration
@EnableCaching
// [LEARN] @EnableCaching: @Cacheable, @CacheEvict 등 캐시 어노테이션 활성화
// Application.java에도 있지만, 설정 클래스에 두면 캐시 관련 설정을 한 곳에서 관리
public class CachingConfig {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] CacheManager 빈 정의
    // ─────────────────────────────────────────────────────────────────────────
    /**
     * 캐시 매니저 빈
     * ConcurrentMapCacheManager: 인메모리 캐시 (개발/테스트용)
     * 운영 환경에서는 RedisCacheManager 사용 권장
     */
    @Bean
    @Primary
    // [LEARN] @Primary: 같은 타입의 빈이 여러 개일 때 우선 주입
    // RedisCacheManager도 있다면 이것이 기본으로 선택됨
    public CacheManager cacheManager() {
        return new ConcurrentMapCacheManager(
                "products",        // 상품 목록 (TTL: 5분)
                "product",         // 개별 상품 (TTL: 30분)
                "categories",      // 카테고리 (TTL: 1시간)
                "hot-items",       // 인기 상품 (TTL: 1분)
                "popularIssues"    // 기존 인기 이슈 캐시
        );
        // ─────────────────────────────────────────────────────────────────────
        // [LEARN] ConcurrentMapCacheManager:
        // - 내부적으로 ConcurrentHashMap 사용
        // - 장점: 설정 간단, 별도 인프라 불필요
        // - 단점:
        //   1) TTL(만료 시간) 미지원 → 수동 evict 필요
        //   2) 메모리 제한 없음 → OOM 위험
        //   3) 분산 환경 미지원 → 서버 간 캐시 불일치
        //
        // 프로덕션 권장: Redis, Caffeine 등 사용
        //   @Bean
        //   public CacheManager redisCacheManager(RedisConnectionFactory cf) {
        //       return RedisCacheManager.builder(cf)
        //           .cacheDefaults(RedisCacheConfiguration.defaultCacheConfig()
        //               .entryTtl(Duration.ofMinutes(10)))
        //           .build();
        //   }
        // ─────────────────────────────────────────────────────────────────────
    }
}
