package com.example.training.ratelimit;

import org.springframework.stereotype.Component;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

/**
 * Token Bucket Rate Limiter
 * 인메모리 구현 (단일 인스턴스용)
 */
@Component
public class TokenBucketRateLimiter implements RateLimiter {

    private final ConcurrentMap<String, Bucket> buckets = new ConcurrentHashMap<>();

    /**
     * 버킷 최대 용량 (버스트 허용량)
     */
    private static final int MAX_TOKENS = 100;

    /**
     * 초당 리필 토큰 수
     */
    private static final int REFILL_RATE = 10;

    /**
     * 리필 간격 (밀리초)
     */
    private static final long REFILL_INTERVAL_MS = 1000;

    @Override
    public RateLimitResult tryConsume(String key) {
        Bucket bucket = buckets.computeIfAbsent(key,
                k -> new Bucket(MAX_TOKENS, System.currentTimeMillis()));

        synchronized (bucket) {
            refillTokens(bucket);

            if (bucket.tokens > 0) {
                bucket.tokens--;
                return RateLimitResult.allowed(
                        MAX_TOKENS,
                        bucket.tokens,
                        calculateResetTime(bucket)
                );
            } else {
                long retryAfter = calculateRetryAfter(bucket);
                return RateLimitResult.denied(
                        MAX_TOKENS,
                        0,
                        calculateResetTime(bucket),
                        retryAfter
                );
            }
        }
    }

    /**
     * 토큰 리필
     */
    private void refillTokens(Bucket bucket) {
        long now = System.currentTimeMillis();
        long elapsed = now - bucket.lastRefillTime;

        if (elapsed >= REFILL_INTERVAL_MS) {
            int tokensToAdd = (int) (elapsed / REFILL_INTERVAL_MS * REFILL_RATE);
            bucket.tokens = Math.min(MAX_TOKENS, bucket.tokens + tokensToAdd);
            bucket.lastRefillTime = now;
        }
    }

    /**
     * 리셋 시간 계산 (Unix Epoch 초)
     */
    private long calculateResetTime(Bucket bucket) {
        return (bucket.lastRefillTime + REFILL_INTERVAL_MS) / 1000;
    }

    /**
     * 재시도 대기 시간 계산 (초)
     */
    private long calculateRetryAfter(Bucket bucket) {
        long elapsed = System.currentTimeMillis() - bucket.lastRefillTime;
        long remaining = REFILL_INTERVAL_MS - elapsed;
        return Math.max(1, remaining / 1000 + 1);
    }

    /**
     * 테스트용: 버킷 초기화
     */
    public void clearBuckets() {
        buckets.clear();
    }

    /**
     * 버킷 내부 클래스
     */
    private static class Bucket {
        int tokens;
        long lastRefillTime;

        Bucket(int tokens, long lastRefillTime) {
            this.tokens = tokens;
            this.lastRefillTime = lastRefillTime;
        }
    }
}
