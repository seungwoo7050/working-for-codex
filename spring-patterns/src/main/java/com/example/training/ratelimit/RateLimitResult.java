package com.example.training.ratelimit;

/**
 * Rate Limit 결과
 */
public class RateLimitResult {

    private final boolean allowed;
    private final int limit;
    private final int remaining;
    private final long resetEpochSeconds;
    private final long retryAfterSeconds;

    private RateLimitResult(boolean allowed, int limit, int remaining,
                            long resetEpochSeconds, long retryAfterSeconds) {
        this.allowed = allowed;
        this.limit = limit;
        this.remaining = remaining;
        this.resetEpochSeconds = resetEpochSeconds;
        this.retryAfterSeconds = retryAfterSeconds;
    }

    /**
     * 요청 허용 결과 생성
     */
    public static RateLimitResult allowed(int limit, int remaining, long resetEpochSeconds) {
        return new RateLimitResult(true, limit, remaining, resetEpochSeconds, 0);
    }

    /**
     * 요청 거부 결과 생성
     */
    public static RateLimitResult denied(int limit, int remaining,
                                          long resetEpochSeconds, long retryAfterSeconds) {
        return new RateLimitResult(false, limit, remaining, resetEpochSeconds, retryAfterSeconds);
    }

    public boolean isAllowed() {
        return allowed;
    }

    public int getLimit() {
        return limit;
    }

    public int getRemaining() {
        return remaining;
    }

    public long getResetEpochSeconds() {
        return resetEpochSeconds;
    }

    public long getRetryAfterSeconds() {
        return retryAfterSeconds;
    }
}
