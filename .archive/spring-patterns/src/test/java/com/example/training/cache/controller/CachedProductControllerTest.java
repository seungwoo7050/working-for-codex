package com.example.training.cache.controller;

import com.example.training.cache.service.CachedProductService;
import com.example.training.cache.service.CachedProductService.ProductDTO;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.DisplayName;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

import java.math.BigDecimal;
import java.util.List;
import java.util.Optional;

import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.when;

/**
 * CachedProductController 단위 테스트
 */
@ExtendWith(MockitoExtension.class)
class CachedProductControllerTest {

    @Mock
    private CachedProductService productService;

    @InjectMocks
    private CachedProductController controller;

    private ProductDTO sampleProduct;

    @BeforeEach
    void setUp() {
        sampleProduct = new ProductDTO(1L, "Test Product", new BigDecimal("100.00"));
    }

    @Test
    @DisplayName("전체 상품 조회")
    void shouldGetAllProducts() {
        // given
        when(productService.findAll()).thenReturn(List.of(sampleProduct));

        // when
        List<ProductDTO> result = controller.findAll();

        // then
        assertThat(result).hasSize(1);
        assertThat(result.get(0).name()).isEqualTo("Test Product");
    }

    @Test
    @DisplayName("ID로 상품 조회 성공")
    void shouldGetProductById() {
        // given
        when(productService.findById(1L)).thenReturn(Optional.of(sampleProduct));

        // when
        ResponseEntity<ProductDTO> result = controller.findById(1L);

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.OK);
        assertThat(result.getBody()).isNotNull();
        assertThat(result.getBody().name()).isEqualTo("Test Product");
    }

    @Test
    @DisplayName("상품 없음 404")
    void shouldReturn404WhenProductNotFound() {
        // given
        when(productService.findById(999L)).thenReturn(Optional.empty());

        // when
        ResponseEntity<ProductDTO> result = controller.findById(999L);

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NOT_FOUND);
    }

    @Test
    @DisplayName("상품 검색")
    void shouldSearchProducts() {
        // given
        when(productService.search("Test")).thenReturn(List.of(sampleProduct));

        // when
        List<ProductDTO> result = controller.search("Test");

        // then
        assertThat(result).hasSize(1);
        assertThat(result.get(0).name()).isEqualTo("Test Product");
    }

    @Test
    @DisplayName("상품 생성")
    void shouldCreateProduct() {
        // given
        when(productService.createProduct(any(), any())).thenReturn(sampleProduct);
        var request = new CachedProductController.CreateProductRequest("Test Product", new BigDecimal("100.00"));

        // when
        ProductDTO result = controller.create(request);

        // then
        assertThat(result.name()).isEqualTo("Test Product");
    }

    @Test
    @DisplayName("상품 수정")
    void shouldUpdateProduct() {
        // given
        var updated = new ProductDTO(1L, "Updated Product", new BigDecimal("200.00"));
        when(productService.updateProduct(eq(1L), any(), any())).thenReturn(Optional.of(updated));
        var request = new CachedProductController.UpdateProductRequest("Updated Product", new BigDecimal("200.00"));

        // when
        ResponseEntity<ProductDTO> result = controller.update(1L, request);

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.OK);
        assertThat(result.getBody().name()).isEqualTo("Updated Product");
    }

    @Test
    @DisplayName("상품 삭제")
    void shouldDeleteProduct() {
        // given
        when(productService.deleteProduct(1L)).thenReturn(true);

        // when
        ResponseEntity<Void> result = controller.delete(1L);

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
    }

    @Test
    @DisplayName("캐시 무효화")
    void shouldClearCache() {
        // when
        ResponseEntity<Void> result = controller.clearCache();

        // then
        assertThat(result.getStatusCode()).isEqualTo(HttpStatus.NO_CONTENT);
    }
}
