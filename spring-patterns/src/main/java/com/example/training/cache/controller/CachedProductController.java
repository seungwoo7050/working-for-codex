package com.example.training.cache.controller;

import com.example.training.cache.service.CachedProductService;
import com.example.training.cache.service.CachedProductService.ProductDTO;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.math.BigDecimal;
import java.util.List;

/**
 * 캐시 적용 상품 컨트롤러
 */
@RestController
@RequestMapping("/api/cache/products")
public class CachedProductController {

    private final CachedProductService productService;

    public CachedProductController(CachedProductService productService) {
        this.productService = productService;
    }

    /**
     * 전체 상품 조회
     */
    @GetMapping
    public List<ProductDTO> findAll() {
        return productService.findAll();
    }

    /**
     * ID로 상품 조회
     */
    @GetMapping("/{id}")
    public ResponseEntity<ProductDTO> findById(@PathVariable Long id) {
        return productService.findById(id)
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    /**
     * 상품 검색
     */
    @GetMapping("/search")
    public List<ProductDTO> search(@RequestParam String keyword) {
        return productService.search(keyword);
    }

    /**
     * 상품 생성
     */
    @PostMapping
    public ProductDTO create(@RequestBody CreateProductRequest request) {
        return productService.createProduct(request.name(), request.price());
    }

    /**
     * 상품 수정
     */
    @PutMapping("/{id}")
    public ResponseEntity<ProductDTO> update(
            @PathVariable Long id,
            @RequestBody UpdateProductRequest request) {
        return productService.updateProduct(id, request.name(), request.price())
                .map(ResponseEntity::ok)
                .orElse(ResponseEntity.notFound().build());
    }

    /**
     * 상품 삭제
     */
    @DeleteMapping("/{id}")
    public ResponseEntity<Void> delete(@PathVariable Long id) {
        if (productService.deleteProduct(id)) {
            return ResponseEntity.noContent().build();
        }
        return ResponseEntity.notFound().build();
    }

    /**
     * 캐시 전체 무효화
     */
    @DeleteMapping("/cache")
    public ResponseEntity<Void> clearCache() {
        productService.clearAllCaches();
        return ResponseEntity.noContent().build();
    }

    public record CreateProductRequest(String name, BigDecimal price) {}
    public record UpdateProductRequest(String name, BigDecimal price) {}
}
