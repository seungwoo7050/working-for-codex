package com.example.training.common.security;

import io.jsonwebtoken.Claims;
import io.jsonwebtoken.Jwts;
import io.jsonwebtoken.security.Keys;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import javax.crypto.SecretKey;
import java.nio.charset.StandardCharsets;
import java.util.Date;

// [FILE]
// - 목적: JWT(JSON Web Token) 생성, 파싱, 검증 유틸리티
// - 주요 역할: 로그인 시 토큰 발급, 요청 시 토큰 검증
// - 관련 클론 가이드 단계: [CG-v1.1.0] Team & RBAC
// - 권장 읽는 순서: Order 1(생성자) → Order 2(토큰 생성) → Order 3(토큰 파싱) → Order 4(검증)
//
// [LEARN] C 개발자를 위한 설명:
// JWT는 "서명된 JSON 데이터"다. 구조: Header.Payload.Signature
// - Header: 알고리즘 정보 (예: {"alg": "HS256"})
// - Payload: 실제 데이터 (예: {"userId": 123, "exp": 1234567890})
// - Signature: HMAC(header + payload, secret_key)
//
// C에서 직접 구현한다면:
// 1. JSON 생성 → Base64 인코딩
// 2. HMAC-SHA256으로 서명 생성
// 3. header.payload.signature 형태로 결합
//
// 검증 시:
// 1. header.payload 분리
// 2. 같은 secret으로 서명 재생성
// 3. 원본 서명과 비교 → 일치하면 유효, 아니면 위조됨

@Component
public class JwtUtil {

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 생성자 - 설정값 주입
    // ═══════════════════════════════════════════════════════════════════════

    private final SecretKey key;
    private final long expiration;

    public JwtUtil(
            @Value("${jwt.secret}") String secret,
            // [LEARN] @Value: application.yml의 설정값을 주입
            // jwt:
            //   secret: "your-256-bit-secret-key-here..."
            //
            // C에서 getenv("JWT_SECRET") 또는 config 파일 파싱하던 것

            @Value("${jwt.expiration}") long expiration
    ) {
        // [LEARN] HMAC 키 생성: 비밀 문자열 → SecretKey 객체
        // HS256은 최소 256비트(32바이트) 키 필요
        this.key = Keys.hmacShaKeyFor(secret.getBytes(StandardCharsets.UTF_8));
        this.expiration = expiration;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] JWT 토큰 생성
    // ═══════════════════════════════════════════════════════════════════════

    public String generateToken(Long userId, String email) {
        // [LEARN] 현재 시간과 만료 시간 계산
        Date now = new Date();
        Date expiryDate = new Date(now.getTime() + expiration);

        return Jwts.builder()
                .subject(String.valueOf(userId))
                // [LEARN] subject: 토큰의 주체 (보통 사용자 식별자)
                // JWT 표준 클레임 중 하나 (sub)

                .claim("email", email)
                // [LEARN] claim: 커스텀 데이터 추가
                // 토큰에 원하는 정보를 넣을 수 있음

                .issuedAt(now)
                // [LEARN] issuedAt: 토큰 발급 시간 (iat)

                .expiration(expiryDate)
                // [LEARN] expiration: 토큰 만료 시간 (exp)
                // 만료된 토큰은 validateToken()에서 false 반환

                .signWith(key)
                // [LEARN] 서명: HMAC-SHA256(header + payload, key)
                // 이 서명으로 토큰 위조 여부를 검증

                .compact();
                // [LEARN] 최종 JWT 문자열 생성
                // "eyJhbGciOiJIUzI1NiJ9.eyJzdWIiOiIxMjM0...".형태
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] JWT 토큰 파싱
    // ═══════════════════════════════════════════════════════════════════════

    public Claims parseToken(String token) {
        // [LEARN] 토큰 파싱 및 서명 검증
        // 1. Base64 디코딩
        // 2. 서명 재계산 및 비교 (verifyWith)
        // 3. 유효하면 Claims(페이로드) 반환, 아니면 예외 발생
        return Jwts.parser()
                .verifyWith(key)
                .build()
                .parseSignedClaims(token)
                .getPayload();
    }

    public Long getUserIdFromToken(String token) {
        // [LEARN] 토큰에서 userId 추출
        // Claims의 subject를 Long으로 변환
        Claims claims = parseToken(token);
        return Long.parseLong(claims.getSubject());
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 4] 토큰 유효성 검증
    // ═══════════════════════════════════════════════════════════════════════

    public boolean validateToken(String token) {
        // [LEARN] 토큰 검증:
        // - 서명 일치 여부
        // - 만료 시간 초과 여부
        // - 토큰 형식 올바른지
        //
        // 실패 시 예외 발생 → catch에서 false 반환
        try {
            parseToken(token);
            return true;
        } catch (Exception e) {
            // [LEARN] 발생 가능한 예외:
            // - SignatureException: 서명 불일치 (위조됨)
            // - ExpiredJwtException: 만료됨
            // - MalformedJwtException: 형식 오류
            return false;
        }
    }
}
