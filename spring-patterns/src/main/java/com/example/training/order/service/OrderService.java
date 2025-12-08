package com.example.training.order.service;

import com.example.training.common.exception.ResourceNotFoundException;
import com.example.training.order.domain.Order;
import com.example.training.order.domain.OrderItem;
import com.example.training.order.dto.CreateOrderRequest;
import com.example.training.order.dto.OrderItemRequest;
import com.example.training.order.dto.OrderResponse;
import com.example.training.order.event.OrderEvent;
import com.example.training.order.repository.OrderRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.kafka.core.KafkaTemplate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.stream.Collectors;

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * [FILE] OrderService - Kafka 이벤트 기반 주문 처리
 * ─────────────────────────────────────────────────────────────────────────────────
 * - 목적: 주문 생성/조회 + 이벤트 발행
 * - 관련 클론 가이드 단계: [CG-v1.4.0] 1.4.2 비동기 이벤트 발행
 * 
 * [LEARN] 이벤트 기반 아키텍처 (Event-Driven Architecture):
 * 동기 방식:
 *   createOrder() → saveToDb() → sendEmail() → sendSms() → return
 *   (전체 처리 완료까지 응답 지연)
 * 
 * 이벤트 기반:
 *   createOrder() → saveToDb() → publishEvent() → return (즉시)
 *                                      ↓
 *   [Kafka] → Consumer1: sendEmail()
 *          → Consumer2: sendSms()
 *          → Consumer3: updateStats()
 * 
 * 장점:
 * - 응답 시간 단축
 * - 서비스 간 느슨한 결합(loose coupling)
 * - 확장 용이 (새 Consumer 추가만으로 기능 확장)
 * ═══════════════════════════════════════════════════════════════════════════════
 */
@Service
public class OrderService {

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 1] 로깅과 상수
    // ─────────────────────────────────────────────────────────────────────────
    private static final Logger logger = LoggerFactory.getLogger(OrderService.class);
    // [LEARN] SLF4J Logger: 로깅 추상화 라이브러리
    // 실제 구현체(Logback, Log4j2)는 런타임에 결정
    
    private static final String ORDER_EVENTS_TOPIC = "order-events";
    // [LEARN] Kafka 토픽 이름: 이벤트가 발행되는 채널
    // 토픽 = 메시지 카테고리 (우편함 역할)

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 2] 의존성 주입
    // ─────────────────────────────────────────────────────────────────────────
    private final OrderRepository orderRepository;
    private final KafkaTemplate<String, OrderEvent> kafkaTemplate;
    // [LEARN] KafkaTemplate<K, V>:
    // - K: 메시지 키 타입 (파티셔닝에 사용)
    // - V: 메시지 값 타입 (이벤트 객체)
    // Spring Boot가 자동 설정 (application.yml의 kafka 설정 사용)

    public OrderService(OrderRepository orderRepository, KafkaTemplate<String, OrderEvent> kafkaTemplate) {
        this.orderRepository = orderRepository;
        this.kafkaTemplate = kafkaTemplate;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 3] 주문 생성 + 이벤트 발행
    // ─────────────────────────────────────────────────────────────────────────
    @Transactional
    public OrderResponse createOrder(Long userId, CreateOrderRequest request) {
        // Create order
        Order order = new Order();
        order.setUserId(userId);

        // Add items
        for (OrderItemRequest itemRequest : request.getItems()) {
            OrderItem item = new OrderItem();
            item.setProductId(itemRequest.getProductId());
            item.setQuantity(itemRequest.getQuantity());
            item.setPrice(itemRequest.getPrice());
            order.addItem(item);
            // [LEARN] 편의 메서드 사용: Order ↔ OrderItem 양방향 연관관계 설정
        }

        // Calculate total amount
        order.calculateTotalAmount();
        // [LEARN] 도메인 로직은 엔티티 내부에 (Rich Domain Model)

        // Save order
        Order savedOrder = orderRepository.save(order);

        // Publish event after transaction commits
        publishOrderCreatedEvent(savedOrder);
        // [LEARN] 주의: 이 시점에서 트랜잭션이 아직 커밋되지 않았을 수 있음
        // 완벽하게 하려면 @TransactionalEventListener 사용

        logger.info("Order created: orderId={}, userId={}, totalAmount={}",
                savedOrder.getId(), savedOrder.getUserId(), savedOrder.getTotalAmount());

        return new OrderResponse(savedOrder);
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 4] Kafka 이벤트 발행
    // ─────────────────────────────────────────────────────────────────────────
    private void publishOrderCreatedEvent(Order order) {
        try {
            OrderEvent event = new OrderEvent(
                    "ORDER_CREATED",
                    order.getId(),
                    order.getUserId(),
                    order.getTotalAmount()
            );
            // [LEARN] 이벤트 객체 생성: 불변(immutable) 스냅샷
            // Order 엔티티 자체를 보내지 않고, 필요한 정보만 담은 DTO

            kafkaTemplate.send(ORDER_EVENTS_TOPIC, event.getEventId(), event);
            // [LEARN] kafkaTemplate.send(topic, key, value):
            // - topic: 이벤트가 발행되는 채널
            // - key: 파티션 결정에 사용 (같은 키 = 같은 파티션 = 순서 보장)
            // - value: 실제 이벤트 데이터 (JSON 직렬화됨)
            //
            // C 관점:
            //   int fd = socket(AF_INET, SOCK_STREAM, 0);
            //   connect(fd, kafka_broker, ...);
            //   write(fd, serialize(event), len);
            //   → 이 모든 것을 send() 한 줄로!
            
            logger.info("Published ORDER_CREATED event: eventId={}, orderId={}",
                    event.getEventId(), order.getId());
        } catch (Exception e) {
            logger.error("Failed to publish ORDER_CREATED event for orderId={}", order.getId(), e);
            // [LEARN] 이벤트 발행 실패 처리:
            // - 프로덕션에서는 재시도 메커니즘 필요
            // - Dead Letter Queue(DLQ)로 실패 이벤트 격리
            // - 주문 생성은 성공했으므로 예외를 다시 던지지 않음
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // [Order 5] 읽기 전용 트랜잭션
    // ─────────────────────────────────────────────────────────────────────────
    @Transactional(readOnly = true)
    public OrderResponse getOrder(Long orderId) {
        Order order = orderRepository.findById(orderId)
                .orElseThrow(() -> new ResourceNotFoundException("Order not found"));
        return new OrderResponse(order);
    }
    // [LEARN] readOnly = true: 읽기 전용 트랜잭션
    // - 더티 체킹 생략 → 성능 향상
    // - DB 레플리카로 라우팅 가능

    @Transactional(readOnly = true)
    public List<OrderResponse> getUserOrders(Long userId) {
        List<Order> orders = orderRepository.findByUserId(userId);
        return orders.stream()
                .map(OrderResponse::new)
                .collect(Collectors.toList());
    }
    // [LEARN] 사용자의 모든 주문 조회
    // Stream API로 엔티티 → DTO 변환
}
