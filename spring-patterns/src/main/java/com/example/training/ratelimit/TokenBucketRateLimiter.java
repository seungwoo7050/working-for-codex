package com.example.training.ratelimit;

import org.springframework.stereotype.Component;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;

// [FILE]
// - 목적: Token Bucket 알고리즘 기반 Rate Limiter 구현
// - 주요 역할: 클라이언트별 토큰 버킷 관리, 요청 허용/거부 판단
// - 관련 클론 가이드 단계: [CG-v2.2.0] Rate Limiting
// - 권장 읽는 순서: Order 1(tryConsume) → Order 2(refillTokens) → Order 3(Bucket 클래스)
//
// [LEARN] C 개발자를 위한 설명:
// Token Bucket 알고리즘은 네트워크 트래픽 제어에서 널리 사용되는 기법이다.
// 개념: 버킷에 토큰이 일정 속도로 채워지고, 요청마다 토큰을 소비한다.
//
// C에서의 구현:
// typedef struct {
//     int tokens;           // 현재 토큰 수
//     int max_tokens;       // 최대 토큰 (버킷 크기)
//     int refill_rate;      // 초당 채워지는 토큰
//     time_t last_refill;   // 마지막 리필 시간
// } TokenBucket;
//
// int try_consume(TokenBucket* b) {
//     refill_if_needed(b);
//     if (b->tokens > 0) { b->tokens--; return 1; }
//     return 0;
// }
//
// 버스트(Burst) 허용: 버킷이 가득 차면 순간적으로 많은 요청 처리 가능
// 평균 속도 제한: 장기적으로는 refill_rate 이하로 제한됨

@Component
public class TokenBucketRateLimiter implements RateLimiter {

    // [LEARN] ConcurrentHashMap: 스레드 안전한 해시맵
    // C의 pthread_mutex + hashmap 조합과 유사하지만,
    // 내부적으로 세분화된 락(segmented lock)을 사용해 더 효율적
    private final ConcurrentMap<String, Bucket> buckets = new ConcurrentHashMap<>();

    // ═══════════════════════════════════════════════════════════════════════
    // 설정 상수 (실무에서는 외부 설정으로 주입)
    // ═══════════════════════════════════════════════════════════════════════

    // [LEARN] MAX_TOKENS: 버킷 최대 용량
    // 버스트 허용량을 결정. 100이면 순간적으로 100개 요청 가능
    private static final int MAX_TOKENS = 100;

    // [LEARN] REFILL_RATE: 초당 리필되는 토큰 수
    // 10이면 초당 10개 요청이 평균 속도 제한
    private static final int REFILL_RATE = 10;

    // [LEARN] REFILL_INTERVAL_MS: 리필 계산 간격
    private static final long REFILL_INTERVAL_MS = 1000;

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 토큰 소비 시도
    // ═══════════════════════════════════════════════════════════════════════

    @Override
    public RateLimitResult tryConsume(String key) {
        // [LEARN] computeIfAbsent: 키가 없으면 새 버킷 생성, 있으면 기존 반환
        // 원자적(atomic) 연산으로 race condition 방지
        // C에서 pthread_mutex_lock() + hash_lookup() + 조건부 insert + unlock() 조합
        Bucket bucket = buckets.computeIfAbsent(key,
                k -> new Bucket(MAX_TOKENS, System.currentTimeMillis()));

        // [LEARN] 버킷 단위 동기화
        // 같은 클라이언트의 동시 요청을 직렬화
        // C: pthread_mutex_lock(&bucket->mutex);
        synchronized (bucket) {
            // 시간 경과에 따른 토큰 리필
            refillTokens(bucket);

            if (bucket.tokens > 0) {
                // [LEARN] 토큰 소비 성공
                bucket.tokens--;
                return RateLimitResult.allowed(
                        MAX_TOKENS,
                        bucket.tokens,
                        calculateResetTime(bucket)
                );
            } else {
                // [LEARN] 토큰 부족 → 거부
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

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 토큰 리필 로직
    // ═══════════════════════════════════════════════════════════════════════

    private void refillTokens(Bucket bucket) {
        // [LEARN] 시간 기반 토큰 리필
        // 마지막 리필 이후 경과 시간에 비례하여 토큰 추가
        long now = System.currentTimeMillis();
        long elapsed = now - bucket.lastRefillTime;

        if (elapsed >= REFILL_INTERVAL_MS) {
            // [LEARN] 경과 시간에 따른 토큰 계산
            // 예: 2.5초 경과, REFILL_RATE=10 → 25개 토큰 추가
            int tokensToAdd = (int) (elapsed / REFILL_INTERVAL_MS * REFILL_RATE);
            // MAX_TOKENS를 초과하지 않도록 제한
            bucket.tokens = Math.min(MAX_TOKENS, bucket.tokens + tokensToAdd);
            bucket.lastRefillTime = now;
        }
    }

    // 리셋 시간 계산 (Unix Epoch 초)
    private long calculateResetTime(Bucket bucket) {
        return (bucket.lastRefillTime + REFILL_INTERVAL_MS) / 1000;
    }

    // 재시도 대기 시간 계산 (초)
    private long calculateRetryAfter(Bucket bucket) {
        long elapsed = System.currentTimeMillis() - bucket.lastRefillTime;
        long remaining = REFILL_INTERVAL_MS - elapsed;
        return Math.max(1, remaining / 1000 + 1);
    }

    // 테스트용: 버킷 초기화
    public void clearBuckets() {
        buckets.clear();
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 버킷 내부 클래스
    // ═══════════════════════════════════════════════════════════════════════

    // [LEARN] 각 클라이언트별 상태를 저장하는 구조체
    // C: typedef struct { int tokens; time_t last_refill; } Bucket;
    private static class Bucket {
        int tokens;
        long lastRefillTime;

        Bucket(int tokens, long lastRefillTime) {
            this.tokens = tokens;
            this.lastRefillTime = lastRefillTime;
        }
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// Token Bucket 알고리즘 동작 원리:
// ───────────────────────────────────────────────────────────────────────────────
// 설정: MAX_TOKENS=100, REFILL_RATE=10/sec
//
// 시나리오 1 - 정상 사용:
// t=0: 버킷 생성, tokens=100
// t=1: 5개 요청 → tokens=95
// t=2: 3개 요청 → tokens=92 (1초 경과로 10개 리필 → 102 → 100 캡, 3개 소비 → 97)
//      ... 계속 10개/초 미만이면 안정적으로 유지
//
// 시나리오 2 - 버스트:
// t=0: 버킷 생성, tokens=100
// t=0.1: 100개 요청 한 번에 → tokens=0 (모두 성공)
// t=0.2: 1개 요청 → 거부 (tokens=0)
// t=1.1: 1개 요청 → 성공 (1초 경과로 10개 리필)
//
// 시나리오 3 - 지속적 과부하:
// t=0~10: 매초 20개 요청 시도
//   → 초당 10개만 성공, 나머지 10개는 429 응답
//   → 평균적으로 REFILL_RATE로 제한됨
//
// 분산 환경 한계:
// ───────────────────────────────────────────────────────────────────────────────
// 이 구현은 인메모리 ConcurrentHashMap 사용 → 단일 서버에서만 동작
// 여러 서버 배포 시:
// - Redis 기반 Rate Limiter 필요 (lua script로 원자적 연산)
// - Spring Cloud Gateway + Redis Rate Limiter 조합 권장
// ═══════════════════════════════════════════════════════════════════════════════
