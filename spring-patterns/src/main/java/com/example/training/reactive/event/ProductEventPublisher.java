package com.example.training.reactive.event;

import com.example.training.reactive.dto.ProductResponse;
import org.springframework.context.annotation.Profile;
import org.springframework.stereotype.Component;
import reactor.core.publisher.Flux;
import reactor.core.publisher.Sinks;

/**
 * 상품 이벤트 발행자
 * SSE를 통한 실시간 상품 업데이트 스트리밍
 */
@Component
@Profile("reactive")
public class ProductEventPublisher {

    private final Sinks.Many<ProductResponse> sink;

    public ProductEventPublisher() {
        // 멀티캐스트 싱크: 여러 구독자에게 동시에 이벤트 발행
        // onBackpressureBuffer: 백프레셔 발생 시 버퍼링
        this.sink = Sinks.many().multicast().onBackpressureBuffer();
    }

    /**
     * 상품 이벤트 발행
     * 상품이 생성/수정될 때 호출
     */
    public void publish(ProductResponse product) {
        sink.tryEmitNext(product);
    }

    /**
     * 이벤트 스트림 반환
     * SSE 클라이언트가 구독
     */
    public Flux<ProductResponse> getEventStream() {
        return sink.asFlux();
    }
}
