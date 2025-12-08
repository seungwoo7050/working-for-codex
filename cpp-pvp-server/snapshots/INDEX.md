# cpp-pvp-server 스냅샷 인덱스

> 이 문서는 CLONE-GUIDE.md의 패치 단위에 따라 생성된 스냅샷들을 정리합니다.
> **모든 스냅샷은 완전한 코드 기반**이며, 각 스냅샷의 `server/` 디렉토리에서 독립적으로 빌드 가능합니다.

## 스냅샷 개요

| 스냅샷 | 대응 버전 | 대응 패치 ID | 주요 기능 | 빌드 가능 |
|--------|----------|-------------|----------|-----------|
| [001_bootstrap](./001_bootstrap/) | v0.1.0 | 0.1.1~0.1.4 | CMake, vcpkg, 프로젝트 구조 | ✅ |
| [004_basic_server](./004_basic_server/) | v1.0.0 | 1.0.1~1.0.6 | WebSocket, GameLoop, 이동, PostgreSQL | ✅ |
| [005_combat](./005_combat/) | v1.1.0 | 1.1.1~1.1.5 | 발사체, 충돌, 데미지, 사망 | ✅ |
| [006_matchmaking](./006_matchmaking/) | v1.2.0 | 1.2.1~1.2.5 | ELO 매칭, 매치 큐, 알림 | ✅ |
| [007_stats](./007_stats/) | v1.3.0 | 1.3.1~1.3.4 | 통계, ELO 등급, 리더보드, Prometheus | ✅ |
| [008_udp_netcode](./008_udp_netcode/) | v1.4.0 | 1.4.0-p1~p3 | UDP 소켓, 권위 서버, 스냅샷/델타 | ✅ |
| [009_prediction](./009_prediction/) | v1.4.1 | 1.4.1-p1~p4 | 클라이언트 예측, 리컨실리에이션, 지연 보상 | ✅ |
| [010_observability](./010_observability/) | v1.4.2 | 1.4.2-p1~p4 | UDP 메트릭, 패킷 시뮬레이터, 부하 테스트 | ✅ |
| [011_distributed](./011_distributed/) | v2.0.0~v2.0.2 | 2.0.0~2.0.2 | 세션 저장소, 로드 밸런서, 서비스 디스커버리 | ✅ |
| [012_anticheat](./012_anticheat/) | v2.1.0~v2.1.4 | 2.1.0~2.1.4 | 히트/이동 검증, 이상 탐지, 리플레이, 밴 시스템 | ✅ |

---

## 스냅샷 빌드 방법

각 스냅샷은 독립적으로 빌드할 수 있습니다:

```bash
cd snapshots/<snapshot_name>/server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
ctest --output-on-failure
```

---

## 스냅샷 상세 설명

### 001_bootstrap (v0.1.0)

**패치 범위:**
- 0.1.1: CMakeLists.txt 설정
- 0.1.2: vcpkg.json 의존성 정의
- 0.1.3: 디렉토리 구조 생성
- 0.1.4: 빌드 스크립트

**주요 기능:**
- C++17 CMake 프로젝트 초기화
- vcpkg 의존성 관리 (boost-system, boost-asio, boost-beast, gtest)
- "Hello cpp-pvp-server" 출력 프로그램

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 004_basic_server (v1.0.0)

**패치 범위:**
- 1.0.1: WebSocket 서버 구현 (Boost.Beast)
- 1.0.2: 연결 관리자 (세션 추적)
- 1.0.3: GameLoop (60 TPS 고정 틱레이트)
- 1.0.4: GameSession (플레이어 상태 관리)
- 1.0.5: 플레이어 이동 처리 (WASD, 정규화)
- 1.0.6: PostgreSQL 연동 (세션 이벤트)

**주요 기능:**
- 2명 동시 접속 가능한 WebSocket 서버
- 60 TPS 게임 루프 (±1ms 편차)
- WASD 이동 + 마우스 에임
- PostgreSQL 세션 이벤트 기록

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 005_combat (v1.1.0)

**패치 범위:**
- 1.1.1: Combat 컴포넌트 (HealthComponent)
- 1.1.2: Projectile 시스템 (30 m/s, 1.5초 수명)
- 1.1.3: Collision 검출 (원-원 교차)
- 1.1.4: Damage 계산 (히트당 20 HP)
- 1.1.5: HP/사망 처리

**주요 기능:**
- fire 입력으로 발사체 생성
- 서버 권위 충돌 검출
- 100 HP 시작, 사망 시 death 이벤트 브로드캐스트

**빌드 가능 여부:** ✅ 완전 빌드 가능 (코드 기반 스냅샷)

---

### 006_matchmaking (v1.2.0)

**패치 범위:**
- 1.2.1: MatchmakingService
- 1.2.2: Redis 큐 연동 (InMemoryMatchQueue)
- 1.2.3: ELO 계산 (동적 허용 오차)
- 1.2.4: 매치 생성 로직
- 1.2.5: 결과 기록

**주요 기능:**
- ELO 기반 매칭 (±100 기본, 5초마다 ±25 확장)
- InMemoryMatchQueue (Redis 스텁 포함)
- MatchNotificationChannel (이벤트 발행)

**빌드 가능 여부:** ✅ 완전 빌드 가능 (코드 기반 스냅샷)

---

### 007_stats (v1.3.0)

**패치 범위:**
- 1.3.1: 통계 집계 서비스 (MatchStats, PlayerProfileService)
- 1.3.2: 랭킹 API (LeaderboardStore)
- 1.3.3: Prometheus 메트릭 (MetricsHttpServer)
- 1.3.4: Grafana 대시보드

**주요 기능:**
- 매치 후 통계 수집 (샷, 히트, 정확도, 데미지, 킬, 데스)
- ELO 등급 조정 (K-factor 25)
- 전역 리더보드 (등급별 정렬, Top N 쿼리)
- HTTP API (프로필, 리더보드, 메트릭)

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 008_udp_netcode (v1.4.0)

**패치 범위:**
- 1.4.0-p1: UDP 소켓 래퍼 (Boost.Asio)
- 1.4.0-p2: 권위 서버 모델 (UDP 게임 서버)
- 1.4.0-p3: 스냅샷/델타 압축 (상태 동기화)

**주요 기능:**
- UDP 소켓 (비동기 수신/전송)
- 패킷 타입 (CONNECT, INPUT, STATE, EVENT)
- 클라이언트 연결 관리 (타임아웃)
- 스냅샷 생성 및 델타 계산
- 60 TPS 상태 브로드캐스트

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 009_prediction (v1.4.1)

**패치 범위:**
- 1.4.1-p1: 클라이언트 예측 엔진
- 1.4.1-p2: 리컨실리에이션 시스템
- 1.4.1-p3: 입력 버퍼링
- 1.4.1-p4: 지연 보상

**주요 기능:**
- 클라이언트 측 입력 예측 (즉각적인 반응)
- 서버 상태 불일치 시 부드러운 보정
- 서버 측 입력 버퍼링 (지터 완화)
- 지연 보상된 히트 판정 (과거 상태 조회)

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 010_observability (v1.4.2)

**패치 범위:**
- 1.4.2-p1: UDP 메트릭 수집
- 1.4.2-p2: 패킷 시뮬레이터
- 1.4.2-p3: 부하 테스트
- 1.4.2-p4: Grafana 대시보드

**주요 기능:**
- UDP 연결 품질 메트릭 (RTT, 지터, 패킷 손실률)
- 클라이언트별 메트릭 추적
- Prometheus 형식 출력
- 네트워크 조건 시뮬레이션 (테스트용)
- 성능 벤치마크 테스트

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 011_distributed (v2.0.0~v2.0.2)

**패치 범위:**
- 2.0.0: 세션 서버 분리
- 2.0.1: 로드 밸런서/Consistent Hashing
- 2.0.2: 서비스 디스커버리/gRPC

**주요 기능:**
- ISessionStore 인터페이스 (InMemory, Redis)
- ConsistentHashRing (MurmurHash3, 150 가상 노드)
- LoadBalancer (세션 어피니티, 헬스 체크)
- ServiceDiscovery (Redis pub/sub 기반)

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

### 012_anticheat (v2.1.0~v2.1.4)

**패치 범위:**
- 2.1.0: 서버 권위적 히트 검증
- 2.1.1: 이동 속도/월핵 탐지
- 2.1.2: 이상 통계 탐지
- 2.1.3: 리플레이 시스템
- 2.1.4: 밴 시스템

**주요 기능:**
- HitValidator (레이캐스트, 지연 보상)
- MovementValidator (속도핵, 텔레포트, 월핵)
- AnomalyDetector (Z-Score 기반 통계 분석)
- SuspicionSystem (의심 레벨 관리)
- ReplayRecorder (게임 상태 녹화)
- BanService (플레이어/HWID/IP 밴)

**빌드 가능 여부:** ✅ 완전 빌드 가능

---

## 스냅샷 생성 원칙

1. **점진적 누적**: 각 스냅샷은 이전 스냅샷의 모든 기능을 포함
2. **독립 빌드**: 가능한 경우 각 스냅샷은 독립적으로 빌드 가능
3. **코드 제거 방식**: 최종 코드에서 해당 시점에 없는 기능을 제거하여 생성
4. **테스트 포함**: 해당 시점에 적합한 테스트 코드 포함

---

## 빌드 방법 (공통)

```bash
cd snapshots/<스냅샷_디렉토리>/server
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

---

## 버전별 파일 구조 변화

### v0.1.0 → v1.0.0
추가된 파일:
- `src/core/config.cpp`
- `src/core/game_loop.cpp`
- `src/game/game_session.cpp`
- `src/network/websocket_server.cpp`
- `src/storage/postgres_storage.cpp`

### v1.0.0 → v1.1.0
추가된 파일:
- `src/game/combat.cpp`
- `src/game/projectile.cpp`
- `include/pvpserver/game/combat.h`
- `include/pvpserver/game/projectile.h`

변경된 파일:
- `MovementInput`에 `fire` 필드 추가
- `GameSession`에 전투 로직 추가
- `WebSocketServer`에 death 이벤트 브로드캐스트 추가

### v1.1.0 → v1.2.0
추가된 파일:
- `src/matchmaking/*.cpp`
- `include/pvpserver/matchmaking/*.h`

### v1.2.0 → v1.3.0
추가된 파일:
- `src/stats/*.cpp`
- `src/network/metrics_http_server.cpp`
- `src/network/profile_http_router.cpp`
- `include/pvpserver/stats/*.h`
- `include/pvpserver/network/metrics_http_server.h`
- `include/pvpserver/network/profile_http_router.h`

### v1.3.0 → v1.4.0
추가된 파일:
- `src/network/udp_socket.cpp`
- `src/network/packet_types.cpp`
- `src/network/udp_game_server.cpp`
- `src/network/snapshot_manager.cpp`
- `include/pvpserver/network/udp_socket.h`
- `include/pvpserver/network/packet_types.h`
- `include/pvpserver/network/udp_game_server.h`
- `include/pvpserver/network/snapshot_manager.h`

### v1.4.0 → v1.4.1
추가된 파일:
- `src/netcode/client_prediction.cpp`
- `src/netcode/reconciliation.cpp`
- `src/netcode/input_buffer.cpp`
- `src/netcode/lag_compensation.cpp`
- `include/pvpserver/netcode/client_prediction.h`
- `include/pvpserver/netcode/reconciliation.h`
- `include/pvpserver/netcode/input_buffer.h`
- `include/pvpserver/netcode/lag_compensation.h`

### v1.4.1 → v1.4.2
추가된 파일:
- `src/network/udp_metrics.cpp`
- `src/network/packet_simulator.cpp`
- `include/pvpserver/network/udp_metrics.h`
- `include/pvpserver/network/packet_simulator.h`
- `tests/performance/udp_load_test.cpp`
- `monitoring/grafana/dashboards/udp-dashboard.json`

### v1.4.2 → v2.0.0~v2.0.2
추가된 파일:
- `src/distributed/consistent_hash.cpp`
- `src/distributed/load_balancer.cpp`
- `src/distributed/service_discovery.cpp`
- `src/storage/session_store.cpp`
- `src/storage/in_memory_session_store.cpp`
- `src/storage/redis_session_store.cpp`
- `include/pvpserver/distributed/consistent_hash.h`
- `include/pvpserver/distributed/load_balancer.h`
- `include/pvpserver/distributed/service_discovery.h`
- `include/pvpserver/storage/session_store.h`
- `include/pvpserver/storage/in_memory_session_store.h`
- `include/pvpserver/storage/redis_session_store.h`
- `design/v2.0.0-session-store.md`
- `design/v2.0.1-load-balancer.md`
- `design/v2.0.2-grpc.md`

### v2.0.2 → v2.1.0~v2.1.4
추가된 파일:
- `src/anticheat/hit_validator.cpp`
- `src/anticheat/movement_validator.cpp`
- `src/anticheat/anomaly_detector.cpp`
- `src/anticheat/replay_recorder.cpp`
- `src/anticheat/ban_service.cpp`
- `include/pvpserver/anticheat/hit_validator.h`
- `include/pvpserver/anticheat/movement_validator.h`
- `include/pvpserver/anticheat/anomaly_detector.h`
- `include/pvpserver/anticheat/replay_recorder.h`
- `include/pvpserver/anticheat/ban_service.h`
- `tests/unit/test_hit_validator.cpp`
- `tests/unit/test_movement_validator.cpp`
- `tests/unit/test_anomaly_detector.cpp`
- `tests/unit/test_ban_service.cpp`
- `design/v2.1-anticheat.md`

---

## 참고 문서

- [CLONE-GUIDE.md](../CLONE-GUIDE.md) - 패치 단위 가이드
- [design/](../design/) - 설계 문서
  - [0.0-initial-design.md](../design/0.0-initial-design.md) - 초기 설계
  - [v1.0.0-game-server.md](../design/v1.0.0-game-server.md) - 기본 게임 서버
  - [v1.1.0-combat.md](../design/v1.1.0-combat.md) - 전투 시스템
  - [v1.2.0-matchmaking.md](../design/v1.2.0-matchmaking.md) - 매치메이킹
  - [v1.3.0-stats.md](../design/v1.3.0-stats.md) - 통계 & 랭킹
  - [v1.4.0-udp-netcode.md](../design/v1.4.0-udp-netcode.md) - UDP 넷코드 코어
  - [v1.4.1-prediction.md](../design/v1.4.1-prediction.md) - 예측 & 리컨실리에이션
  - [v1.4.2-observability.md](../design/v1.4.2-observability.md) - 관찰 가능성
  - [v2.0.0-session-store.md](../design/v2.0.0-session-store.md) - 세션 저장소
  - [v2.0.1-load-balancer.md](../design/v2.0.1-load-balancer.md) - 로드 밸런서
  - [v2.0.2-grpc.md](../design/v2.0.2-grpc.md) - gRPC 통신
  - [v2.1-anticheat.md](../design/v2.1-anticheat.md) - 안티 치트 시스템
