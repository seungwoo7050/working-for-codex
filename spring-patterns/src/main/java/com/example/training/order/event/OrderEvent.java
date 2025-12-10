package com.example.training.order.event;

import java.math.BigDecimal;
import java.time.LocalDateTime;
import java.util.UUID;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] OrderEvent - Kafka 이벤트 메시지 DTO
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문 관련 이벤트를 Kafka로 전송하기 위한 메시지 객체
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.2 이벤트 설계
 * 
 * [LEARN] 이벤트 메시지 설계 원칙:
 * 
 * 1. 자기 설명적(Self-describing):
 *    - eventId: 이벤트 고유 식별자 (중복 처리 방지)
 *    - eventType: 이벤트 종류 (라우팅, 처리 분기)
 *    - timestamp: 발생 시간 (순서, 디버깅)
 * 
 * 2. 불변성(Immutable):
 *    - 이벤트는 "과거에 일어난 사실"
 *    - 한 번 발행되면 변경 불가
 * 
 * 3. 독립성(Decoupled):
 *    - 엔티티 직접 참조 X (순환 의존 방지)
 *    - 필요한 데이터만 복사
 * 
 * C 관점:
 *   struct OrderEvent {
 *       char event_id[37];  // UUID
 *       char event_type[32];
 *       time_t timestamp;
 *       long order_id;
 *       // ...
 *   };
 * ═══════════════════════════════════════════════════════════════════════════════
 */
public class OrderEvent {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 이벤트 메타데이터
    // ─────────────────────────────────────────────────────────────────────────
    private String eventId;
    // [LEARN] UUID: 전역 고유 식별자
    // - 이벤트 중복 처리 방지 (멱등성 보장)
    // - 컨슈머가 이미 처리한 이벤트인지 확인
    
    private String eventType;
    // [LEARN] 이벤트 유형: "ORDER_CREATED", "ORDER_PAID", "ORDER_CANCELLED"
    // 컨슈머가 처리할 이벤트 타입 필터링
    
    private LocalDateTime timestamp;
    // [LEARN] 이벤트 발생 시간: 순서 보장, 디버깅, 감사 로그

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 비즈니스 데이터
    // ─────────────────────────────────────────────────────────────────────────
    private Long orderId;
    private Long userId;
    private BigDecimal totalAmount;
    // [LEARN] 필요한 데이터만 포함:
    // - Order 엔티티 전체가 아닌 핵심 정보만
    // - 컨슈머가 추가 정보 필요시 orderId로 조회

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 생성자
    // ─────────────────────────────────────────────────────────────────────────
    // No-arg constructor for deserialization
    public OrderEvent() {
    }
    // [LEARN] 기본 생성자: JSON 역직렬화에 필요
    // Kafka Consumer가 JSON → OrderEvent 변환 시 사용

    public OrderEvent(String eventType, Long orderId, Long userId, BigDecimal totalAmount) {
        this.eventId = UUID.randomUUID().toString();
        // [LEARN] UUID.randomUUID(): 전역 고유 ID 생성
        // 충돌 확률: 10^-37 (사실상 0)
        
        this.eventType = eventType;
        this.timestamp = LocalDateTime.now();
        this.orderId = orderId;
        this.userId = userId;
        this.totalAmount = totalAmount;
    }
    // [LEARN] 이벤트 생성 시 자동 설정:
    // - eventId: 새 UUID 생성
    // - timestamp: 현재 시간
    // 비즈니스 데이터만 파라미터로 받음

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] Getter/Setter
    // ─────────────────────────────────────────────────────────────────────────
    // Getters and Setters
    public String getEventId() {
        return eventId;
    }

    public void setEventId(String eventId) {
        this.eventId = eventId;
    }

    public String getEventType() {
        return eventType;
    }

    public void setEventType(String eventType) {
        this.eventType = eventType;
    }

    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }

    public Long getOrderId() {
        return orderId;
    }

    public void setOrderId(Long orderId) {
        this.orderId = orderId;
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
    // [LEARN] Setter 필요 이유:
    // Jackson JSON 역직렬화가 setter를 통해 값 설정
    // (또는 @JsonProperty로 필드 직접 설정 가능)
}
