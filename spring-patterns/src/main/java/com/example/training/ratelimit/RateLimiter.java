package com.example.training.ratelimit;

/**
 * Rate Limiter 인터페이스
 */
public interface RateLimiter {

    /**
     * 클라이언트 키에 대해 토큰 소비 시도
     *
     * @param key 클라이언트 식별 키
     * @return Rate Limit 결과
     */
    RateLimitResult tryConsume(String key);
}
