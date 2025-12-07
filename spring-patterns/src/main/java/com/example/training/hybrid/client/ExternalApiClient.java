package com.example.training.hybrid.client;

import org.springframework.stereotype.Component;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;
import reactor.core.scheduler.Scheduler;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.util.List;
import java.util.concurrent.Semaphore;

/**
 * 외부 API 클라이언트
 * Virtual Threads를 사용한 병렬 HTTP 호출
 */
@Component
public class ExternalApiClient {

    private final HttpClient httpClient;
    private final Scheduler blockingScheduler;
    private final Semaphore rateLimiter;

    private static final int MAX_CONCURRENT_REQUESTS = 10;
    private static final Duration CONNECT_TIMEOUT = Duration.ofSeconds(10);
    private static final Duration REQUEST_TIMEOUT = Duration.ofSeconds(30);

    public ExternalApiClient(Scheduler blockingScheduler) {
        this.blockingScheduler = blockingScheduler;
        this.httpClient = HttpClient.newBuilder()
                .connectTimeout(CONNECT_TIMEOUT)
                .build();
        this.rateLimiter = new Semaphore(MAX_CONCURRENT_REQUESTS);
    }

    /**
     * 단일 HTTP GET 요청
     *
     * @param url 요청 URL
     * @return 응답 본문
     */
    public Mono<String> get(String url) {
        return Mono.fromCallable(() -> executeRequest(url))
                .subscribeOn(blockingScheduler);
    }

    /**
     * Rate limiting이 적용된 HTTP GET 요청
     *
     * @param url 요청 URL
     * @return 응답 본문
     */
    public Mono<String> getWithRateLimit(String url) {
        return Mono.fromCallable(() -> {
            rateLimiter.acquire();
            try {
                return executeRequest(url);
            } finally {
                rateLimiter.release();
            }
        }).subscribeOn(blockingScheduler);
    }

    /**
     * 여러 URL을 병렬로 호출
     *
     * @param urls URL 목록
     * @return 응답 스트림
     */
    public Flux<String> getParallel(List<String> urls) {
        return Flux.fromIterable(urls)
                .flatMap(this::getWithRateLimit, MAX_CONCURRENT_REQUESTS);
    }

    /**
     * 여러 URL을 호출하고 결과를 수집
     *
     * @param urls URL 목록
     * @return 모든 응답 목록
     */
    public Mono<List<String>> getAllAndCollect(List<String> urls) {
        return getParallel(urls).collectList();
    }

    /**
     * 타임아웃이 있는 HTTP GET 요청
     *
     * @param url 요청 URL
     * @param timeout 타임아웃
     * @return 응답 본문
     */
    public Mono<String> getWithTimeout(String url, Duration timeout) {
        return get(url).timeout(timeout);
    }

    /**
     * 폴백이 있는 HTTP GET 요청
     *
     * @param url 요청 URL
     * @param fallback 실패 시 폴백 값
     * @return 응답 본문 또는 폴백
     */
    public Mono<String> getWithFallback(String url, String fallback) {
        return get(url)
                .onErrorReturn(fallback);
    }

    /**
     * HTTP 요청 실행 (블로킹)
     */
    private String executeRequest(String url) throws Exception {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(url))
                .timeout(REQUEST_TIMEOUT)
                .GET()
                .build();

        HttpResponse<String> response = httpClient.send(
                request,
                HttpResponse.BodyHandlers.ofString()
        );

        if (response.statusCode() >= 400) {
            throw new RuntimeException("HTTP Error: " + response.statusCode());
        }

        return response.body();
    }
}
