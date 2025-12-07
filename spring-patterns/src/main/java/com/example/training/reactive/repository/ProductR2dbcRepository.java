package com.example.training.reactive.repository;

import com.example.training.reactive.domain.ProductEntity;
import org.springframework.context.annotation.Profile;
import org.springframework.data.r2dbc.repository.Query;
import org.springframework.data.repository.reactive.ReactiveCrudRepository;
import org.springframework.stereotype.Repository;
import reactor.core.publisher.Flux;

import java.math.BigDecimal;

/**
 * R2DBC 기반 리액티브 Product Repository
 */
@Repository
@Profile("reactive")
public interface ProductR2dbcRepository extends ReactiveCrudRepository<ProductEntity, Long> {

    /**
     * 이름에 키워드를 포함하는 상품 검색
     */
    Flux<ProductEntity> findByNameContaining(String keyword);

    /**
     * 가격 범위로 상품 검색
     */
    @Query("SELECT * FROM products WHERE price BETWEEN :min AND :max")
    Flux<ProductEntity> findByPriceRange(BigDecimal min, BigDecimal max);

    /**
     * 재고가 있는 상품만 조회
     */
    @Query("SELECT * FROM products WHERE stock > 0")
    Flux<ProductEntity> findInStock();
}
