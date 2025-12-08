package com.example.training.order.controller;

import com.example.training.order.dto.CreateOrderRequest;
import com.example.training.order.dto.OrderResponse;
import com.example.training.order.service.OrderService;
import jakarta.validation.Valid;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.security.core.Authentication;
import org.springframework.web.bind.annotation.*;

import java.util.List;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] OrderController - 주문 관리 REST API
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문 생성/조회 API + Kafka 이벤트 발행 연동
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.3 주문 API
 * 
 * [LEARN] 이벤트 기반 주문 처리 흐름:
 * 1. POST /api/orders → 주문 생성
 * 2. OrderService.createOrder() → DB 저장
 * 3. Kafka 이벤트 발행 (ORDER_CREATED)
 * 4. 클라이언트에 즉시 응답 (201 Created)
 * 5. [비동기] Consumer가 이메일 발송, 재고 업데이트 등 처리
 * 
 * 장점: 응답 시간 단축, 서비스 간 느슨한 결합
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@RestController
@RequestMapping("/api/orders")
public class OrderController {

    private final OrderService orderService;

    public OrderController(OrderService orderService) {
        this.orderService = orderService;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 주문 생성
    // ─────────────────────────────────────────────────────────────────────────
    @PostMapping
    public ResponseEntity<OrderResponse> createOrder(
            Authentication authentication,
            @Valid @RequestBody CreateOrderRequest request) {
        Long userId = Long.parseLong(authentication.getName());
        // [LEARN] authentication.getName():
        // JWT subject에서 사용자 ID 추출 (문자열 → Long 변환)
        // IssueController와 다른 방식 (getPrincipal vs getName)
        // → 프로젝트에서 일관성 유지 필요
        
        OrderResponse response = orderService.createOrder(userId, request);
        // [LEARN] 내부에서 Kafka 이벤트 발행
        // 주문 저장 + 이벤트 발행이 하나의 트랜잭션은 아님 (주의)
        
        return ResponseEntity.status(HttpStatus.CREATED).body(response);
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 주문 상세 조회
    // ─────────────────────────────────────────────────────────────────────────
    @GetMapping("/{id}")
    public ResponseEntity<OrderResponse> getOrder(@PathVariable Long id) {
        OrderResponse response = orderService.getOrder(id);
        return ResponseEntity.ok(response);
    }
    // [LEARN] 주의: 현재 구현은 모든 사용자가 모든 주문 조회 가능
    // 실제 서비스에서는 주문 소유자 검증 필요:
    //   if (!order.getUserId().equals(currentUserId)) {
    //       throw new ForbiddenException("권한 없음");
    //   }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 내 주문 목록 조회
    // ─────────────────────────────────────────────────────────────────────────
    @GetMapping
    public ResponseEntity<List<OrderResponse>> getUserOrders(Authentication authentication) {
        Long userId = Long.parseLong(authentication.getName());
        List<OrderResponse> orders = orderService.getUserOrders(userId);
        return ResponseEntity.ok(orders);
    }
    // [LEARN] 인증된 사용자의 주문만 조회
    // userId로 필터링하여 다른 사용자 주문 접근 방지
}
