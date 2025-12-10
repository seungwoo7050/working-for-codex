package com.example.training.search.service;

import co.elastic.clients.elasticsearch._types.query_dsl.BoolQuery;
import co.elastic.clients.elasticsearch._types.query_dsl.Query;
import co.elastic.clients.elasticsearch._types.query_dsl.RangeQuery;
import co.elastic.clients.json.JsonData;
import com.example.training.search.domain.ProductDocument;
import com.example.training.search.dto.ProductResponse;
import com.example.training.search.dto.ProductSearchRequest;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;
import org.springframework.data.elasticsearch.client.elc.NativeQuery;
import org.springframework.data.elasticsearch.core.ElasticsearchOperations;
import org.springframework.data.elasticsearch.core.SearchHit;
import org.springframework.data.elasticsearch.core.SearchHits;
import org.springframework.data.elasticsearch.core.mapping.IndexCoordinates;
import org.springframework.data.support.PageableExecutionUtils;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] ProductSearchService - Elasticsearch 전문 검색 서비스
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 상품 전문 검색 (키워드, 필터, 범위 검색)
 * - 관련 클론 가이드 단계: [CG-v1.3.0] 1.3.2 Elasticsearch 검색 구현
 * 
 * [LEARN] Elasticsearch vs RDBMS 검색 비교:
 * 
 * RDBMS (LIKE 검색):
 *   SELECT * FROM products WHERE name LIKE '%노트북%';
 *   - 인덱스 사용 불가 → Full Table Scan → 느림
 *   - 형태소 분석 불가 ("노트북" ≠ "notebook")
 * 
 * Elasticsearch (역색인 검색):
 *   GET /products/_search { "query": { "match": { "name": "노트북" } } }
 *   - 역색인(Inverted Index) → 밀리초 단위 검색
 *   - 형태소 분석, 동의어 처리 가능
 * 
 * C 관점:
 *   // RDBMS: 선형 탐색
 *   for (int i = 0; i < n; i++) {
 *       if (strstr(products[i].name, keyword)) ...
 *   }
 * 
 *   // Elasticsearch: 해시맵 룩업
 *   inverted_index["노트북"] → [doc1, doc5, doc23]  // O(1)
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Service
public class ProductSearchService {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] Elasticsearch 클라이언트 주입
    // ─────────────────────────────────────────────────────────────────────────
    private final ElasticsearchOperations elasticsearchOperations;
    // [LEARN] ElasticsearchOperations: Spring Data Elasticsearch의 핵심 인터페이스
    // CRUD + 검색 기능 제공 (JdbcTemplate과 유사한 역할)

    public ProductSearchService(ElasticsearchOperations elasticsearchOperations) {
        this.elasticsearchOperations = elasticsearchOperations;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 상품 검색 (복합 쿼리)
    // ─────────────────────────────────────────────────────────────────────────
    public Page<ProductResponse> searchProducts(ProductSearchRequest searchRequest) {
        Pageable pageable = PageRequest.of(searchRequest.getPage(), searchRequest.getSize());
        // [LEARN] 페이징: page=0, size=10 → 첫 10개 결과

        // ─────────────────────────────────────────────────────────────────────
        // [LEARN] Bool Query 구조:
        // {
        //   "bool": {
        //     "must": [...],    // AND 조건 (점수 영향)
        //     "filter": [...],  // AND 조건 (점수 무관, 캐싱 가능)
        //     "should": [...],  // OR 조건
        //     "must_not": [...] // NOT 조건
        //   }
        // }
        // ─────────────────────────────────────────────────────────────────────
        BoolQuery.Builder boolQueryBuilder = new BoolQuery.Builder();

        // Keyword search on name and description
        if (searchRequest.getQ() != null && !searchRequest.getQ().isEmpty()) {
            boolQueryBuilder.must(Query.of(q -> q
                .multiMatch(m -> m
                    .query(searchRequest.getQ())
                    .fields("name", "description")
                )
            ));
            // [LEARN] MultiMatch: 여러 필드에서 키워드 검색
            // - name 필드와 description 필드 모두에서 검색
            // - 점수(score) 계산에 영향 → 관련성 높은 문서가 상위에
        }

        // Filter by category
        if (searchRequest.getCategory() != null && !searchRequest.getCategory().isEmpty()) {
            boolQueryBuilder.filter(Query.of(q -> q
                .term(t -> t
                    .field("category")
                    .value(searchRequest.getCategory())
                )
            ));
            // [LEARN] Term Query + Filter:
            // - 정확히 일치하는 값만 검색 (분석기 적용 안 함)
            // - filter 절: 점수 계산 안 함 → 캐싱 가능 → 빠름
        }

        // Filter by brand
        if (searchRequest.getBrand() != null && !searchRequest.getBrand().isEmpty()) {
            boolQueryBuilder.filter(Query.of(q -> q
                .term(t -> t
                    .field("brand")
                    .value(searchRequest.getBrand())
                )
            ));
        }

        // Filter by price range
        if (searchRequest.getMinPrice() != null || searchRequest.getMaxPrice() != null) {
            RangeQuery.Builder rangeBuilder = new RangeQuery.Builder().field("price");

            if (searchRequest.getMinPrice() != null) {
                rangeBuilder.gte(JsonData.of(searchRequest.getMinPrice().doubleValue()));
                // [LEARN] gte: Greater Than or Equal (>=)
            }

            if (searchRequest.getMaxPrice() != null) {
                rangeBuilder.lte(JsonData.of(searchRequest.getMaxPrice().doubleValue()));
                // [LEARN] lte: Less Than or Equal (<=)
            }

            boolQueryBuilder.filter(Query.of(q -> q.range(rangeBuilder.build())));
            // [LEARN] Range Query: 범위 검색
            // SQL: WHERE price >= 10000 AND price <= 50000
        }

        // ─────────────────────────────────────────────────────────────────────
        // [Order 3] 네이티브 쿼리 빌드 및 실행
        // ─────────────────────────────────────────────────────────────────────
        NativeQuery query = NativeQuery.builder()
            .withQuery(Query.of(q -> q.bool(boolQueryBuilder.build())))
            .withPageable(pageable)
            .build();
        // [LEARN] NativeQuery: Elasticsearch DSL을 직접 사용하는 쿼리
        // Repository 메서드로 표현하기 어려운 복잡한 쿼리에 사용

        SearchHits<ProductDocument> searchHits = elasticsearchOperations.search(
            query,
            ProductDocument.class,
            IndexCoordinates.of("products")
        );
        // [LEARN] search() 실행:
        // - 쿼리를 Elasticsearch 클러스터로 전송
        // - SearchHits: 검색 결과 + 메타데이터 (총 개수, 점수 등)

        // ─────────────────────────────────────────────────────────────────────
        // [Order 4] 결과 변환 및 페이징
        // ─────────────────────────────────────────────────────────────────────
        List<ProductResponse> products = searchHits.getSearchHits().stream()
            .map(SearchHit::getContent)        // SearchHit → ProductDocument
            .map(ProductResponse::from)         // ProductDocument → DTO
            .collect(Collectors.toList());

        return PageableExecutionUtils.getPage(
            products,
            pageable,
            searchHits::getTotalHits            // 총 문서 수 (지연 계산)
        );
        // [LEARN] PageableExecutionUtils: Spring Data의 페이징 유틸리티
        // 총 개수를 지연 계산하여 불필요한 count 쿼리 방지
    }
}
