package com.example.training.order.domain;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] OrderStatus Enum - 주문 상태 정의
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문의 생명주기 상태를 정의
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.1 주문 상태 설계
 * 
 * [LEARN] 상태 머신(State Machine) 패턴:
 * 주문 상태 전이:
 *   PENDING ──(결제 완료)──→ PAID
 *      │                      │
 *      └──(취소)──→ CANCELLED ←──(환불)──┘
 * 
 * C 관점:
 *   enum OrderStatus { PENDING=0, PAID=1, CANCELLED=2 };
 * 
 *   int can_cancel(enum OrderStatus status) {
 *       return status == PENDING;  // PENDING 상태만 취소 가능
 *   }
 * ═══════════════════════════════════════════════════════════════════════════════
 */
public enum OrderStatus {
    
    PENDING,
    // [LEARN] 대기 중: 주문 생성 직후, 결제 대기 상태
    // - 취소 가능
    // - 일정 시간(예: 30분) 후 자동 취소 가능
    
    PAID,
    // [LEARN] 결제 완료: 결제가 성공적으로 처리됨
    // - 배송 준비 시작
    // - 환불 요청 가능
    
    CANCELLED
    // [LEARN] 취소됨: 주문이 취소된 상태
    // - 최종 상태 (더 이상 전이 불가)
    // - 취소 사유 별도 저장 권장
    
    // ─────────────────────────────────────────────────────────────────────────
    // [LEARN] 실제 서비스에서는 더 많은 상태가 필요:
    // PENDING → PAID → PREPARING → SHIPPED → DELIVERED
    //    ↓        ↓        ↓          ↓
    // CANCELLED  REFUNDING → REFUNDED (환불)
    //
    // 상태 전이 검증 예:
    //   public boolean canTransitionTo(OrderStatus newStatus) {
    //       switch (this) {
    //           case PENDING: return newStatus == PAID || newStatus == CANCELLED;
    //           case PAID: return newStatus == CANCELLED;
    //           default: return false;
    //       }
    //   }
    // ─────────────────────────────────────────────────────────────────────────
}
