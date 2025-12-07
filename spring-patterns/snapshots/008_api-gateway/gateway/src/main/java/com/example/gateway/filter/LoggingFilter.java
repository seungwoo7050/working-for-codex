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

/**
 * Global logging filter for all incoming requests.
 * Logs request method, path, and response status with timing.
 */
@Component
public class LoggingFilter implements GlobalFilter, Ordered {

    private static final Logger log = LoggerFactory.getLogger(LoggingFilter.class);
    private static final String REQUEST_ID_HEADER = "X-Request-Id";

    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        String requestId = generateRequestId();
        long startTime = System.currentTimeMillis();

        ServerHttpRequest request = exchange.getRequest();
        String method = request.getMethod() != null ? request.getMethod().name() : "UNKNOWN";
        String path = request.getURI().getPath();
        
        log.info("[{}] >> {} {}", requestId, method, path);

        // Add request ID to headers for tracing
        ServerHttpRequest mutatedRequest = request.mutate()
                .header(REQUEST_ID_HEADER, requestId)
                .build();

        return chain.filter(exchange.mutate().request(mutatedRequest).build())
                .doOnSuccess(v -> logResponse(requestId, exchange, startTime))
                .doOnError(e -> logError(requestId, e, startTime));
    }

    private void logResponse(String requestId, ServerWebExchange exchange, long startTime) {
        long duration = System.currentTimeMillis() - startTime;
        var statusCode = exchange.getResponse().getStatusCode();
        log.info("[{}] << {} ({}ms)", requestId, statusCode, duration);
    }

    private void logError(String requestId, Throwable error, long startTime) {
        long duration = System.currentTimeMillis() - startTime;
        log.error("[{}] << ERROR: {} ({}ms)", requestId, error.getMessage(), duration);
    }

    private String generateRequestId() {
        return UUID.randomUUID().toString().substring(0, 8);
    }

    @Override
    public int getOrder() {
        return -1; // High priority - execute early
    }
}
