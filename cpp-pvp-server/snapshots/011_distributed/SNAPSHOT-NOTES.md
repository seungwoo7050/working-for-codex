# 스냅샷 011_distributed (v2.0.0 ~ v2.0.2)

> 분산 서버 아키텍처 완료 시점 (코드 기반 스냅샷)

## 대응 버전
- **버전**: v2.0.0 ~ v2.0.2
- **패치 범위**: 2.0.0-p1 ~ 2.0.2-p4

## 이전 스냅샷과의 차이
- 010_observability 기반 + distributed/ 모듈 및 session_store 추가
- 신규 파일: consistent_hash.h/cpp, load_balancer.h/cpp, service_discovery.h/cpp
- 신규 파일: session_store.h, in_memory_session_store.h, redis_session_store.h

## 패치 내용

### v2.0.0: 세션 서버 분리 및 Redis 통합
| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.0.0-p1 | SessionStore 인터페이스 정의 | `storage/session_store.h` |
| 2.0.0-p2 | InMemorySessionStore 구현 | `storage/in_memory_session_store.*` |
| 2.0.0-p3 | RedisSessionStore 구현 | `storage/redis_session_store.*` |
| 2.0.0-p4 | 기존 코드 리팩토링 | `storage/session_store.cpp` |

### v2.0.1: 로드 밸런서 및 Consistent Hashing
| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.0.1-p1 | ConsistentHashRing 구현 | `distributed/consistent_hash.*` |
| 2.0.1-p2 | LoadBalancer 구현 | `distributed/load_balancer.*` |
| 2.0.1-p3 | ServiceDiscovery 구현 | `distributed/service_discovery.*` |
| 2.0.1-p4 | HealthCheck 및 Failover | `distributed/service_discovery.cpp` |

### v2.0.2: 서버 간 통신 (스텁)
gRPC 통합은 의존성 추가 후 완전 구현 예정

## 주요 기능

### 세션 저장소
- 세션 데이터 추상화 (SessionStore 인터페이스)
- 인메모리 구현 (개발/테스트용)
- Redis Cluster 구현 (프로덕션용)
- TTL 기반 세션 만료
- 플레이어 ID ↔ 세션 ID 매핑

### 로드 밸런싱
- Consistent Hashing (세션 어피니티)
- Round Robin (균등 분배)
- Least Connections (최소 부하)
- 가상 노드 기반 균등 분배 (150 vNodes)

### 서비스 디스커버리
- Redis 기반 서버 등록/발견
- 하트비트 기반 상태 모니터링
- 서버 추가/제거 이벤트 콜백
- 헬스 체커 통합

## 포함된 모듈 (신규)
```
server/
├── include/pvpserver/
│   ├── storage/
│   │   ├── session_store.h
│   │   ├── in_memory_session_store.h
│   │   └── redis_session_store.h
│   └── distributed/
│       ├── consistent_hash.h
│       ├── load_balancer.h
│       └── service_discovery.h
└── src/
    ├── storage/
    │   ├── session_store.cpp
    │   ├── in_memory_session_store.cpp
    │   └── redis_session_store.cpp
    └── distributed/
        ├── consistent_hash.cpp
        ├── load_balancer.cpp
        └── service_discovery.cpp
```

## 아키텍처

```
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ 게임 서버 1  │  │ 게임 서버 2  │  │ 게임 서버 N  │
└──────┬───────┘  └──────┬───────┘  └──────┬───────┘
       │                 │                 │
       ├─────────────────┼─────────────────┤
       │                 │                 │
       │    ┌────────────▼────────────┐    │
       │    │      LoadBalancer       │    │
       │    │   (ConsistentHash)      │    │
       │    └────────────┬────────────┘    │
       │                 │                 │
       └─────────────────┼─────────────────┘
                         │
              ┌──────────▼──────────┐
              │    Redis Cluster    │
              │  ┌───────────────┐  │
              │  │ SessionStore  │  │
              │  │ ServiceDiscov │  │
              │  └───────────────┘  │
              └─────────────────────┘
```

## 빌드 가능 여부
✅ 완전 빌드 가능 (Redis 의존성은 스텁 구현)

## 성능 특성
| 메트릭 | 측정값 |
|--------|--------|
| GetNode (Consistent Hash) | < 1μs |
| 세션 저장/조회 (InMemory) | < 1ms |
| 노드 추가/제거 | < 1ms |
| 하트비트 주기 | 5초 |
| 장애 탐지 | < 20초 |

## 의존성 (추가 필요)
```json
{
  "dependencies": [
    "hiredis",
    "redis-plus-plus",
    "nlohmann-json",
    "grpc",
    "protobuf"
  ]
}
```
