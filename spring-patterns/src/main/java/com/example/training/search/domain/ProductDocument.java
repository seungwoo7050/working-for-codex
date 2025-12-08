package com.example.training.search.domain;

import org.springframework.data.annotation.Id;
import org.springframework.data.elasticsearch.annotations.Document;
import org.springframework.data.elasticsearch.annotations.Field;
import org.springframework.data.elasticsearch.annotations.FieldType;

import java.math.BigDecimal;
import java.time.LocalDateTime;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] ProductDocument - Elasticsearch 문서 매핑
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 상품 정보를 Elasticsearch 인덱스에 저장하기 위한 문서 모델
 * - 관련 클론 가이드 단계: [CG-v1.3.0] 1.3.1 Elasticsearch 문서 설계
 * 
 * [LEARN] JPA Entity vs Elasticsearch Document:
 * 
 * JPA Entity:
 *   - RDBMS 테이블에 매핑
 *   - @Entity, @Table, @Column
 *   - 관계형 데이터, 트랜잭션 처리
 * 
 * Elasticsearch Document:
 *   - ES 인덱스에 매핑
 *   - @Document, @Field
 *   - 전문 검색, 집계 분석
 * 
 * C 관점: 같은 데이터를 두 가지 저장소에 다른 형태로 저장
 *   struct ProductDB { ... };   // PostgreSQL용
 *   struct ProductES { ... };   // Elasticsearch용
 *   // DB에서 읽어서 ES에 색인
 * ═══════════════════════════════════════════════════════════════════════════════
 */

// ─────────────────────────────────────────────────────────────────────────────
// [Order 1] Elasticsearch 문서 어노테이션
// ─────────────────────────────────────────────────────────────────────────────
@Document(indexName = "products")
// [LEARN] @Document: Elasticsearch 인덱스에 매핑
// indexName = "products": 저장될 인덱스 이름
// RDBMS의 테이블과 유사하지만, 스키마가 더 유연함
public class ProductDocument {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 필드 매핑 - 문서 ID
    // ─────────────────────────────────────────────────────────────────────────
    @Id
    private String id;
    // [LEARN] ES 문서 ID: 유일 식별자
    // String 타입 권장 (ES 내부적으로 문자열로 처리)

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 필드 매핑 - Text vs Keyword
    // ─────────────────────────────────────────────────────────────────────────
    @Field(type = FieldType.Text)
    private String name;
    // [LEARN] FieldType.Text: 전문 검색용
    // - 분석기(Analyzer)가 토큰화 수행
    // - "Apple MacBook Pro" → ["apple", "macbook", "pro"]
    // - 검색 시 "macbook"만 입력해도 매칭됨

    @Field(type = FieldType.Text)
    private String description;
    // [LEARN] 상품 설명: 전문 검색 대상

    @Field(type = FieldType.Keyword)
    private String category;
    // [LEARN] FieldType.Keyword: 정확한 값 매칭용
    // - 분석기 적용 안 함 (원본 그대로 저장)
    // - "Electronics" → ["Electronics"]
    // - 집계(Aggregation), 정렬, 필터링에 사용
    // - 검색 시 정확히 "Electronics" 입력해야 매칭

    @Field(type = FieldType.Keyword)
    private String brand;
    // [LEARN] 브랜드: 필터용 → Keyword 타입

    @Field(type = FieldType.Double)
    private BigDecimal price;
    // [LEARN] 숫자 타입: 범위 검색, 정렬에 사용
    // - Double: 부동소수점
    // - Long: 정수
    // - 가격 범위 검색: price >= 10000 AND price <= 50000

    @Field(type = FieldType.Keyword)
    private String status;
    // [LEARN] 상태값: Enum을 String으로 저장 → Keyword 타입

    @Field(type = FieldType.Date)
    private LocalDateTime createdAt;
    // [LEARN] 날짜 타입: 날짜 범위 검색, 시계열 분석
    // - 포맷 설정 가능: @Field(type = Date, format = DateFormat.date_hour_minute_second)

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 생성자
    // ─────────────────────────────────────────────────────────────────────────
    protected ProductDocument() {
    }
    // [LEARN] 기본 생성자: 역직렬화(JSON → 객체)에 필요

    public ProductDocument(String id, String name, String description, String category, String brand,
                          BigDecimal price, String status, LocalDateTime createdAt) {
        this.id = id;
        this.name = name;
        this.description = description;
        this.category = category;
        this.brand = brand;
        this.price = price;
        this.status = status;
        this.createdAt = createdAt;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 5] 팩토리 메서드 - Entity → Document 변환
    // ─────────────────────────────────────────────────────────────────────────
    public static ProductDocument from(Product product) {
        return new ProductDocument(
            product.getId().toString(),
            product.getName(),
            product.getDescription(),
            product.getCategory(),
            product.getBrand(),
            product.getPrice(),
            product.getStatus().name(),
            product.getCreatedAt()
        );
    }
    // [LEARN] 정적 팩토리 메서드: JPA Entity → ES Document 변환
    // 사용 예:
    //   Product product = productRepository.findById(id);
    //   ProductDocument doc = ProductDocument.from(product);
    //   elasticsearchOps.save(doc);  // ES에 색인
    //
    // 왜 분리하는가?
    // - JPA Entity는 영속성 컨텍스트, 지연 로딩 등 복잡한 상태 가짐
    // - ES Document는 단순한 JSON 직렬화 대상
    // - 관심사 분리(Separation of Concerns)

    // Getters
    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getCategory() {
        return category;
    }

    public String getBrand() {
        return brand;
    }

    public BigDecimal getPrice() {
        return price;
    }

    public String getStatus() {
        return status;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }
}
