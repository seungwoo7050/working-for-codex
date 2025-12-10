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

// [FILE]
// - 목적: Virtual Threads를 활용한 외부 HTTP API 클라이언트
// - 주요 역할: 블로킹 HTTP 호출을 리액티브 스트림으로 래핑, 병렬 호출 지원
// - 관련 클론 가이드 단계: [CG-v2.1.0] 2.1.4 외부 API 병렬 호출
// - 권장 읽는 순서: Order 1(get) → Order 2(getParallel) → Order 3(executeRequest)
//
// [LEARN] C 개발자를 위한 설명:
// 외부 API 호출은 네트워크 I/O로 인해 수십~수백 ms가 걸린다.
// 여러 API를 순차 호출하면 지연이 누적되고, 병렬 호출이 필수적이다.
//
// [순차 호출 - 비효율적]
// for (int i = 0; i < 10; i++) {
//     fetch_api(urls[i]);  // 각각 100ms → 총 1초
// }
//
// [병렬 호출 - 효율적]
// for (int i = 0; i < 10; i++) {
//     pthread_create(&threads[i], fetch_api, urls[i]);
// }
// for (int i = 0; i < 10; i++) {
//     pthread_join(threads[i], NULL);  // 총 100ms (가장 느린 것)
// }
//
// 이 클래스는 Virtual Threads + Reactor를 사용해
// 수천 개의 동시 HTTP 요청도 효율적으로 처리한다.

@Component
public class ExternalApiClient {

    private final HttpClient httpClient;
    // [LEARN] Java 11+ HttpClient: 표준 HTTP 클라이언트
    // C의 libcurl과 유사한 역할
    
    private final Scheduler blockingScheduler;
    // [LEARN] Scheduler: 블로킹 작업을 실행할 스레드 풀
    // Virtual Thread 기반이라 수천 개 동시 요청 가능
    
    private final Semaphore rateLimiter;
    // [LEARN] Semaphore: 동시 요청 수 제한
    // C의 sem_wait()/sem_post()와 동일

    private static final int MAX_CONCURRENT_REQUESTS = 10;
    // [LEARN] 동시 요청 제한: 외부 서버 부하 방지, API 제한 준수
    
    private static final Duration CONNECT_TIMEOUT = Duration.ofSeconds(10);
    private static final Duration REQUEST_TIMEOUT = Duration.ofSeconds(30);

    public ExternalApiClient(Scheduler blockingScheduler) {
        this.blockingScheduler = blockingScheduler;
        this.httpClient = HttpClient.newBuilder()
                .connectTimeout(CONNECT_TIMEOUT)
                .build();
        this.rateLimiter = new Semaphore(MAX_CONCURRENT_REQUESTS);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 단일 HTTP 요청
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * 단일 HTTP GET 요청 (비동기)
     * Virtual Thread에서 블로킹 HTTP 호출 실행
     */
    public Mono<String> get(String url) {
        return Mono.fromCallable(() -> executeRequest(url))
                .subscribeOn(blockingScheduler);
    }
    // [LEARN] Mono.fromCallable(): 블로킹 람다를 Mono로 변환
    // subscribeOn(blockingScheduler): Virtual Thread에서 실행

    /**
     * Rate limiting이 적용된 HTTP GET 요청
     */
    public Mono<String> getWithRateLimit(String url) {
        return Mono.fromCallable(() -> {
            rateLimiter.acquire();
            // [LEARN] acquire(): 세마포어 획득 (없으면 대기)
            try {
                return executeRequest(url);
            } finally {
                rateLimiter.release();
                // [LEARN] release(): 세마포어 반환 (다른 요청 허용)
            }
        }).subscribeOn(blockingScheduler);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 병렬 HTTP 요청
    // ═══════════════════════════════════════════════════════════════════════

    /**
     * 여러 URL을 병렬로 호출
     */
    public Flux<String> getParallel(List<String> urls) {
        return Flux.fromIterable(urls)
                .flatMap(this::getWithRateLimit, MAX_CONCURRENT_REQUESTS);
    }
    // [LEARN] flatMap(..., concurrency): 동시 실행 수 제한
    // 10개씩 병렬 처리하고 완료되면 다음 10개 시작

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
