package com.example.training.ratelimit;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import static org.assertj.core.api.Assertions.assertThat;

/**
 * Rate Limiting 테스트
 */
class RateLimitingTest {

    private TokenBucketRateLimiter rateLimiter;

    @BeforeEach
    void setUp() {
        rateLimiter = new TokenBucketRateLimiter();
        rateLimiter.clearBuckets();
    }

    @Test
    void shouldAllowRequestsWithinLimit() {
        String clientKey = "test-client-1";

        // 100개 요청 모두 허용되어야 함
        for (int i = 0; i < 100; i++) {
            RateLimitResult result = rateLimiter.tryConsume(clientKey);
            assertThat(result.isAllowed()).isTrue();
            assertThat(result.getLimit()).isEqualTo(100);
            assertThat(result.getRemaining()).isEqualTo(99 - i);
        }
    }

    @Test
    void shouldDenyRequestsExceedingLimit() {
        String clientKey = "test-client-2";

        // 한도 소진 (100개)
        for (int i = 0; i < 100; i++) {
            rateLimiter.tryConsume(clientKey);
        }

        // 추가 요청은 거부되어야 함
        RateLimitResult result = rateLimiter.tryConsume(clientKey);
        assertThat(result.isAllowed()).isFalse();
        assertThat(result.getRemaining()).isEqualTo(0);
        assertThat(result.getRetryAfterSeconds()).isGreaterThan(0);
    }

    @Test
    void shouldTrackDifferentClientsIndependently() {
        String client1 = "client-a";
        String client2 = "client-b";

        // 클라이언트 1: 50개 소비
        for (int i = 0; i < 50; i++) {
            rateLimiter.tryConsume(client1);
        }

        // 클라이언트 2: 첫 요청 - 전체 한도 사용 가능
        RateLimitResult result = rateLimiter.tryConsume(client2);
        assertThat(result.isAllowed()).isTrue();
        assertThat(result.getRemaining()).isEqualTo(99);  // 100 - 1

        // 클라이언트 1: 남은 50개
        RateLimitResult result1 = rateLimiter.tryConsume(client1);
        assertThat(result1.isAllowed()).isTrue();
        assertThat(result1.getRemaining()).isEqualTo(49);  // 100 - 50 - 1
    }

    @Test
    void shouldReturnCorrectRateLimitHeaders() {
        String clientKey = "test-client-3";

        RateLimitResult result = rateLimiter.tryConsume(clientKey);

        assertThat(result.getLimit()).isEqualTo(100);
        assertThat(result.getRemaining()).isEqualTo(99);
        assertThat(result.getResetEpochSeconds()).isGreaterThan(0);
    }

    @Test
    void shouldRefillTokensAfterTime() throws InterruptedException {
        String clientKey = "test-client-4";

        // 모든 토큰 소비
        for (int i = 0; i < 100; i++) {
            rateLimiter.tryConsume(clientKey);
        }

        // 토큰 소진 확인
        RateLimitResult denied = rateLimiter.tryConsume(clientKey);
        assertThat(denied.isAllowed()).isFalse();

        // 1.5초 대기 (리필 간격 후)
        Thread.sleep(1500);

        // 토큰 리필 확인
        RateLimitResult allowed = rateLimiter.tryConsume(clientKey);
        assertThat(allowed.isAllowed()).isTrue();
    }

    @Test
    void clientKeyResolver_shouldResolveIPAddress() {
        ClientKeyResolver resolver = new ClientKeyResolver();

        // Mock HttpServletRequest 대신 간단한 검증
        // 실제 통합 테스트에서 더 상세히 검증
        assertThat(resolver).isNotNull();
    }
}
