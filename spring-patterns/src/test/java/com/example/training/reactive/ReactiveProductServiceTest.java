package com.example.training.reactive;

import com.example.training.reactive.domain.ProductEntity;
import com.example.training.reactive.dto.ProductRequest;
import com.example.training.reactive.event.ProductEventPublisher;
import com.example.training.reactive.repository.ProductR2dbcRepository;
import com.example.training.reactive.service.ReactiveProductService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;
import reactor.test.StepVerifier;

import java.math.BigDecimal;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.when;

/**
 * ReactiveProductService 단위 테스트
 * StepVerifier를 사용한 리액티브 스트림 검증
 */
@ExtendWith(MockitoExtension.class)
class ReactiveProductServiceTest {

    @Mock
    private ProductR2dbcRepository repository;

    @Mock
    private ProductEventPublisher eventPublisher;

    private ReactiveProductService productService;

    @BeforeEach
    void setUp() {
        productService = new ReactiveProductService(repository, eventPublisher);
    }

    private ProductEntity createSampleEntity(Long id, String name) {
        ProductEntity entity = new ProductEntity(name, "Description", new BigDecimal("1000"), 10);
        entity.setId(id);
        return entity;
    }

    @Test
    void findAll_returnsAllProducts() {
        ProductEntity product1 = createSampleEntity(1L, "Product 1");
        ProductEntity product2 = createSampleEntity(2L, "Product 2");

        when(repository.findAll()).thenReturn(Flux.just(product1, product2));

        StepVerifier.create(productService.findAll())
                .expectNextMatches(p -> p.getName().equals("Product 1"))
                .expectNextMatches(p -> p.getName().equals("Product 2"))
                .verifyComplete();
    }

    @Test
    void findById_whenExists_returnsProduct() {
        ProductEntity product = createSampleEntity(1L, "Test Product");

        when(repository.findById(1L)).thenReturn(Mono.just(product));

        StepVerifier.create(productService.findById(1L))
                .expectNextMatches(p -> p.getName().equals("Test Product"))
                .verifyComplete();
    }

    @Test
    void findById_whenNotExists_returnsError() {
        when(repository.findById(999L)).thenReturn(Mono.empty());

        StepVerifier.create(productService.findById(999L))
                .expectError(ReactiveProductService.ResourceNotFoundException.class)
                .verify();
    }

    @Test
    void create_savesAndReturnsProduct() {
        ProductRequest request = new ProductRequest("New Product", "Desc", new BigDecimal("1500"), 20);
        ProductEntity savedEntity = createSampleEntity(1L, "New Product");

        when(repository.save(any(ProductEntity.class))).thenReturn(Mono.just(savedEntity));

        StepVerifier.create(productService.create(request))
                .expectNextMatches(p -> p.getName().equals("New Product") && p.getId().equals(1L))
                .verifyComplete();
    }

    @Test
    void update_whenExists_updatesAndReturns() {
        ProductRequest request = new ProductRequest("Updated", "Desc", new BigDecimal("2000"), 30);
        ProductEntity existingEntity = createSampleEntity(1L, "Original");
        ProductEntity updatedEntity = createSampleEntity(1L, "Updated");

        when(repository.findById(1L)).thenReturn(Mono.just(existingEntity));
        when(repository.save(any(ProductEntity.class))).thenReturn(Mono.just(updatedEntity));

        StepVerifier.create(productService.update(1L, request))
                .expectNextMatches(p -> p.getName().equals("Updated"))
                .verifyComplete();
    }

    @Test
    void update_whenNotExists_returnsError() {
        ProductRequest request = new ProductRequest("Updated", "Desc", new BigDecimal("2000"), 30);

        when(repository.findById(999L)).thenReturn(Mono.empty());

        StepVerifier.create(productService.update(999L, request))
                .expectError(ReactiveProductService.ResourceNotFoundException.class)
                .verify();
    }

    @Test
    void delete_whenExists_deletesSuccessfully() {
        ProductEntity entity = createSampleEntity(1L, "To Delete");

        when(repository.findById(1L)).thenReturn(Mono.just(entity));
        when(repository.deleteById(1L)).thenReturn(Mono.empty());

        StepVerifier.create(productService.delete(1L))
                .verifyComplete();
    }

    @Test
    void delete_whenNotExists_returnsError() {
        when(repository.findById(999L)).thenReturn(Mono.empty());

        StepVerifier.create(productService.delete(999L))
                .expectError(ReactiveProductService.ResourceNotFoundException.class)
                .verify();
    }

    @Test
    void searchByName_returnsMatchingProducts() {
        ProductEntity product = createSampleEntity(1L, "MacBook Pro");

        when(repository.findByNameContaining("Mac")).thenReturn(Flux.just(product));

        StepVerifier.create(productService.searchByName("Mac"))
                .expectNextMatches(p -> p.getName().contains("Mac"))
                .verifyComplete();
    }
}
