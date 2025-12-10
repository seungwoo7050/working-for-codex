package com.example.training.reactive;

import com.example.training.reactive.controller.ReactiveProductController;
import com.example.training.reactive.dto.ProductRequest;
import com.example.training.reactive.dto.ProductResponse;
import com.example.training.reactive.event.ProductEventPublisher;
import com.example.training.reactive.service.ReactiveProductService;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.http.MediaType;
import org.springframework.test.web.reactive.server.WebTestClient;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import java.math.BigDecimal;
import java.time.LocalDateTime;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;

/**
 * ReactiveProductController 단위 테스트
 * 순수 Mockito 기반 테스트 - Spring Context 로드 없음
 * @Profile("reactive") 컴포넌트들과의 충돌 방지
 */
@ExtendWith(MockitoExtension.class)
class ReactiveProductControllerTest {

    private WebTestClient webTestClient;

    @Mock
    private ReactiveProductService productService;

    @Mock
    private ProductEventPublisher eventPublisher;

    @BeforeEach
    void setUp() {
        ReactiveProductController controller = new ReactiveProductController(productService, eventPublisher);
        webTestClient = WebTestClient.bindToController(controller).build();
    }

    private ProductResponse createSampleProduct(Long id, String name) {
        return new ProductResponse(
                id, name, "Description", new BigDecimal("1000"),
                10, LocalDateTime.now(), LocalDateTime.now()
        );
    }

    @Test
    void getAllProducts_returnsProducts() {
        ProductResponse product1 = createSampleProduct(1L, "Product 1");
        ProductResponse product2 = createSampleProduct(2L, "Product 2");

        Mockito.when(productService.findAll())
                .thenReturn(Flux.just(product1, product2));

        webTestClient.get()
                .uri("/api/v2/products")
                .accept(MediaType.APPLICATION_JSON)
                .exchange()
                .expectStatus().isOk()
                .expectBodyList(ProductResponse.class)
                .hasSize(2);
    }

    @Test
    void getProduct_whenExists_returnsProduct() {
        ProductResponse product = createSampleProduct(1L, "Test Product");

        Mockito.when(productService.findById(1L))
                .thenReturn(Mono.just(product));

        webTestClient.get()
                .uri("/api/v2/products/1")
                .accept(MediaType.APPLICATION_JSON)
                .exchange()
                .expectStatus().isOk()
                .expectBody()
                .jsonPath("$.name").isEqualTo("Test Product");
    }

    @Test
    void getProduct_whenNotExists_returns500() {
        // 순수 단위 테스트에서는 ExceptionHandler가 없으므로 예외는 500으로 반환됨
        // 실제 통합 테스트는 reactive 프로파일에서 수행
        Mockito.when(productService.findById(999L))
                .thenReturn(Mono.error(new ReactiveProductService.ResourceNotFoundException("Product not found: 999")));

        webTestClient.get()
                .uri("/api/v2/products/999")
                .accept(MediaType.APPLICATION_JSON)
                .exchange()
                .expectStatus().is5xxServerError();
    }

    @Test
    void createProduct_returnsCreated() {
        ProductRequest request = new ProductRequest("New Product", "Desc", new BigDecimal("1500"), 20);
        ProductResponse response = createSampleProduct(1L, "New Product");

        Mockito.when(productService.create(any(ProductRequest.class)))
                .thenReturn(Mono.just(response));

        webTestClient
                .post()
                .uri("/api/v2/products")
                .contentType(MediaType.APPLICATION_JSON)
                .bodyValue(request)
                .exchange()
                .expectStatus().isCreated()
                .expectBody()
                .jsonPath("$.name").isEqualTo("New Product");
    }

    @Test
    void updateProduct_returnsUpdated() {
        ProductRequest request = new ProductRequest("Updated Product", "Desc", new BigDecimal("2000"), 30);
        ProductResponse response = new ProductResponse(
                1L, "Updated Product", "Desc", new BigDecimal("2000"),
                30, LocalDateTime.now(), LocalDateTime.now()
        );

        Mockito.when(productService.update(eq(1L), any(ProductRequest.class)))
                .thenReturn(Mono.just(response));

        webTestClient
                .put()
                .uri("/api/v2/products/1")
                .contentType(MediaType.APPLICATION_JSON)
                .bodyValue(request)
                .exchange()
                .expectStatus().isOk()
                .expectBody()
                .jsonPath("$.name").isEqualTo("Updated Product");
    }

    @Test
    void deleteProduct_returnsNoContent() {
        Mockito.when(productService.delete(1L))
                .thenReturn(Mono.empty());

        webTestClient
                .delete()
                .uri("/api/v2/products/1")
                .exchange()
                .expectStatus().isNoContent();
    }
}
