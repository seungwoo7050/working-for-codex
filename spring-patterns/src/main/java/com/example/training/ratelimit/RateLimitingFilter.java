package com.example.training.ratelimit;

import jakarta.servlet.*;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.io.IOException;

/**
 * Rate Limiting 필터
 * 모든 API 요청에 대해 속도 제한 적용
 */
@Component
@Order(1)  // 가장 먼저 실행
public class RateLimitingFilter implements Filter {

    private final RateLimiter rateLimiter;
    private final ClientKeyResolver keyResolver;

    public RateLimitingFilter(RateLimiter rateLimiter, ClientKeyResolver keyResolver) {
        this.rateLimiter = rateLimiter;
        this.keyResolver = keyResolver;
    }

    @Override
    public void doFilter(ServletRequest request, ServletResponse response,
                         FilterChain chain) throws IOException, ServletException {
        HttpServletRequest httpRequest = (HttpServletRequest) request;
        HttpServletResponse httpResponse = (HttpServletResponse) response;

        String path = httpRequest.getRequestURI();

        // Rate Limiting 제외 경로
        if (isExcludedPath(path)) {
            chain.doFilter(request, response);
            return;
        }

        // 클라이언트 키 추출
        String clientKey = keyResolver.resolve(httpRequest);

        // Rate Limit 확인
        RateLimitResult result = rateLimiter.tryConsume(clientKey);

        // 헤더 추가
        addRateLimitHeaders(httpResponse, result);

        if (result.isAllowed()) {
            chain.doFilter(request, response);
        } else {
            // 429 Too Many Requests
            httpResponse.setStatus(429);
            httpResponse.setContentType("application/json");
            httpResponse.setHeader("Retry-After", String.valueOf(result.getRetryAfterSeconds()));
            httpResponse.getWriter().write(String.format(
                    "{\"error\":\"Too Many Requests\"," +
                    "\"message\":\"Rate limit exceeded. Please retry after %d seconds.\"," +
                    "\"retryAfter\":%d}",
                    result.getRetryAfterSeconds(),
                    result.getRetryAfterSeconds()
            ));
        }
    }

    /**
     * Rate Limit 헤더 추가
     */
    private void addRateLimitHeaders(HttpServletResponse response, RateLimitResult result) {
        response.setHeader("X-RateLimit-Limit", String.valueOf(result.getLimit()));
        response.setHeader("X-RateLimit-Remaining", String.valueOf(result.getRemaining()));
        response.setHeader("X-RateLimit-Reset", String.valueOf(result.getResetEpochSeconds()));
    }

    /**
     * Rate Limiting 제외 경로 확인
     */
    private boolean isExcludedPath(String path) {
        return path.startsWith("/actuator") ||
               path.equals("/api/health") ||
               path.startsWith("/h2-console");
    }
}
