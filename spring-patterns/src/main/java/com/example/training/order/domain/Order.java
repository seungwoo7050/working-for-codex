package com.example.training.order.domain;

import jakarta.persistence.*;
import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] Order 엔티티 - 주문 도메인 모델
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문 정보와 주문 항목들을 저장하는 JPA 엔티티
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.1 이벤트 기반 주문 처리
 * 
 * [LEARN] 일대다(1:N) 연관관계:
 * Order(1) ↔ OrderItem(N) 관계
 * - 하나의 주문에 여러 주문 항목이 포함됨
 * - 부모 엔티티(Order)가 자식 엔티티(OrderItem)의 생명주기를 관리
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Entity
@Table(name = "orders")
// [LEARN] 테이블명 "orders": SQL 예약어 "ORDER"를 피하기 위해 복수형 사용
public class Order {

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(nullable = false)
    private Long userId;
    // [LEARN] 주문자 ID: User 엔티티와 FK 관계 (여기서는 ID만 저장)

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] BigDecimal 컬럼 매핑
    // ─────────────────────────────────────────────────────────────────────────
    @Column(nullable = false, precision = 10, scale = 2)
    private BigDecimal totalAmount;
    // [LEARN] BigDecimal: 금액 계산에 필수! double/float 사용 금지
    // 
    // 왜 double이 위험한가?
    //   double price = 0.1 + 0.2;  // 0.30000000000000004 (부동소수점 오차)
    // 
    // BigDecimal 사용:
    //   BigDecimal price = new BigDecimal("0.1").add(new BigDecimal("0.2"));
    //   // 정확히 0.3
    //
    // precision = 10: 전체 자릿수 (최대 10자리)
    // scale = 2: 소수점 이하 자릿수 (2자리 = 센트 단위)
    // DDL: total_amount DECIMAL(10,2) NOT NULL

    @Enumerated(EnumType.STRING)
    @Column(nullable = false)
    private OrderStatus status;
    // [LEARN] 주문 상태: PENDING → PAID → CANCELLED 등
    // 상태 머신(State Machine) 패턴으로 관리 권장

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] 일대다 연관관계 (부모 → 자식)
    // ─────────────────────────────────────────────────────────────────────────
    @OneToMany(mappedBy = "order", cascade = CascadeType.ALL, orphanRemoval = true)
    private List<OrderItem> items = new ArrayList<>();
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] @OneToMany 연관관계 상세:
    // 
    // mappedBy = "order":
    //   - 연관관계의 주인은 OrderItem.order 필드
    //   - 외래키(FK)는 order_items 테이블에 있음
    //   - Order는 읽기 전용 (mappedBy 쪽은 FK 관리 안 함)
    //
    // cascade = CascadeType.ALL:
    //   - Order 저장 시 OrderItem도 함께 저장
    //   - Order 삭제 시 OrderItem도 함께 삭제
    //   - C 관점: free(order) 호출 시 free(order->items[i]) 자동 호출
    //
    // orphanRemoval = true:
    //   - items 컬렉션에서 제거된 OrderItem은 DB에서도 삭제
    //   - order.getItems().remove(item) → DELETE FROM order_items
    //
    // 초기화 = new ArrayList<>():
    //   - NullPointerException 방지
    //   - 빈 리스트로 시작
    // ─────────────────────────────────────────────────────────────────────────

    @Column(nullable = false, updatable = false)
    private LocalDateTime createdAt;

    @PrePersist
    protected void onCreate() {
        createdAt = LocalDateTime.now();
        if (status == null) {
            status = OrderStatus.PENDING;
        }
    }
    // [LEARN] @PrePersist: 기본 상태 설정
    // 주문 생성 시 자동으로 PENDING 상태로 시작

    // Getters and Setters
    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    public Long getUserId() {
        return userId;
    }

    public void setUserId(Long userId) {
        this.userId = userId;
    }

    public BigDecimal getTotalAmount() {
        return totalAmount;
    }

    public void setTotalAmount(BigDecimal totalAmount) {
        this.totalAmount = totalAmount;
    }

    public OrderStatus getStatus() {
        return status;
    }

    public void setStatus(OrderStatus status) {
        this.status = status;
    }

    public List<OrderItem> getItems() {
        return items;
    }

    public void setItems(List<OrderItem> items) {
        this.items = items;
    }

    public LocalDateTime getCreatedAt() {
        return createdAt;
    }

    public void setCreatedAt(LocalDateTime createdAt) {
        this.createdAt = createdAt;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 5] 연관관계 편의 메서드
    // ─────────────────────────────────────────────────────────────────────────
    // Helper method to add items
    public void addItem(OrderItem item) {
        items.add(item);
        item.setOrder(this);
    }
    // [LEARN] 양방향 연관관계 편의 메서드:
    // - 부모(Order)에 자식(OrderItem) 추가
    // - 동시에 자식에게 부모 참조 설정
    // - 양쪽 동기화를 한 번에 처리
    //
    // 이렇게 안 하면:
    //   order.getItems().add(item);  // 부모 → 자식
    //   item.setOrder(order);         // 자식 → 부모 (빠뜨리기 쉬움!)

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 6] 도메인 로직 (DDD 패턴)
    // ─────────────────────────────────────────────────────────────────────────
    // Helper method to calculate total amount
    public void calculateTotalAmount() {
        this.totalAmount = items.stream()
                .map(item -> item.getPrice().multiply(BigDecimal.valueOf(item.getQuantity())))
                .reduce(BigDecimal.ZERO, BigDecimal::add);
    }
    // [LEARN] 도메인 로직을 엔티티 내부에 구현 (Rich Domain Model):
    // - 총액 계산은 Order의 책임
    // - 서비스 레이어가 아닌 도메인 객체가 비즈니스 로직 담당
    //
    // Stream API 분석:
    //   items.stream()                           // List → Stream
    //       .map(item -> price * quantity)       // 각 항목 금액 계산
    //       .reduce(ZERO, BigDecimal::add)       // 모두 합산
    //
    // C 관점:
    //   BigDecimal total = ZERO;
    //   for (int i = 0; i < items.size(); i++) {
    //       total = total.add(items[i].price * items[i].quantity);
    //   }
}
