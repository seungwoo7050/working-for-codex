package com.example.training.order.repository;

import com.example.training.order.domain.Order;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] OrderRepository - 주문 데이터 접근 계층
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문 엔티티의 CRUD 및 조회 기능 제공
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.1 주문 도메인
 * 
 * [LEARN] Spring Data JPA Repository 상속 계층:
 * 
 * Repository (마커 인터페이스)
 *     ↓
 * CrudRepository (save, findById, delete, ...)
 *     ↓
 * PagingAndSortingRepository (findAll(Pageable), findAll(Sort))
 *     ↓
 * JpaRepository (flush, saveAndFlush, deleteAllInBatch, ...)
 * 
 * JpaRepository<Entity, ID>를 상속하면 모든 기능 사용 가능
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Repository
public interface OrderRepository extends JpaRepository<Order, Long> {
    
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] 쿼리 메서드 네이밍 규칙
    // ─────────────────────────────────────────────────────────────────────────
    List<Order> findByUserId(Long userId);
    // [LEARN] 메서드 이름으로 쿼리 자동 생성:
    // findByUserId → SELECT * FROM orders WHERE user_id = ?
    //
    // 네이밍 규칙:
    // - findBy + 필드명: 조건 검색
    // - findByFieldAndField2: AND 조건
    // - findByFieldOrField2: OR 조건
    // - findByFieldOrderByField2Desc: 정렬
    // - findByFieldGreaterThan: 비교 연산
    //
    // C 관점:
    //   SELECT * FROM orders WHERE user_id = ?
    //   → 이 SQL을 Spring Data가 자동으로 생성
}
