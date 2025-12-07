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

/**
 * 리액티브 상품 컨트롤러
 * WebFlux 기반 비동기 REST API
 */
@RestController
@RequestMapping("/api/v2/products")
@Profile("reactive")
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
    public Flux<ServerSentEvent<ProductResponse>> streamProducts() {
        return eventPublisher.getEventStream()
                .map(product -> ServerSentEvent.<ProductResponse>builder()
                        .id(String.valueOf(product.getId()))
                        .event("product-update")
                        .data(product)
                        .build());
    }
}
