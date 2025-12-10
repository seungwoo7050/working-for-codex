package com.example.training.reactive.service;

import com.example.training.reactive.domain.ProductEntity;
import com.example.training.reactive.dto.ProductRequest;
import com.example.training.reactive.dto.ProductResponse;
import com.example.training.reactive.event.ProductEventPublisher;
import com.example.training.reactive.repository.ProductR2dbcRepository;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Service;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Mono;

import java.math.BigDecimal;
import java.time.LocalDateTime;

// [FILE]
// - 목적: WebFlux 기반 리액티브 상품 서비스
// - 주요 역할: Mono/Flux를 사용한 논블로킹 비즈니스 로직
// - 관련 클론 가이드 단계: [CG-v2.0.0] WebFlux & R2DBC 전환
// - 권장 읽는 순서: Order 1(조회) → Order 2(생성) → Order 3(수정/삭제)
//
// [LEARN] C 개발자를 위한 설명:
// 이 클래스는 "리액티브(Reactive) 프로그래밍" 패턴을 구현한다.
// 전통적인 블로킹 방식과 리액티브 방식의 핵심 차이:
//
// [블로킹 방식 - 이전 IssueService]
// public Issue findById(Long id) {
//     return repository.findById(id);  // DB 응답 올 때까지 스레드 대기
// }
// → 요청 100개 = 스레드 100개 필요
// → 스레드풀 고갈 시 더 이상 요청 처리 불가
//
// [리액티브 방식 - 이 클래스]
// public Mono<Product> findById(Long id) {
//     return repository.findById(id);  // 즉시 반환, 나중에 결과 도착
// }
// → 요청 100개 = 소수의 이벤트 루프 스레드로 처리
// → 스레드가 대기하지 않고 다른 작업 수행
//
// C 관점에서 비유:
// - 블로킹: read(fd, buf, n); // 데이터 올 때까지 블록
// - 논블로킹: fcntl(fd, F_SETFL, O_NONBLOCK); epoll_wait(...); // 이벤트 대기
//
// Mono<T>: 0개 또는 1개의 결과 (Optional과 유사하지만 비동기)
// Flux<T>: 0개 이상의 결과 스트림 (List와 유사하지만 비동기)

@Service
@Profile("reactive")
// [LEARN] @Profile("reactive"): 프로파일이 "reactive"일 때만 이 Bean 활성화
// application.yml에서 spring.profiles.active=reactive 설정 시 사용
// 기존 블로킹 서비스와 충돌하지 않게 분리
public class ReactiveProductService {

    private final ProductR2dbcRepository repository;
    // [LEARN] R2DBC: Reactive Relational Database Connectivity
    // JDBC의 리액티브 버전, 논블로킹 DB 접근 제공
    // 일반 JpaRepository 대신 R2dbcRepository 사용

    private final ProductEventPublisher eventPublisher;

    public ReactiveProductService(ProductR2dbcRepository repository, ProductEventPublisher eventPublisher) {
        this.repository = repository;
        this.eventPublisher = eventPublisher;
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 1] 조회 메서드 - Flux/Mono 반환
    // ═══════════════════════════════════════════════════════════════════════

    public Flux<ProductResponse> findAll() {
        // [LEARN] Flux: 여러 개의 상품을 스트림으로 반환
        // 전통적 List<Product>와 달리, 데이터가 도착하는 대로 하나씩 방출
        // 대용량 데이터에서 메모리 효율적 (전체 로딩 불필요)
        return repository.findAll()
                .map(ProductResponse::from);
                // [LEARN] .map(): 각 요소를 변환 (Entity → DTO)
                // Stream.map()과 유사하지만 비동기로 동작
    }

    public Mono<ProductResponse> findById(Long id) {
        // [LEARN] Mono: 0개 또는 1개의 상품 반환
        // 결과가 없으면 빈 Mono, 있으면 값을 담은 Mono
        return repository.findById(id)
                .map(ProductResponse::from)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)));
                // [LEARN] switchIfEmpty: 결과가 없으면 다른 Mono로 전환
                // 여기서는 에러 Mono를 반환하여 예외 발생
    }

    public Flux<ProductResponse> searchByName(String keyword) {
        return repository.findByNameContaining(keyword)
                .map(ProductResponse::from);
    }

    public Flux<ProductResponse> findByPriceRange(BigDecimal min, BigDecimal max) {
        return repository.findByPriceRange(min, max)
                .map(ProductResponse::from);
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 2] 생성 메서드
    // ═══════════════════════════════════════════════════════════════════════

    public Mono<ProductResponse> create(ProductRequest request) {
        ProductEntity entity = new ProductEntity(
                request.getName(),
                request.getDescription(),
                request.getPrice(),
                request.getStock()
        );

        return repository.save(entity)
                .map(ProductResponse::from)
                .doOnSuccess(eventPublisher::publish);
                // [LEARN] doOnSuccess: 성공 시 부수 효과(side effect) 실행
                // 이벤트 발행, 로깅 등 메인 흐름에 영향 없는 작업에 사용
                // C에서 콜백 함수를 등록하는 것과 유사
    }

    // ═══════════════════════════════════════════════════════════════════════
    // [Order 3] 수정/삭제 메서드
    // ═══════════════════════════════════════════════════════════════════════

    public Mono<ProductResponse> update(Long id, ProductRequest request) {
        return repository.findById(id)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)))
                .flatMap(entity -> {
                    // [LEARN] flatMap: 비동기 작업을 체인으로 연결
                    // map은 동기 변환, flatMap은 비동기 변환 (Mono/Flux 반환)
                    //
                    // 비유: map = f(x) → y
                    //       flatMap = f(x) → Mono<y> (비동기 결과)
                    entity.setName(request.getName());
                    entity.setDescription(request.getDescription());
                    entity.setPrice(request.getPrice());
                    entity.setStock(request.getStock());
                    entity.setUpdatedAt(LocalDateTime.now());
                    return repository.save(entity);
                })
                .map(ProductResponse::from)
                .doOnSuccess(eventPublisher::publish);
    }

    public Mono<Void> delete(Long id) {
        // [LEARN] Mono<Void>: 결과 없이 완료만 알림
        // 삭제 작업은 반환값이 없으므로 Void 사용
        return repository.findById(id)
                .switchIfEmpty(Mono.error(new ResourceNotFoundException("Product not found: " + id)))
                .flatMap(entity -> repository.deleteById(id));
    }

    public Flux<ProductResponse> findInStock() {
        return repository.findInStock()
                .map(ProductResponse::from);
    }

    /**
     * 리소스를 찾을 수 없을 때 발생하는 예외
     */
    public static class ResourceNotFoundException extends RuntimeException {
        public ResourceNotFoundException(String message) {
            super(message);
        }
    }
}

// [Reader Notes]
// ═══════════════════════════════════════════════════════════════════════════════
// 이 파일에서 처음 등장한 개념: 리액티브 프로그래밍, WebFlux, Mono/Flux
//
// 리액티브 프로그래밍 (C 개발자 관점):
// ───────────────────────────────────────────────────────────────────────────────
// 1. 블로킹 vs 논블로킹 I/O:
//
//    [블로킹 - 전통적 Spring MVC]
//    요청 → 스레드 할당 → DB 쿼리 (대기) → 응답 → 스레드 반환
//    문제: 스레드가 I/O 대기 중에 아무것도 못 함
//
//    [논블로킹 - WebFlux]
//    요청 → 이벤트 루프에 등록 → DB 쿼리 시작 → 스레드는 다른 요청 처리
//         → DB 응답 도착 → 이벤트 루프가 콜백 실행 → 응답
//    장점: 적은 스레드로 많은 동시 요청 처리
//
// 2. C에서의 유사 패턴:
//    - epoll/kqueue/IOCP: 이벤트 기반 I/O 멀티플렉싱
//    - libuv(Node.js), libevent: 이벤트 루프 라이브러리
//    - 콜백 지옥 vs Reactor 패턴: Mono/Flux는 콜백을 체이닝으로 깔끔하게 표현
//
// 3. 주요 연산자:
//    - map: 동기 변환 (A → B)
//    - flatMap: 비동기 변환 (A → Mono<B>)
//    - filter: 조건에 맞는 요소만 통과
//    - switchIfEmpty: 빈 결과 대체
//    - doOnSuccess/doOnError: 부수 효과
//    - zip: 여러 Mono 결합
//
// 4. 주의사항:
//    - 구독(subscribe) 전까지 아무것도 실행 안 됨 (Lazy)
//    - 블로킹 코드 호출하면 성능 이점 사라짐
//    - 디버깅이 어려움 (스택 트레이스가 복잡)
//
// 이 파일을 이해한 다음 볼 파일:
// ───────────────────────────────────────────────────────────────────────────────
// 1. reactive/repository/ProductR2dbcRepository.java - R2DBC Repository
// 2. reactive/controller/ReactiveProductController.java - WebFlux Controller
// ═══════════════════════════════════════════════════════════════════════════════
