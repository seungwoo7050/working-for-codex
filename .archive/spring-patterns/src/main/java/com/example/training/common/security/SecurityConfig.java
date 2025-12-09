package com.example.training.common.security;

import jakarta.servlet.http.HttpServletResponse;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;
import org.springframework.security.web.authentication.UsernamePasswordAuthenticationFilter;

// [FILE]
// - 목적: Spring Security 보안 설정 정의
// - 주요 역할: 인증/인가 규칙, JWT 필터 등록, 보안 정책 설정
// - 관련 클론 가이드 단계: [CG-v1.1.0] Team & RBAC
// - 권장 읽는 순서: Order 1(passwordEncoder) → Order 2(filterChain)
//
// [LEARN] C 개발자를 위한 설명:
// Spring Security는 "보안 미들웨어 체인"을 구성한다.
// HTTP 요청이 들어오면 여러 보안 필터를 순차적으로 통과한다:
// 1. 인증 필터: JWT 토큰 검증 → 사용자 정보 추출
// 2. 인가 필터: URL별 접근 권한 확인
// 3. 예외 처리: 인증 실패 시 401, 권한 없음 시 403 응답
//
// C에서 main() 앞에 항상 호출되는 전처리 함수들을 생각하면 됨:
// request → check_auth() → check_permission() → handler()
//
// @Configuration: 이 클래스가 Bean 정의를 포함함을 선언

@Configuration
@EnableWebSecurity
// [LEARN] @EnableWebSecurity: Spring Security 활성화
// 이 어노테이션이 없으면 보안 설정이 적용되지 않음
public class SecurityConfig {

    private final JwtAuthenticationFilter jwtAuthenticationFilter;

    public SecurityConfig(JwtAuthenticationFilter jwtAuthenticationFilter) {
        // [LEARN] 의존성 주입: JWT 인증 필터를 주입받음
        // 이 필터가 Security 체인에 등록되어 모든 요청을 가로챔
        this.jwtAuthenticationFilter = jwtAuthenticationFilter;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 비밀번호 암호화 Bean
    // ═══════════════════════════════════════════════════════════════════════

    @Bean
    // [LEARN] @Bean: 이 메서드의 반환값을 Spring Bean으로 등록
    // 다른 곳에서 PasswordEncoder를 주입받으면 이 인스턴스가 사용됨
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
        // [LEARN] BCrypt: 단방향 해시 + salt + cost factor
        // C에서 crypt() 함수를 사용하던 것과 유사하지만,
        // BCrypt는 자동으로 랜덤 salt를 생성하고 저장함
        //
        // 비밀번호 저장: hash = bcrypt(password)
        // 비밀번호 검증: bcrypt.matches(inputPassword, storedHash)
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 보안 필터 체인 설정
    // ═══════════════════════════════════════════════════════════════════════

    @Bean
    public SecurityFilterChain filterChain(HttpSecurity http) throws Exception {
        // [LEARN] HttpSecurity: 보안 설정을 빌더 패턴으로 구성
        // 메서드 체이닝으로 여러 설정을 연결

        http
                .csrf(csrf -> csrf.disable())
                // [LEARN] CSRF 비활성화: REST API는 stateless이므로 CSRF 토큰 불필요
                // 브라우저 기반 폼 제출이 아닌 API 요청에서는 JWT로 인증

                .sessionManagement(session -> session
                        .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
                )
                // [LEARN] STATELESS: 서버에 세션을 저장하지 않음
                // 모든 요청은 JWT 토큰으로 독립적으로 인증됨
                // C에서 세션 테이블 없이 매 요청마다 토큰 검증하는 방식

                .exceptionHandling(exception -> exception
                        .authenticationEntryPoint((request, response, authException) -> {
                            // [LEARN] 인증 실패 시 커스텀 응답
                            // 기본 동작(로그인 페이지 리다이렉트) 대신 JSON 에러 반환
                            response.setContentType("application/json");
                            response.setStatus(HttpServletResponse.SC_UNAUTHORIZED);
                            response.getWriter().write("{\"code\":\"UNAUTHORIZED\",\"message\":\"Authentication required\"}");
                        })
                )

                .authorizeHttpRequests(auth -> auth
                        // [LEARN] URL 패턴별 인가 규칙 정의
                        // permitAll(): 인증 없이 접근 가능
                        // authenticated(): 인증된 사용자만 접근 가능
                        .requestMatchers("/api/health", "/api/users", "/api/auth/login", "/h2-console/**").permitAll()
                        // [LEARN] 위 URL들은 로그인 없이 접근 가능
                        // - /api/health: 헬스체크 (Kubernetes liveness probe)
                        // - /api/users: 회원가입
                        // - /api/auth/login: 로그인
                        // - /h2-console: 개발용 DB 콘솔

                        .anyRequest().authenticated()
                        // [LEARN] 나머지 모든 요청은 인증 필요
                )

                .addFilterBefore(jwtAuthenticationFilter, UsernamePasswordAuthenticationFilter.class);
                // [LEARN] JWT 필터를 기본 인증 필터 앞에 추가
                // 요청 순서: JWT 필터 → (검증 성공 시) 나머지 필터 체인
                //
                // C에서 미들웨어 체인을 구성할 때:
                // handler_chain = { jwt_filter, auth_filter, route_handler };
                // 와 유사한 개념

        // H2 Console 접근을 위한 설정
        http.headers(headers -> headers.frameOptions(frame -> frame.disable()));
        // [LEARN] H2 Console은 iframe을 사용하므로 X-Frame-Options 비활성화
        // 프로덕션에서는 H2 Console을 사용하지 않으므로 이 설정도 제거

        return http.build();
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: Spring Security, 필터 체인, JWT 인증
//
// Spring Security 핵심 개념 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 필터 체인 (Filter Chain):
//    - HTTP 요청이 Controller에 도달하기 전에 통과하는 필터들
//    - C의 미들웨어 패턴: request → filter1 → filter2 → handler
//    - 각 필터는 요청을 통과시키거나, 거부(401/403)하거나, 수정할 수 있음
//
// 2. 인증 (Authentication) vs 인가 (Authorization):
//    - 인증: "너 누구야?" → JWT 토큰 → 사용자 정보 추출
//    - 인가: "이거 해도 돼?" → URL 패턴 + 권한 확인
//    - C에서 check_token() → check_permission() 순서로 호출하는 것과 동일
//
// 3. Stateless 인증:
//    - 서버에 세션 저장 안 함 → 스케일 아웃 용이
//    - 매 요청마다 토큰 검증 → CPU 비용 증가 (하지만 DB I/O 감소)
//    - C에서 세션 테이블 대신 JWT 서명 검증하는 방식
//
// 4. SecurityContext:
//    - 현재 요청의 인증 정보를 저장하는 스레드 로컬 저장소
//    - Controller에서 Authentication 객체로 접근
//    - C에서 pthread_setspecific()으로 스레드별 컨텍스트 저장하는 것과 유사
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. common/security/JwtAuthenticationFilter.java - JWT 토큰 검증 필터
// 2. common/security/JwtUtil.java - JWT 토큰 생성/파싱
// ═══════════════════════════════════════════════════════════════════════════════
