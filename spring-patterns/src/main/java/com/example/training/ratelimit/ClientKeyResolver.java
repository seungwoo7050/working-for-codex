package com.example.training.ratelimit;

import jakarta.servlet.http.HttpServletRequest;
import org.springframework.stereotype.Component;

/**
 * 클라이언트 키 리졸버
 * 클라이언트를 식별하는 키 생성
 */
@Component
public class ClientKeyResolver {

    private static final String X_FORWARDED_FOR = "X-Forwarded-For";
    private static final String AUTHORIZATION = "Authorization";

    /**
     * 클라이언트 식별 키 추출
     * 우선순위: 1. 인증 토큰 2. X-Forwarded-For 3. RemoteAddr
     *
     * @param request HTTP 요청
     * @return 클라이언트 식별 키
     */
    public String resolve(HttpServletRequest request) {
        // 1. 인증된 사용자인 경우 토큰 기반 식별
        String authorization = request.getHeader(AUTHORIZATION);
        if (authorization != null && authorization.startsWith("Bearer ")) {
            // 토큰의 일부만 사용 (보안)
            String token = authorization.substring(7);
            return "user:" + token.substring(0, Math.min(token.length(), 32));
        }

        // 2. 프록시를 통한 요청인 경우 원본 IP
        String xForwardedFor = request.getHeader(X_FORWARDED_FOR);
        if (xForwardedFor != null && !xForwardedFor.isEmpty()) {
            // 첫 번째 IP가 원본 클라이언트
            return "ip:" + xForwardedFor.split(",")[0].trim();
        }

        // 3. 직접 연결된 클라이언트 IP
        return "ip:" + request.getRemoteAddr();
    }
}
