package com.example.training.reactive.service;

import com.example.training.reactive.domain.ProductEntity;
import com.example.training.reactive.dto.ProductRequest;
import com.example.training.reactive.dto.ProductResponse;
import com.example.training.reactive.event.ProductEventPublisher;
import com.example.training.reactive.repository.ProductR2dbcRepository;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Service;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import java.math.BigDecimal;
import java.time.LocalDateTime;

/**
 * 리액티브 상품 서비스
 * Mono/Flux를 사용한 논블로킹 비즈니스 로직
 */
@Service
@Profile("reactive")
public class ReactiveProductService {

    private final ProductR2dbcRepository repository;
    private final ProductEventPublisher eventPublisher;

    public ReactiveProductService(ProductR2dbcRepository repository, ProductEventPublisher eventPublisher) {
        this.repository = repository;
        this.eventPublisher = eventPublisher;
    }

    /**
     * 모든 상품 조회
     */
    public Flux<ProductResponse> findAll() {
        return repository.findAll()
                .map(ProductResponse::from);
    }

    /**
     * ID로 상품 조회
     */
    public Mono<ProductResponse> findById(Long id) {
        return repository.findById(id)
                .map(ProductResponse::from)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)));
    }

    /**
     * 이름으로 상품 검색
     */
    public Flux<ProductResponse> searchByName(String keyword) {
        return repository.findByNameContaining(keyword)
                .map(ProductResponse::from);
    }

    /**
     * 가격 범위로 상품 검색
     */
    public Flux<ProductResponse> findByPriceRange(BigDecimal min, BigDecimal max) {
        return repository.findByPriceRange(min, max)
                .map(ProductResponse::from);
    }

    /**
     * 상품 생성
     */
    public Mono<ProductResponse> create(ProductRequest request) {
        ProductEntity entity = new ProductEntity(
                request.getName(),
                request.getDescription(),
                request.getPrice(),
                request.getStock()
        );

        return repository.save(entity)
                .map(ProductResponse::from)
                .doOnSuccess(eventPublisher::publish);
    }

    /**
     * 상품 수정
     */
    public Mono<ProductResponse> update(Long id, ProductRequest request) {
        return repository.findById(id)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)))
                .flatMap(entity -> {
                    entity.setName(request.getName());
                    entity.setDescription(request.getDescription());
                    entity.setPrice(request.getPrice());
                    entity.setStock(request.getStock());
                    entity.setUpdatedAt(LocalDateTime.now());
                    return repository.save(entity);
                })
                .map(ProductResponse::from)
                .doOnSuccess(eventPublisher::publish);
    }

    /**
     * 상품 삭제
     */
    public Mono<Void> delete(Long id) {
        return repository.findById(id)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)))
                .flatMap(entity -> repository.deleteById(id));
    }

    /**
     * 재고 있는 상품만 조회
     */
    public Flux<ProductResponse> findInStock() {
        return repository.findInStock()
                .map(ProductResponse::from);
    }

    /**
     * 리소스를 찾을 수 없을 때 발생하는 예외
     */
    public static class ResourceNotFoundException extends RuntimeException {
        public ResourceNotFoundException(String message) {
            super(message);
        }
    }
}
