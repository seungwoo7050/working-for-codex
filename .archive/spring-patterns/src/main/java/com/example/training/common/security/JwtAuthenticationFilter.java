package com.example.training.common.security;

import jakarta.servlet.FilterChain;
import jakarta.servlet.ServletException;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.web.authentication.WebAuthenticationDetailsSource;
import org.springframework.stereotype.Component;
import org.springframework.util.StringUtils;
import org.springframework.web.filter.OncePerRequestFilter;

import java.io.IOException;
import java.util.Collections;

// [FILE]
// - 목적: HTTP 요청에서 JWT 토큰을 추출하고 검증하는 필터
// - 주요 역할: Authorization 헤더 파싱 → 토큰 검증 → SecurityContext에 인증 정보 저장
// - 관련 클론 가이드 단계: [CG-v1.1.0] Team & RBAC
// - 권장 읽는 순서: Order 1(doFilterInternal) → Order 2(extractJwtFromRequest)
//
// [LEARN] C 개발자를 위한 설명:
// 이 클래스는 "HTTP 요청 전처리 함수"다.
// 모든 HTTP 요청이 Controller에 도달하기 전에 이 필터를 거친다.
//
// C에서 HTTP 서버의 미들웨어를 구현할 때:
// void process_request(Request* req) {
//     if (!validate_jwt(req->headers["Authorization"], &user_id)) {
//         // 토큰 없거나 유효하지 않으면 그냥 통과 (permitAll 경로용)
//     } else {
//         req->context.user_id = user_id;  // 컨텍스트에 사용자 정보 저장
//     }
//     next_handler(req);  // 다음 핸들러로 전달
// }
//
// OncePerRequestFilter를 상속하면 요청당 한 번만 실행됨이 보장됨

@Component
// [LEARN] @Component: Spring Bean으로 등록
// SecurityConfig에서 이 Bean을 주입받아 필터 체인에 등록
public class JwtAuthenticationFilter extends OncePerRequestFilter {

    private final JwtUtil jwtUtil;

    public JwtAuthenticationFilter(JwtUtil jwtUtil) {
        this.jwtUtil = jwtUtil;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 필터 메인 로직
    // ═══════════════════════════════════════════════════════════════════════

    @Override
    protected void doFilterInternal(
            HttpServletRequest request,
            HttpServletResponse response,
            FilterChain filterChain
    ) throws ServletException, IOException {
        // [LEARN] 필터 체인 패턴:
        // 1. 전처리 수행 (JWT 검증)
        // 2. filterChain.doFilter() 호출 → 다음 필터 또는 Controller로 전달
        // 3. 후처리 수행 (여기서는 없음)
        //
        // C에서 링크드 리스트로 미들웨어 체인을 구현할 때:
        // current->process(req); current->next->process(req); ...

        try {
            String jwt = extractJwtFromRequest(request);

            if (jwt != null && jwtUtil.validateToken(jwt)) {
                // [LEARN] JWT 검증 성공: 토큰에서 사용자 ID 추출
                Long userId = jwtUtil.getUserIdFromToken(jwt);

                // [LEARN] UsernamePasswordAuthenticationToken: 인증 정보 객체
                // - principal: 주체 (여기서는 userId)
                // - credentials: 자격증명 (JWT 방식에서는 null)
                // - authorities: 권한 목록 (RBAC에서 사용)
                UsernamePasswordAuthenticationToken authentication =
                        new UsernamePasswordAuthenticationToken(userId, null, Collections.emptyList());
                authentication.setDetails(new WebAuthenticationDetailsSource().buildDetails(request));

                // [LEARN] SecurityContextHolder: 스레드 로컬에 인증 정보 저장
                // 이후 Controller에서 Authentication 파라미터로 접근 가능
                // C의 pthread_setspecific()과 유사한 스레드별 저장소
                SecurityContextHolder.getContext().setAuthentication(authentication);
            }
        } catch (Exception ex) {
            // [LEARN] 인증 실패해도 요청은 계속 진행
            // permitAll 경로는 인증 없이 접근 가능하기 때문
            // 인증이 필요한 경로는 다음 필터에서 401 응답
            logger.error("Could not set user authentication in security context", ex);
        }

        // [LEARN] 다음 필터로 요청 전달
        // 이 호출이 없으면 요청이 여기서 멈추고 응답이 없음
        filterChain.doFilter(request, response);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] Authorization 헤더에서 JWT 추출
    // ═══════════════════════════════════════════════════════════════════════

    private String extractJwtFromRequest(HttpServletRequest request) {
        // [LEARN] Bearer 토큰 형식:
        // Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
        //
        // "Bearer " 접두사를 제거하고 토큰만 추출
        // C에서: if (strncmp(header, "Bearer ", 7) == 0) token = header + 7;

        String bearerToken = request.getHeader("Authorization");
        if (StringUtils.hasText(bearerToken) && bearerToken.startsWith("Bearer ")) {
            return bearerToken.substring(7);
        }
        return null;
    }
}
