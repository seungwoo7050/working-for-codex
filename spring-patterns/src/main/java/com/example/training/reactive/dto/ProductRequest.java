package com.example.training.reactive.dto;

import jakarta.validation.constraints.NotBlank;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Positive;
import jakarta.validation.constraints.PositiveOrZero;

import java.math.BigDecimal;

/**
 * 상품 생성/수정 요청 DTO
 */
public class ProductRequest {

    @NotBlank(message = "상품명은 필수입니다")
    private String name;

    private String description;

    @NotNull(message = "가격은 필수입니다")
    @Positive(message = "가격은 0보다 커야 합니다")
    private BigDecimal price;

    @NotNull(message = "재고는 필수입니다")
    @PositiveOrZero(message = "재고는 0 이상이어야 합니다")
    private Integer stock;

    public ProductRequest() {
    }

    public ProductRequest(String name, String description, BigDecimal price, Integer stock) {
        this.name = name;
        this.description = description;
        this.price = price;
        this.stock = stock;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public BigDecimal getPrice() {
        return price;
    }

    public void setPrice(BigDecimal price) {
        this.price = price;
    }

    public Integer getStock() {
        return stock;
    }

    public void setStock(Integer stock) {
        this.stock = stock;
    }
}
