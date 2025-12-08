package com.example.gateway;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

// [FILE]
// - 목적: Spring Cloud Gateway 애플리케이션 진입점
// - 주요 역할: API Gateway 서버 부트스트랩
// - 관련 클론 가이드 단계: [CG-v1.6.0] API Gateway
// - 권장 읽는 순서: 이 파일 → application.yml → LoggingFilter
//
// [LEARN] C 개발자를 위한 설명:
// API Gateway는 "리버스 프록시 + 라우팅 + 공통 처리" 역할을 하는 서버다.
// 
// 일반적인 마이크로서비스 구조:
//   클라이언트 → [API Gateway] → 서비스 A
//                             → 서비스 B
//                             → 서비스 C
//
// API Gateway의 역할 (C 관점):
// 1. 라우팅: URL 패턴 → 백엔드 서비스 매핑 (nginx proxy_pass와 유사)
// 2. 필터: 인증, 로깅, Rate Limiting 등 공통 로직 (미들웨어)
// 3. 로드밸런싱: 여러 인스턴스에 요청 분배
//
// Spring Cloud Gateway 특징:
// - WebFlux 기반: 논블로킹 I/O로 높은 동시성 처리
// - 선언적 라우팅: application.yml에서 라우트 규칙 정의
// - 필터 체인: 요청/응답 전처리 커스터마이징

@SpringBootApplication
public class GatewayApplication {

    public static void main(String[] args) {
        SpringApplication.run(GatewayApplication.class, args);
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: API Gateway, 리버스 프록시
//
// API Gateway 패턴 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 단일 진입점:
//    - 클라이언트는 Gateway 주소만 알면 됨
//    - 백엔드 서비스 주소/포트 숨김 (보안)
//
// 2. 횡단 관심사 처리:
//    - 인증/인가: 모든 요청의 JWT 토큰 검증
//    - 로깅: 요청/응답 추적 (correlation ID)
//    - Rate Limiting: 과도한 요청 차단
//    - CORS: 브라우저 교차 출처 요청 처리
//
// 3. Spring Cloud Gateway vs nginx:
//    - nginx: 설정 파일 기반, C로 작성, 정적
//    - Gateway: Java 코드, Spring 생태계, 동적 라우팅 가능
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. src/main/resources/application.yml - 라우팅 규칙 정의
// 2. filter/LoggingFilter.java - 공통 로깅 필터
// ═══════════════════════════════════════════════════════════════════════════════
