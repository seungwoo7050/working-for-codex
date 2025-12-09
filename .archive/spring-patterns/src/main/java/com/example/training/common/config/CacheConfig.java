package com.example.training.common.config;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.cache.CacheManager;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.cache.concurrent.ConcurrentMapCacheManager;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.data.redis.cache.RedisCacheConfiguration;
import org.springframework.data.redis.cache.RedisCacheManager;
import org.springframework.data.redis.connection.RedisConnectionFactory;
import org.springframework.data.redis.serializer.GenericJackson2JsonRedisSerializer;
import org.springframework.data.redis.serializer.RedisSerializationContext;
import org.springframework.data.redis.serializer.StringRedisSerializer;

import java.time.Duration;

// [FILE]
// - 목적: 애플리케이션 캐시 매니저 설정
// - 주요 역할: Redis 또는 인메모리 캐시 선택적 활성화
// - 관련 클론 가이드 단계: [CG-v1.2.0] 1.2.4 캐시 설정, [CG-v2.3.0] 캐싱 & 압축
// - 권장 읽는 순서: Order 1(Redis) → Order 2(InMemory)
//
// [LEARN] C 개발자를 위한 설명:
// 캐시(Cache)는 "자주 조회하는 데이터를 메모리에 저장"하여 성능을 높이는 기법이다.
//
// C에서 간단한 캐시 구현:
// typedef struct { char* key; void* value; time_t expires; } CacheEntry;
// CacheEntry cache[1000];
//
// void* get_cached(char* key) {
//     for (int i = 0; i < 1000; i++) {
//         if (strcmp(cache[i].key, key) == 0) {
//             if (time(NULL) < cache[i].expires) return cache[i].value;  // 히트
//             else return NULL;  // 만료
//         }
//     }
//     return NULL;  // 미스
// }
//
// Spring Cache는 이런 로직을 @Cacheable 어노테이션으로 자동 처리한다.
// 이 설정 클래스는 "어디에 캐시할지" (Redis vs 메모리)를 결정한다.

@Configuration
// [LEARN] @Configuration: 이 클래스가 Bean 정의를 포함함을 선언
// C의 config.h처럼 설정을 모아두는 역할

@EnableCaching
// [LEARN] @EnableCaching: 캐시 기능 활성화
// 이 어노테이션 없으면 @Cacheable 어노테이션이 동작하지 않음
public class CacheConfig {

    private static final Logger logger = LoggerFactory.getLogger(CacheConfig.class);

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] Redis Cache Manager (프로덕션용)
    // ═══════════════════════════════════════════════════════════════════════

    @Bean
    @Primary
    // [LEARN] @Primary: 동일 타입 Bean이 여러 개일 때 기본값으로 사용
    @ConditionalOnProperty(name = "spring.cache.type", havingValue = "redis")
    // [LEARN] @ConditionalOnProperty: 설정값에 따라 Bean 생성 여부 결정
    // spring.cache.type=redis 일 때만 이 Bean 활성화
    public CacheManager redisCacheManager(RedisConnectionFactory connectionFactory) {
        logger.info("Configuring Redis Cache Manager");

        RedisCacheConfiguration cacheConfig = RedisCacheConfiguration.defaultCacheConfig()
                .entryTtl(Duration.ofMinutes(5))
                // [LEARN] TTL (Time To Live): 캐시 만료 시간
                // 5분 후 자동 삭제 → 오래된 데이터 방지
                .serializeKeysWith(
                        RedisSerializationContext.SerializationPair.fromSerializer(new StringRedisSerializer())
                )
                // [LEARN] 키 직렬화: 문자열로 저장 (가독성)
                .serializeValuesWith(
                        RedisSerializationContext.SerializationPair.fromSerializer(
                                new GenericJackson2JsonRedisSerializer()
                        )
                );
                // [LEARN] 값 직렬화: JSON으로 저장 (디버깅 용이)

        return RedisCacheManager.builder(connectionFactory)
                .cacheDefaults(cacheConfig)
                .build();
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] In-Memory Cache Manager (개발/테스트용)
    // ═══════════════════════════════════════════════════════════════════════

    @Bean
    @ConditionalOnProperty(name = "spring.cache.type", havingValue = "simple", matchIfMissing = true)
    // [LEARN] matchIfMissing = true: 설정이 없으면 이 Bean 사용 (기본값)
    public CacheManager simpleCacheManager() {
        logger.warn("Redis not configured. Using in-memory cache (ConcurrentMapCacheManager)");
        return new ConcurrentMapCacheManager("popularIssues", "stats", "external");
        // [LEARN] ConcurrentMapCacheManager: JVM 힙 메모리 사용
        // - 장점: 설정 간단, 외부 의존성 없음
        // - 단점: 서버 재시작 시 캐시 사라짐, 분산 환경에서 서버간 공유 안 됨
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: CacheManager, Redis 분산 캐시
//
// 캐시 전략 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 인메모리 캐시 vs 분산 캐시:
//    [인메모리] 서버A ─ cache[]     서버B ─ cache[]  (별도 캐시)
//    [분산]    서버A ─┐                             
//                    └─── Redis ───┘               서버B (공유 캐시)
//
// 2. TTL의 중요성:
//    - 캐시 없으면: DB 직접 조회 (느림)
//    - 캐시만 있으면: 데이터 갱신 안 됨 (오래된 데이터)
//    - TTL 적용: 일정 시간 후 자동 갱신
//
// 3. 캐시 무효화:
//    - 데이터 수정 시 @CacheEvict로 해당 캐시 삭제
//    - 다음 조회 시 DB에서 새로 읽어 캐시
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. cache/service/CachedProductService.java - @Cacheable 사용 예시
// 2. issue/service/IssueService.java - 실제 캐시 적용
// ═══════════════════════════════════════════════════════════════════════════════
