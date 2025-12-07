# 스냅샷 006: Async Events (v1.4.0)

## 개요
- **대응 버전**: v1.4.0
- **패치 범위**: 1.4.1 ~ 1.4.5
- **빌드 가능**: ⚠️ Kafka 인스턴스 필요
- **테스트 가능**: ⚠️ Kafka 인스턴스 필요

## 포함된 기능
- v1.3.0의 모든 기능 (Elasticsearch)
- Order 도메인
- OrderEvent (이벤트 객체)
- Kafka Producer/Consumer

## 새로 추가된 컴포넌트
### Entity
- Order
- OrderStatus (enum)

### Event
- OrderEvent
- OrderCreatedEvent
- OrderCompletedEvent

### Kafka
- OrderEventProducer (@KafkaListener)
- OrderEventConsumer

### Service
- OrderService

### Controller
- OrderController

### Configuration
- KafkaConfig (Producer/Consumer 설정)

## 주요 어노테이션
- `@KafkaListener`: Kafka 메시지 수신
- `@EnableKafka`: Kafka 활성화

## 빌드 방법
```bash
cd snapshots/006_async-events
./gradlew build
```

## 테스트 방법
```bash
# Kafka 실행 필요
docker-compose up -d kafka

./gradlew test
```

## 제한 사항
- 로컬 Kafka 없으면 테스트 실패
- docker-compose.yml로 Kafka 컨테이너 실행 권장
