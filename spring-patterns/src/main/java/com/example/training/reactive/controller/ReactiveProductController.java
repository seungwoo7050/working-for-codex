package com.example.training.reactive.controller;

import com.example.training.reactive.dto.ProductRequest;
import com.example.training.reactive.dto.ProductResponse;
import com.example.training.reactive.event.ProductEventPublisher;
import com.example.training.reactive.service.ReactiveProductService;
import jakarta.validation.Valid;
import org.springframework.context.annotation.Profile;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.codec.ServerSentEvent;
import org.springframework.web.bind.annotation.*;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import java.math.BigDecimal;

// [FILE]
// - 목적: WebFlux 기반 리액티브 상품 REST API
// - 주요 역할: 비동기 HTTP 요청 처리, SSE 스트리밍 제공
// - 관련 클론 가이드 단계: [CG-v2.0.0] 2.0.3 리액티브 컨트롤러 구현
// - 권장 읽는 순서: Order 1(조회) → Order 2(생성/수정/삭제) → Order 3(SSE 스트리밍)
//
// [LEARN] C 개발자를 위한 설명:
// 이 컨트롤러는 기존 IssueController와 비슷하지만 "리액티브" 방식이다.
//
// [기존 MVC Controller]
// @GetMapping("/{id}")
// public Product getProduct(@PathVariable Long id) {
//     return productService.findById(id);  // 동기 - 결과 올 때까지 스레드 대기
// }
//
// [WebFlux Controller - 이 클래스]
// @GetMapping("/{id}")
// public Mono<Product> getProduct(@PathVariable Long id) {
//     return productService.findById(id);  // 비동기 - 즉시 반환, 나중에 결과 전달
// }
//
// C 관점 비유:
// - 동기: read(fd, buf, n); return buf;    // 블로킹
// - 비동기: aio_read(&aiocb); return &aiocb; // 논블로킹, 나중에 결과 확인

@RestController
@RequestMapping("/api/v2/products")
// [LEARN] /api/v2/: 버전 구분
// 기존 블로킹 API(/api/products)와 리액티브 API를 분리
@Profile("reactive")
// [LEARN] @Profile("reactive"): spring.profiles.active=reactive 일 때만 활성화
// 동일 경로의 블로킹 컨트롤러와 충돌 방지
public class ReactiveProductController {

    private final ReactiveProductService productService;
    private final ProductEventPublisher eventPublisher;

    public ReactiveProductController(ReactiveProductService productService,
                                      ProductEventPublisher eventPublisher) {
        this.productService = productService;
        this.eventPublisher = eventPublisher;
    }

    /**
     * 모든 상품 조회
     * GET /api/v2/products
     */
    @GetMapping
    public Flux<ProductResponse> getAllProducts() {
        return productService.findAll();
    }

    /**
     * ID로 상품 조회
     * GET /api/v2/products/{id}
     */
    @GetMapping("/{id}")
    public Mono<ProductResponse> getProduct(@PathVariable Long id) {
        return productService.findById(id);
    }

    /**
     * 이름으로 상품 검색
     * GET /api/v2/products/search?keyword=xxx
     */
    @GetMapping("/search")
    public Flux<ProductResponse> searchProducts(@RequestParam String keyword) {
        return productService.searchByName(keyword);
    }

    /**
     * 가격 범위로 상품 검색
     * GET /api/v2/products/price-range?min=100&max=1000
     */
    @GetMapping("/price-range")
    public Flux<ProductResponse> getProductsByPriceRange(
            @RequestParam BigDecimal min,
            @RequestParam BigDecimal max) {
        return productService.findByPriceRange(min, max);
    }

    /**
     * 재고 있는 상품만 조회
     * GET /api/v2/products/in-stock
     */
    @GetMapping("/in-stock")
    public Flux<ProductResponse> getProductsInStock() {
        return productService.findInStock();
    }

    /**
     * 상품 생성
     * POST /api/v2/products
     */
    @PostMapping
    @ResponseStatus(HttpStatus.CREATED)
    public Mono<ProductResponse> createProduct(@Valid @RequestBody ProductRequest request) {
        return productService.create(request);
    }

    /**
     * 상품 수정
     * PUT /api/v2/products/{id}
     */
    @PutMapping("/{id}")
    public Mono<ProductResponse> updateProduct(
            @PathVariable Long id,
            @Valid @RequestBody ProductRequest request) {
        return productService.update(id, request);
    }

    /**
     * 상품 삭제
     * DELETE /api/v2/products/{id}
     */
    @DeleteMapping("/{id}")
    @ResponseStatus(HttpStatus.NO_CONTENT)
    public Mono<Void> deleteProduct(@PathVariable Long id) {
        return productService.delete(id);
    }

    /**
     * 상품 업데이트 실시간 스트리밍 (SSE)
     * GET /api/v2/products/stream
     */
    @GetMapping(value = "/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
    // [LEARN] MediaType.TEXT_EVENT_STREAM_VALUE: SSE(Server-Sent Events) 응답
    // 클라이언트와 지속적인 연결을 유지하며 데이터를 스트리밍
    public Flux<ServerSentEvent<ProductResponse>> streamProducts() {
        return eventPublisher.getEventStream()
                .map(product -> ServerSentEvent.<ProductResponse>builder()
                        .id(String.valueOf(product.getId()))
                        .event("product-update")
                        .data(product)
                        .build());
    }
    // [LEARN] SSE (Server-Sent Events):
    // - HTTP 연결 유지하며 서버→클라이언트 단방향 스트리밍
    // - WebSocket보다 단순 (단방향이므로)
    // - 실시간 알림, 대시보드 갱신 등에 사용
    //
    // C 관점:
    // while (1) {
    //     event = wait_for_event();
    //     fprintf(client_stream, "event: product-update\n");
    //     fprintf(client_stream, "data: %s\n\n", json);
    //     fflush(client_stream);
    // }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: WebFlux Controller, SSE 스트리밍
//
// WebFlux Controller (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. MVC vs WebFlux 차이:
//    [MVC]     @GetMapping → ResponseEntity<List<T>> (동기, 전체 응답 완료 후 반환)
//    [WebFlux] @GetMapping → Flux<T> (비동기, 데이터 도착하는 대로 스트리밍)
//
// 2. Mono<T> vs Flux<T>:
//    - Mono<T>: 0~1개 결과 (단건 조회, 생성, 삭제)
//    - Flux<T>: 0~N개 결과 (목록 조회, 스트리밍)
//
// 3. 스레드 모델:
//    [MVC]     요청 1개 = 스레드 1개 (thread-per-request)
//    [WebFlux] 요청 N개 = 소수의 이벤트 루프 스레드 (event-loop)
//
// 4. SSE vs WebSocket:
//    [SSE]       서버 → 클라이언트 (단방향)
//    [WebSocket] 서버 ↔ 클라이언트 (양방향)
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. reactive/service/ReactiveProductService.java - 비동기 비즈니스 로직
// 2. reactive/event/ProductEventPublisher.java - SSE 이벤트 발행
// ═══════════════════════════════════════════════════════════════════════════════
