package com.example.gateway.filter;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.cloud.gateway.filter.GatewayFilterChain;
import org.springframework.cloud.gateway.filter.GlobalFilter;
import org.springframework.core.Ordered;
import org.springframework.http.server.reactive.ServerHttpRequest;
import org.springframework.stereotype.Component;
import org.springframework.web.server.ServerWebExchange;
import reactor.core.publisher.Mono;

import java.util.UUID;

// [FILE]
// - 목적: 모든 Gateway 요청에 대한 로깅 및 추적
// - 주요 역할: 요청 ID 생성, 요청/응답 로깅, 처리 시간 측정
// - 관련 클론 가이드 단계: [CG-v1.6.0] 1.6.4 공통 필터 (로깅)
// - 권장 읽는 순서: Order 1(filter) → Order 2(logResponse) → Order 3(getOrder)
//
// [LEARN] C 개발자를 위한 설명:
// GlobalFilter는 "모든 요청을 가로채는 미들웨어"다.
// 
// C에서 HTTP 프록시 로깅을 구현한다면:
// void handle_request(int client_fd) {
//     char request_id[9];
//     generate_uuid(request_id, 8);
//     
//     clock_t start = clock();
//     log_request(request_id, method, path);       // >> GET /api/users
//     
//     forward_to_backend(client_fd, backend_fd);
//     
//     double duration = (clock() - start) * 1000.0 / CLOCKS_PER_SEC;
//     log_response(request_id, status, duration);  // << 200 (45ms)
// }
//
// Spring Cloud Gateway의 GlobalFilter는 이 패턴을 리액티브로 구현한다.

@Component
// [LEARN] @Component: Spring Bean으로 자동 등록
// GlobalFilter 인터페이스 구현체는 모든 라우트에 자동 적용됨
public class LoggingFilter implements GlobalFilter, Ordered {

    private static final Logger log = LoggerFactory.getLogger(LoggingFilter.class);
    private static final String REQUEST_ID_HEADER = "X-Request-Id";
    // [LEARN] X-Request-Id: 분산 시스템에서 요청 추적용 표준 헤더
    // 백엔드 서비스에서도 같은 ID로 로깅하면 전체 흐름 추적 가능

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 필터 로직 - 요청 가로채기 및 로깅
    // ═══════════════════════════════════════════════════════════════════════
    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        // [LEARN] ServerWebExchange: 요청 + 응답을 감싸는 컨테이너
        // C의 struct { Request* req; Response* res; } context; 와 유사
        
        String requestId = generateRequestId();
        long startTime = System.currentTimeMillis();

        ServerHttpRequest request = exchange.getRequest();
        String method = request.getMethod() != null ? request.getMethod().name() : "UNKNOWN";
        String path = request.getURI().getPath();
        
        log.info("[{}] >> {} {}", requestId, method, path);
        // [LEARN] >> 표시: 요청 시작 로그 (inbound)

        // [LEARN] Request 헤더에 Request ID 추가 (불변 객체이므로 mutate 사용)
        ServerHttpRequest mutatedRequest = request.mutate()
                .header(REQUEST_ID_HEADER, requestId)
                .build();

        // [LEARN] chain.filter(): 다음 필터로 전달 (또는 백엔드로 라우팅)
        // doOnSuccess/doOnError: 결과에 따른 콜백 등록
        return chain.filter(exchange.mutate().request(mutatedRequest).build())
                .doOnSuccess(v -> logResponse(requestId, exchange, startTime))
                .doOnError(e -> logError(requestId, e, startTime));
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 응답 로깅
    // ═══════════════════════════════════════════════════════════════════════
    private void logResponse(String requestId, ServerWebExchange exchange, long startTime) {
        long duration = System.currentTimeMillis() - startTime;
        var statusCode = exchange.getResponse().getStatusCode();
        log.info("[{}] << {} ({}ms)", requestId, statusCode, duration);
        // [LEARN] << 표시: 응답 완료 로그 (outbound)
        // 처리 시간(ms)도 함께 기록하여 성능 모니터링
    }

    private void logError(String requestId, Throwable error, long startTime) {
        long duration = System.currentTimeMillis() - startTime;
        log.error("[{}] << ERROR: {} ({}ms)", requestId, error.getMessage(), duration);
    }

    private String generateRequestId() {
        // [LEARN] UUID 앞 8자리만 사용 (로그 가독성)
        // 전체 UUID는 36자로 너무 길어서 단축
        return UUID.randomUUID().toString().substring(0, 8);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 필터 우선순위
    // ═══════════════════════════════════════════════════════════════════════
    @Override
    public int getOrder() {
        return -1; // High priority - execute early
    }
    // [LEARN] Ordered 인터페이스: 필터 실행 순서 정의
    // 숫자가 낮을수록 먼저 실행 (-1 = 거의 최우선)
    // 로깅 필터는 가장 먼저 실행되어 전체 요청 흐름을 추적해야 함
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: GlobalFilter, 리액티브 필터 체인
//
// Spring Cloud Gateway 필터 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. GlobalFilter vs GatewayFilter:
//    - GlobalFilter: 모든 라우트에 적용
//    - GatewayFilter: 특정 라우트에만 적용 (application.yml에서 지정)
//
// 2. 리액티브 필터 체인:
//    요청 → [Filter1] → [Filter2] → [Filter3] → Backend
//          ↑              ↑              ↑
//         -1             0              1        (order)
//
//    C에서의 책임 연쇄 패턴과 유사:
//    typedef struct Filter { int (*handle)(Context*); struct Filter* next; } Filter;
//
// 3. Mono<Void>:
//    - 비동기 완료 신호 (결과값 없음)
//    - 필터는 "처리 완료"만 알리면 되므로 Void 사용
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. backend의 common/controller/HealthController.java - 라우팅 대상
// 2. backend의 ratelimit/RateLimitingFilter.java - 유사한 필터 패턴
// ═══════════════════════════════════════════════════════════════════════════════
