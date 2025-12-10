package com.example.training.ratelimit;

import jakarta.servlet.*;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.io.IOException;

// [FILE]
// - 목적: API 요청 속도 제한 (Rate Limiting) 구현
// - 주요 역할: 클라이언트별 요청 빈도 제한, 과도한 요청 차단
// - 관련 클론 가이드 단계: [CG-v2.2.0] Rate Limiting
// - 권장 읽는 순서: Order 1(doFilter) → Order 2(헤더 추가) → Order 3(제외 경로)
//
// [LEARN] C 개발자를 위한 설명:
// Rate Limiting은 "단위 시간당 요청 수를 제한"하는 보안 기법이다.
// DDoS 공격 방어, 리소스 보호, 공정한 사용 보장 등의 목적으로 사용.
//
// C에서 구현한다면:
// typedef struct {
//     char* client_ip;
//     int token_count;      // 남은 토큰 수
//     time_t last_refill;   // 마지막 토큰 충전 시간
// } RateLimitBucket;
//
// int check_rate_limit(RateLimitBucket* bucket) {
//     refill_tokens_if_needed(bucket);  // 시간 경과에 따라 토큰 충전
//     if (bucket->token_count > 0) {
//         bucket->token_count--;
//         return ALLOWED;
//     }
//     return RATE_LIMITED;
// }
//
// 이 필터는 위 로직을 Filter 인터페이스로 구현하여
// 모든 HTTP 요청 전에 자동으로 실행되게 한다.

@Component
@Order(1)
// [LEARN] @Order(1): 필터 실행 순서 지정 (숫자가 낮을수록 먼저 실행)
// Rate Limiting은 최대한 빨리 실행하여 불필요한 처리를 막아야 함
// Security 필터보다도 먼저 실행하여 인증 전에 차단
public class RateLimitingFilter implements Filter {

    private final RateLimiter rateLimiter;
    private final ClientKeyResolver keyResolver;

    public RateLimitingFilter(RateLimiter rateLimiter, ClientKeyResolver keyResolver) {
        // [LEARN] 의존성 주입
        // - RateLimiter: 실제 토큰 버킷 알고리즘 구현
        // - ClientKeyResolver: 요청에서 클라이언트 식별자 추출 (IP, API Key 등)
        this.rateLimiter = rateLimiter;
        this.keyResolver = keyResolver;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 필터 메인 로직
    // ═══════════════════════════════════════════════════════════════════════

    @Override
    public void doFilter(ServletRequest request, ServletResponse response,
                         FilterChain chain) throws IOException, ServletException {
        // [LEARN] ServletRequest → HttpServletRequest 캐스팅
        // HTTP 관련 메서드(getHeader, getRequestURI 등)를 사용하기 위함
        HttpServletRequest httpRequest = (HttpServletRequest) request;
        HttpServletResponse httpResponse = (HttpServletResponse) response;

        String path = httpRequest.getRequestURI();

        // [LEARN] 특정 경로는 Rate Limiting에서 제외
        // 헬스체크, 모니터링 등은 항상 접근 가능해야 함
        if (isExcludedPath(path)) {
            chain.doFilter(request, response);
            return;
        }

        // [LEARN] 클라이언트 식별
        // IP 주소, API Key, 사용자 ID 등으로 클라이언트 구분
        // 같은 클라이언트의 요청만 함께 제한됨
        String clientKey = keyResolver.resolve(httpRequest);

        // [LEARN] 토큰 소비 시도
        // 토큰이 남아있으면 1개 소비하고 통과
        // 토큰이 없으면 거부
        RateLimitResult result = rateLimiter.tryConsume(clientKey);

        // [LEARN] 표준 Rate Limit 응답 헤더 추가
        // 클라이언트가 현재 상태를 알 수 있도록 정보 제공
        addRateLimitHeaders(httpResponse, result);

        if (result.isAllowed()) {
            // [LEARN] 허용: 다음 필터로 전달
            chain.doFilter(request, response);
        } else {
            // [LEARN] 거부: 429 Too Many Requests 응답
            // HTTP 429는 Rate Limiting 전용 상태 코드
            httpResponse.setStatus(429);
            httpResponse.setContentType("application/json");
            httpResponse.setHeader("Retry-After", String.valueOf(result.getRetryAfterSeconds()));
            // [LEARN] Retry-After 헤더: 클라이언트에게 재시도 타이밍 안내
            httpResponse.getWriter().write(String.format(
                    "{\"error\":\"Too Many Requests\"," +
                    "\"message\":\"Rate limit exceeded. Please retry after %d seconds.\"," +
                    "\"retryAfter\":%d}",
                    result.getRetryAfterSeconds(),
                    result.getRetryAfterSeconds()
            ));
        }
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] Rate Limit 응답 헤더
    // ═══════════════════════════════════════════════════════════════════════

    private void addRateLimitHeaders(HttpServletResponse response, RateLimitResult result) {
        // [LEARN] 표준 Rate Limit 헤더 (IETF draft 표준)
        // X-RateLimit-Limit: 윈도우당 최대 요청 수
        // X-RateLimit-Remaining: 남은 요청 수
        // X-RateLimit-Reset: 윈도우 리셋 시간 (Unix timestamp)
        //
        // 클라이언트는 이 헤더를 보고:
        // - 현재 얼마나 요청할 수 있는지 확인
        // - Rate Limit에 도달하기 전에 요청 속도 조절
        response.setHeader("X-RateLimit-Limit", String.valueOf(result.getLimit()));
        response.setHeader("X-RateLimit-Remaining", String.valueOf(result.getRemaining()));
        response.setHeader("X-RateLimit-Reset", String.valueOf(result.getResetEpochSeconds()));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] Rate Limiting 제외 경로
    // ═══════════════════════════════════════════════════════════════════════

    private boolean isExcludedPath(String path) {
        // [LEARN] 항상 접근 가능해야 하는 경로들
        // - /actuator: 모니터링/메트릭 엔드포인트
        // - /api/health: Kubernetes liveness/readiness probe
        // - /h2-console: 개발용 DB 콘솔
        return path.startsWith("/actuator") ||
               path.equals("/api/health") ||
               path.startsWith("/h2-console");
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Rate Limiting, Token Bucket 알고리즘
//
// Rate Limiting 알고리즘 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. Token Bucket 알고리즘:
//    - 버킷에 토큰이 일정 속도로 채워짐 (예: 초당 10개)
//    - 요청마다 토큰 1개 소비
//    - 토큰이 없으면 요청 거부
//    - 버스트 허용: 버킷이 가득 차면 순간적으로 많은 요청 가능
//
//    [C 구현 예시]
//    int refill_rate = 10;  // 초당 10개
//    int bucket_size = 100; // 최대 100개
//    
//    void refill(Bucket* b) {
//        int elapsed = now() - b->last_refill;
//        b->tokens = min(bucket_size, b->tokens + elapsed * refill_rate);
//        b->last_refill = now();
//    }
//
// 2. 다른 알고리즘:
//    - Fixed Window: 고정 시간 윈도우 (예: 1분당 100회)
//    - Sliding Window: 이동 시간 윈도우 (더 정확하지만 복잡)
//    - Leaky Bucket: 일정 속도로만 요청 처리 (버스트 불허)
//
// 3. 분산 환경 고려:
//    - 이 구현은 인메모리 → 단일 서버에서만 동작
//    - 여러 서버면 Redis 등 공유 저장소 필요
//    - Spring Cloud Gateway + Redis Rate Limiter 조합이 일반적
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. ratelimit/TokenBucketRateLimiter.java - 토큰 버킷 알고리즘 구현
// 2. reactive/service/ReactiveProductService.java - WebFlux 리액티브 패턴
// ═══════════════════════════════════════════════════════════════════════════════
