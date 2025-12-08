# 📕 cpp-pvp-server (cpp-pvp-server/) 클론코딩 가이드

> 1v1 듀얼 게임 서버 (Checkpoint A) - C++ 비동기 네트워크 프로그래밍

## v0.1.0: Bootstrap & Initial Design (0.5일)

> 📅 **권장 기간**: 2025년 11월 1일 ~ 11월 2일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/01-cpp17-server-ffmpeg-minimum-basics.md
│   └── C++17 기본 문법, 컴파일러
├── prerequisite/c++/03-cpp17-cmake-project-structure-debugging.md
│   └── CMake, vcpkg, 프로젝트 구조
├── prerequisite/game-server/01-game-server-architecture-and-tcp-basics.md
│   └── 게임 서버 기초 개념
└── prerequisite/server-basic/01-process-vs-thread.md
    └── 프로세스/스레드, 동시성
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 0.1.1 | CMakeLists.txt 설정 | `design/0.0-initial-design.md` | `server/CMakeLists.txt` |
| 0.1.2 | vcpkg.json 의존성 | 〃 | `server/vcpkg.json` |
| 0.1.3 | 디렉토리 구조 생성 | 〃 | `server/src/`, `server/include/` |
| 0.1.4 | 빌드 스크립트 작성 | 〃 | `scripts/build.sh` |

### 🔖 커밋 포인트
```bash
git commit -m "chore: initialize cpp-pvp-server project with CMake and vcpkg"
# v0.1.0 tag
```

### ✅ 완료 기준
- [ ] CMake 빌드 성공
- [ ] vcpkg 의존성 설치 (boost, spdlog, protobuf)
- [ ] main.cpp "Hello cpp-pvp-server" 출력

---

## v1.0.0: Basic Game Server (3일)

> 📅 **권장 기간**: 2025년 11월 3일 ~ 11월 9일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/c++/05-cpp17-asio-networking-fundamentals.md
│   └── boost.asio, io_context, async 패턴 ⭐
├── prerequisite/c++/06-cpp17-websocket-beast-implementation.md
│   └── boost.beast, WebSocket 구현 ⭐
└── prerequisite/game-server/02-epoll-event-loop-session-room-tick-loop.md
    └── 게임 루프, 고정 틱레이트
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.0.1 | WebSocket 서버 구현 | `cpp-pvp-server/design/v1.0.0-game-server.md` | `server/src/network/ws_server.cpp` |
| 1.0.2 | 연결 관리자 | 〃 | `server/src/network/connection_manager.cpp` |
| 1.0.3 | GameLoop (60 TPS) | 〃 | `server/src/core/game_loop.cpp` |
| 1.0.4 | GameSession 관리 | 〃 | `server/src/game/game_session.cpp` |
| 1.0.5 | 플레이어 이동 처리 | 〃 | `server/src/game/player.cpp` |
| 1.0.6 | PostgreSQL 연동 | 〃 | `server/src/storage/postgres_client.cpp` |

### 🔖 커밋 포인트
```bash
# 1단계: 네트워크 기반
git commit -m "feat(network): implement WebSocket server with boost.beast"
git commit -m "feat(network): add connection manager with session tracking"

# 2단계: 게임 루프
git commit -m "feat(core): implement 60 TPS game loop with fixed timestep"
git commit -m "feat(game): add GameSession and player state management"

# 3단계: 데이터 레이어
git commit -m "feat(storage): add PostgreSQL integration for player data"
git commit -m "test: add integration tests for basic server functionality"
# v1.0.0 tag
```

### ✅ 완료 기준
- [ ] 2명 동시 접속 가능
- [ ] 60 TPS 고정 틱레이트 유지
- [ ] WASD 이동 동기화
- [ ] 플레이어 데이터 저장/로드

---

## v1.1.0: Combat System (2.5일)

> 📅 **권장 기간**: 2025년 11월 10일 ~ 11월 14일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/graphics-math/
│   └── AABB, 충돌 검출 알고리즘
└── prerequisite/c++/02-cpp17-memory-raii-error-logging.md
    └── RAII, 메모리 관리 (투사체 풀링)
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.1.1 | Combat 컴포넌트 | `cpp-pvp-server/design/v1.1.0-combat.md` | `server/src/game/combat.cpp` |
| 1.1.2 | Projectile 시스템 | 〃 | `server/src/game/projectile.cpp` |
| 1.1.3 | Collision 검출 | 〃 | `server/src/game/collision.cpp` |
| 1.1.4 | Damage 계산 | 〃 | `server/src/game/damage.cpp` |
| 1.1.5 | HP/사망 처리 | 〃 | `server/src/game/health.cpp` |

### 🔖 커밋 포인트
```bash
git commit -m "feat(combat): implement projectile spawning and trajectory"
git commit -m "feat(combat): add AABB collision detection"
git commit -m "feat(combat): implement damage calculation and health system"
git commit -m "feat(combat): add player death and respawn logic"
# v1.1.0 tag
```

### ✅ 완료 기준
- [ ] 스페이스바 → 투사체 발사
- [ ] 투사체 충돌 검출 정확
- [ ] HP 0 → 사망 → 3초 후 리스폰

---

## v1.2.0: Matchmaking (2.5일)

> 📅 **권장 기간**: 2025년 11월 15일 ~ 11월 19일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── prerequisite/algo/
│   └── ELO 레이팅, 매칭 알고리즘
└── prerequisite/server-basic/06-database-connection-pool.md
    └── Redis, 큐 시스템
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.2.1 | MatchmakingService | `cpp-pvp-server/design/v1.2.0-matchmaking.md` | `server/src/matchmaking/matchmaker.cpp` |
| 1.2.2 | Redis 큐 연동 | 〃 | `server/src/storage/redis_client.cpp` |
| 1.2.3 | ELO 계산 | 〃 | `server/src/matchmaking/elo.cpp` |
| 1.2.4 | 매치 생성 로직 | 〃 | `server/src/matchmaking/match_creator.cpp` |
| 1.2.5 | 결과 기록 | 〃 | `server/src/storage/match_history.cpp` |

### 🔖 커밋 포인트
```bash
git commit -m "feat(matchmaking): implement Redis-based matchmaking queue"
git commit -m "feat(matchmaking): add ELO rating calculation"
git commit -m "feat(matchmaking): implement match creation and assignment"
git commit -m "feat(storage): persist match results and update ELO"
# v1.2.0 tag
```

### ✅ 완료 기준
- [ ] 대기열 참가 → 매칭 완료 < 30초
- [ ] 비슷한 ELO끼리 매칭
- [ ] 매치 결과 ELO 반영

---

## v1.3.0: Statistics & Ranking (2일)

> 📅 **권장 기간**: 2025년 11월 20일 ~ 11월 25일

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.3.1 | 통계 집계 서비스 | `cpp-pvp-server/design/v1.3.0-stats.md` | `server/src/stats/stats_service.cpp` |
| 1.3.2 | 랭킹 API | 〃 | `server/src/stats/ranking.cpp` |
| 1.3.3 | Prometheus 메트릭 | 〃 | `server/src/monitoring/metrics.cpp` |
| 1.3.4 | Grafana 대시보드 | 〃 | `monitoring/grafana/` |

### 🔖 커밋 포인트
```bash
git commit -m "feat(stats): implement player statistics aggregation"
git commit -m "feat(stats): add leaderboard ranking system"
git commit -m "feat(monitoring): integrate Prometheus metrics"
git commit -m "feat(monitoring): add Grafana dashboards"
# v1.3.0 tag
```

### ✅ 완료 기준
- [ ] 전적 조회 API 동작
- [ ] TOP 100 랭킹 조회
- [ ] Grafana 대시보드 표시

---

## v1.4.0: UDP 권위 서버 코어 (2주)

> 📅 **권장 기간**: 2025년 11월 26일 ~ 12월 9일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── udp-game-sync/lab1.1-tcp-echo/
│   └── TCP/UDP 소켓 기본, Boost.Asio 비동기 패턴
├── udp-game-sync/netcode-core/v1.0/
│   └── 권위 서버 모델, 서버-클라이언트 아키텍처
└── udp-game-sync/netcode-core/v1.1/
    └── 스냅샷/델타 동기화, 상태 압축
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.4.0-p1 | UDP 소켓 래퍼 구현 | `cpp-pvp-server/design/v1.4.0-udp-netcode.md` | `server/src/network/udp_socket.cpp` |
| 1.4.0-p2 | UDP 게임 서버 코어 | 〃 | `server/src/network/udp_game_server.cpp` |
| 1.4.0-p3 | 스냅샷/델타 동기화 | 〃 | `server/src/network/snapshot_manager.cpp` |
| 1.4.0-p4 | 기존 WebSocket 교체 | 〃 | `server/src/main.cpp` 수정 |

### 🔖 커밋 포인트
```bash
git commit -m "feat(udp): implement UDP socket wrapper with Boost.Asio"
git commit -m "feat(udp): add UDP game server core with authority model"
git commit -m "feat(udp): implement snapshot-delta synchronization"
git commit -m "refactor(udp): replace WebSocket with UDP in main server loop"
git tag -a v1.4.0 -m "v1.4.0: UDP Netcode Core"
```

### ✅ 완료 기준
- [ ] UDP 소켓으로 패킷 송수신 가능
- [ ] 권위 서버 모델로 게임 상태 관리
- [ ] 60 TPS 스냅샷 전송
- [ ] 델타 압축으로 대역폭 최적화

---

## v1.4.1: 클라이언트 예측/리컨실리에이션 (2주)

> 📅 **권장 기간**: 2025년 12월 10일 ~ 12월 23일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── udp-game-sync/netcode-core/v1.1/
│   └── 클라이언트 예측, 서버 리컨실리에이션
└── 게임 넷코드 관련 GDC 발표
    └── Overwatch GDC, Rocket League GDC
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.4.1-p1 | 클라이언트 예측 엔진 | `cpp-pvp-server/design/v1.4.1-prediction.md` | `server/src/netcode/client_prediction.cpp` |
| 1.4.1-p2 | 리컨실리에이션 시스템 | 〃 | `server/src/netcode/reconciliation.cpp` |
| 1.4.1-p3 | 입력 버퍼링 | 〃 | `server/src/netcode/input_buffer.cpp` |
| 1.4.1-p4 | 지연 보상 | 〃 | `server/src/netcode/lag_compensation.cpp` |

### 🔖 커밋 포인트
```bash
git commit -m "feat(prediction): implement client-side prediction engine"
git commit -m "feat(recon): add server reconciliation with state rewind"
git commit -m "feat(input): add input buffering for consistent simulation"
git commit -m "feat(lag): implement lag compensation for fair gameplay"
git tag -a v1.4.1 -m "v1.4.1: Client Prediction & Reconciliation"
```

### ✅ 완료 기준
- [ ] 클라이언트 예측으로 즉각적인 입력 반응
- [ ] 서버 상태 불일치 시 클라이언트 보정
- [ ] 네트워크 지연에도 일관된 게임 플레이
- [ ] 지연 보상으로 공정한 히트 판정

---

## v1.4.2: 관측성 & 부하 테스트 (2주)

> 📅 **권장 기간**: 2025년 12월 24일 ~ 2026년 1월 6일

### 📖 사전 학습
```
📚 읽어야 할 Prerequisite
├── udp-game-sync/netcode-core/v1.2/
│   └── 메트릭, 부하 테스트, 시뮬레이션
└── prerequisite/server-basic/
    └── Prometheus, Grafana 모니터링
```

### 📋 작업 내용
| 순서 | 작업 | Design 문서 | 핵심 파일 |
|------|------|-------------|----------|
| 1.4.2-p1 | UDP 메트릭 추가 | `cpp-pvp-server/design/v1.4.2-observability.md` | `server/src/network/udp_metrics.cpp` |
| 1.4.2-p2 | 패킷 손실 시뮬레이션 | 〃 | `server/src/network/packet_simulator.cpp` |
| 1.4.2-p3 | 재현 가능한 부하 테스트 | 〃 | `server/tests/performance/udp_load_test.cpp` |
| 1.4.2-p4 | Grafana 대시보드 | 〃 | `monitoring/grafana/dashboards/udp-dashboard.json` |

### 🔖 커밋 포인트
```bash
git commit -m "feat(metrics): add UDP-specific Prometheus metrics"
git commit -m "feat(sim): implement packet loss and latency simulation"
git commit -m "test(load): add reproducible UDP load testing framework"
git commit -m "feat(monitoring): create Grafana dashboard for UDP performance"
git tag -a v1.4.2 -m "v1.4.2: Observability & Load Testing"
```

### ✅ 완료 기준
- [ ] 패킷 손실률, RTT 메트릭 수집
- [ ] 네트워크 조건 시뮬레이션 가능
- [ ] 다중 클라이언트 부하 테스트
- [ ] Grafana에서 UDP 성능 모니터링

---

## Major 2: 분산 서버 아키텍처

> 수평 확장이 가능한 분산 게임 서버 구축

---

### v2.0.0: 세션 서버 분리 및 Redis 통합

**선행조건:** v1.4.2 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.0.0-p1 | SessionStore 인터페이스 정의 | `storage/session_store.h` |
| 2.0.0-p2 | InMemorySessionStore 구현 | `storage/in_memory_session_store.*` |
| 2.0.0-p3 | RedisSessionStore 구현 | `storage/redis_session_store.*` |
| 2.0.0-p4 | 기존 코드 리팩토링 | `game/game_session.cpp`, `network/` |

**2.0.0-p1: SessionStore 인터페이스 정의**

SessionStore 추상 인터페이스:
```cpp
class SessionStore {
public:
    virtual ~SessionStore() = default;

    // 세션 저장
    virtual bool SaveSession(const std::string& session_id, const SessionData& data) = 0;

    // 세션 조회
    virtual std::optional<SessionData> GetSession(const std::string& session_id) = 0;

    // 세션 삭제
    virtual bool DeleteSession(const std::string& session_id) = 0;

    // 세션 갱신 (TTL 연장)
    virtual bool RefreshSession(const std::string& session_id) = 0;

    // 모든 세션 키 조회
    virtual std::vector<std::string> GetAllSessionIds() = 0;
};
```

SessionData 구조체:
```cpp
struct SessionData {
    std::string player_id;
    std::string player_name;
    std::string server_id;        // 할당된 게임 서버
    int64_t created_at;           // 생성 타임스탬프
    int64_t last_activity;        // 마지막 활동
    int elo_rating;
    // 직렬화 메서드
    std::string Serialize() const;
    static std::optional<SessionData> Deserialize(const std::string& data);
};
```

**2.0.0-p2: InMemorySessionStore 구현**

```cpp
class InMemorySessionStore : public SessionStore {
public:
    explicit InMemorySessionStore(int ttl_seconds = 3600);
    // SessionStore 인터페이스 구현
    // + 만료 세션 정리 백그라운드 스레드
private:
    std::unordered_map<std::string, SessionData> sessions_;
    std::unordered_map<std::string, int64_t> expiry_times_;
    std::mutex mutex_;
    int ttl_seconds_;
};
```

**2.0.0-p3: RedisSessionStore 구현**

Redis 연결 설정:
```cpp
struct RedisConfig {
    std::vector<std::string> cluster_nodes;  // "host:port" 형태
    std::string password;
    int connection_pool_size = 10;
    int connection_timeout_ms = 5000;
    int command_timeout_ms = 1000;
};

class RedisSessionStore : public SessionStore {
public:
    explicit RedisSessionStore(const RedisConfig& config);
    // SessionStore 인터페이스 구현
    // Redis Cluster 지원, 연결 풀링
private:
    std::unique_ptr<RedisClusterClient> client_;
    int session_ttl_seconds_;
};
```

**2.0.0-p4: 기존 코드 리팩토링**

GameSession에서 SessionStore 사용:
```cpp
class GameSession {
public:
    GameSession(std::shared_ptr<SessionStore> session_store);
    // 세션 생성/조회/갱신 시 store 사용
private:
    std::shared_ptr<SessionStore> session_store_;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(storage): define SessionStore interface"
git commit -m "feat(storage): implement InMemorySessionStore"
git commit -m "feat(storage): implement RedisSessionStore"
git commit -m "refactor(game): use SessionStore abstraction"
git tag -a v2.0.0 -m "v2.0.0: Session Server Separation"
```

### ✅ 완료 기준
- [ ] InMemorySessionStore 단위 테스트 통과
- [ ] RedisSessionStore 통합 테스트 통과
- [ ] 세션 TTL 만료 동작 확인

---

### v2.0.1: 로드 밸런서 및 Consistent Hashing

**선행조건:** v2.0.0 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.0.1-p1 | ConsistentHashRing 구현 | `distributed/consistent_hash.*` |
| 2.0.1-p2 | LoadBalancer 구현 | `distributed/load_balancer.*` |
| 2.0.1-p3 | ServiceDiscovery 구현 | `distributed/service_discovery.*` |
| 2.0.1-p4 | HealthCheck 및 Failover | `distributed/health_check.*` |

**2.0.1-p1: ConsistentHashRing 구현**

```cpp
class ConsistentHashRing {
public:
    explicit ConsistentHashRing(int virtual_nodes = 150);

    // 노드 추가/제거
    void AddNode(const std::string& node_id);
    void RemoveNode(const std::string& node_id);

    // 키에 대한 노드 조회
    std::string GetNode(const std::string& key) const;

    // N개의 후보 노드 조회 (복제 또는 페일오버용)
    std::vector<std::string> GetNodes(const std::string& key, int n) const;

    // 현재 노드 수
    size_t GetNodeCount() const;

private:
    int virtual_nodes_;
    std::map<size_t, std::string> ring_;
    std::set<std::string> nodes_;
    std::hash<std::string> hasher_;

    size_t Hash(const std::string& key) const;
};
```

**2.0.1-p2: LoadBalancer 구현**

```cpp
enum class LoadBalanceStrategy {
    CONSISTENT_HASH,    // 플레이어 ID 기반 고정 할당
    ROUND_ROBIN,        // 순환
    LEAST_CONNECTIONS   // 최소 연결
};

class LoadBalancer {
public:
    LoadBalancer(LoadBalanceStrategy strategy = LoadBalanceStrategy::CONSISTENT_HASH);

    // 서버 등록/해제
    void RegisterServer(const ServerInfo& server);
    void UnregisterServer(const std::string& server_id);

    // 서버 선택
    std::optional<ServerInfo> SelectServer(const std::string& player_id);

    // 서버 상태 업데이트
    void UpdateServerLoad(const std::string& server_id, int connections);

private:
    LoadBalanceStrategy strategy_;
    ConsistentHashRing hash_ring_;
    std::unordered_map<std::string, ServerInfo> servers_;
    std::atomic<size_t> round_robin_index_{0};
};
```

**2.0.1-p3: ServiceDiscovery 구현**

```cpp
struct ServerInfo {
    std::string server_id;
    std::string host;
    int udp_port;
    int grpc_port;
    int current_connections;
    int max_connections;
    int64_t last_heartbeat;
    bool healthy;
};

class ServiceDiscovery {
public:
    explicit ServiceDiscovery(std::shared_ptr<RedisClient> redis);

    // 서버 등록 (하트비트 시작)
    void Register(const ServerInfo& self);

    // 서버 해제
    void Unregister();

    // 사용 가능한 서버 목록 조회
    std::vector<ServerInfo> GetAvailableServers();

    // 서버 변경 콜백
    void OnServerChange(std::function<void(const std::vector<ServerInfo>&)> callback);

private:
    std::shared_ptr<RedisClient> redis_;
    ServerInfo self_;
    std::thread heartbeat_thread_;
    bool running_;
};
```

**2.0.1-p4: HealthCheck 및 Failover**

```cpp
class HealthChecker {
public:
    HealthChecker(std::shared_ptr<ServiceDiscovery> discovery,
                  std::chrono::seconds check_interval = std::chrono::seconds(5));

    void Start();
    void Stop();

    // 서버 상태 조회
    bool IsServerHealthy(const std::string& server_id) const;

    // 비정상 서버 콜백
    void OnServerUnhealthy(std::function<void(const std::string&)> callback);

private:
    std::shared_ptr<ServiceDiscovery> discovery_;
    std::chrono::seconds check_interval_;
    std::thread check_thread_;
    bool running_;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(distributed): implement ConsistentHashRing"
git commit -m "feat(distributed): implement LoadBalancer"
git commit -m "feat(distributed): implement ServiceDiscovery"
git commit -m "feat(distributed): add HealthChecker"
git tag -a v2.0.1 -m "v2.0.1: Load Balancer & Service Discovery"
```

### ✅ 완료 기준
- [ ] ConsistentHashRing 분배 균일성 테스트
- [ ] 노드 추가/제거 시 최소 재할당 확인
- [ ] ServiceDiscovery 하트비트 동작 확인

---

### v2.0.2: 서버 간 통신 및 gRPC

**선행조건:** v2.0.1 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.0.2-p1 | gRPC 서비스 정의 | `proto/game_server.proto` |
| 2.0.2-p2 | MatchmakingService 구현 | `grpc/matchmaking_service.*` |
| 2.0.2-p3 | GameServerService 구현 | `grpc/game_server_service.*` |
| 2.0.2-p4 | EventBroadcaster 구현 | `distributed/event_broadcaster.*` |

**2.0.2-p1: gRPC 서비스 정의**

game_server.proto:
```protobuf
syntax = "proto3";
package pvpserver;

service MatchmakingService {
    rpc RequestMatch(MatchRequest) returns (MatchResponse);
    rpc CancelMatch(CancelMatchRequest) returns (CancelMatchResponse);
    rpc GetMatchStatus(MatchStatusRequest) returns (MatchStatusResponse);
}

service GameServerService {
    rpc GetServerStatus(ServerStatusRequest) returns (ServerStatusResponse);
    rpc TransferPlayer(TransferPlayerRequest) returns (TransferPlayerResponse);
    rpc BroadcastEvent(EventRequest) returns (EventResponse);
}

message MatchRequest {
    string player_id = 1;
    int32 elo_rating = 2;
}

message MatchResponse {
    bool success = 1;
    string match_id = 2;
    string server_address = 3;
    int32 udp_port = 4;
}
```

**2.0.2-p2: MatchmakingService 구현**

```cpp
class MatchmakingServiceImpl final : public MatchmakingService::Service {
public:
    explicit MatchmakingServiceImpl(std::shared_ptr<Matchmaker> matchmaker,
                                    std::shared_ptr<LoadBalancer> balancer);

    grpc::Status RequestMatch(grpc::ServerContext* context,
                             const MatchRequest* request,
                             MatchResponse* response) override;

    grpc::Status CancelMatch(grpc::ServerContext* context,
                            const CancelMatchRequest* request,
                            CancelMatchResponse* response) override;

private:
    std::shared_ptr<Matchmaker> matchmaker_;
    std::shared_ptr<LoadBalancer> balancer_;
};
```

**2.0.2-p3: GameServerService 구현**

```cpp
class GameServerServiceImpl final : public GameServerService::Service {
public:
    explicit GameServerServiceImpl(std::shared_ptr<UdpGameServer> game_server);

    grpc::Status GetServerStatus(grpc::ServerContext* context,
                                const ServerStatusRequest* request,
                                ServerStatusResponse* response) override;

    grpc::Status TransferPlayer(grpc::ServerContext* context,
                               const TransferPlayerRequest* request,
                               TransferPlayerResponse* response) override;

private:
    std::shared_ptr<UdpGameServer> game_server_;
};
```

**2.0.2-p4: EventBroadcaster 구현**

```cpp
class EventBroadcaster {
public:
    explicit EventBroadcaster(std::shared_ptr<RedisClient> redis);

    // 이벤트 발행
    void Publish(const std::string& channel, const GameEvent& event);

    // 이벤트 구독
    void Subscribe(const std::string& channel,
                  std::function<void(const GameEvent&)> handler);

    // 글로벌 이벤트 (모든 서버)
    void BroadcastGlobal(const GameEvent& event);

private:
    std::shared_ptr<RedisClient> redis_;
    std::unordered_map<std::string, std::function<void(const GameEvent&)>> handlers_;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(proto): define gRPC service interfaces"
git commit -m "feat(grpc): implement MatchmakingService"
git commit -m "feat(grpc): implement GameServerService"
git commit -m "feat(distributed): implement EventBroadcaster"
git tag -a v2.0.2 -m "v2.0.2: Inter-Server Communication"
```

### ✅ 완료 기준
- [ ] gRPC 서비스 빌드 성공
- [ ] MatchmakingService 요청/응답 테스트
- [ ] EventBroadcaster pub/sub 동작 확인

---

## Minor 2.1: 안티 치트 시스템

> 서버 권위 모델 강화 및 이상 행동 탐지

---

### v2.1.0: 서버 권위적 히트 검증

**선행조건:** v2.0.2 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.1.0-p1 | HitValidator 인터페이스 정의 | `anticheat/hit_validator.h` |
| 2.1.0-p2 | RaycastSystem 구현 | `anticheat/raycast_system.*` |
| 2.1.0-p3 | WorldStateBuffer 구현 | `anticheat/world_state_buffer.*` |
| 2.1.0-p4 | 히트 검증 통합 | `game/combat.cpp` |

**2.1.0-p1: HitValidator 인터페이스 정의**

```cpp
struct HitRequest {
    std::string shooter_id;
    std::string target_id;
    Vec3 origin;
    Vec3 direction;
    int64_t client_timestamp;
};

struct HitResult {
    bool valid;
    std::string target_id;
    Vec3 hit_point;
    float damage;
    std::string reject_reason;
};

class HitValidator {
public:
    virtual ~HitValidator() = default;
    virtual HitResult ValidateHit(const HitRequest& request) = 0;
};
```

**2.1.0-p2: RaycastSystem 구현**

```cpp
struct RaycastHit {
    std::string entity_id;
    Vec3 hit_point;
    Vec3 hit_normal;
    float distance;
    HitboxType hitbox;  // HEAD, BODY, LIMB
};

class RaycastSystem {
public:
    std::optional<RaycastHit> Cast(
        const Vec3& origin,
        const Vec3& direction,
        float max_distance,
        const std::vector<std::string>& ignore_list);

private:
    bool IntersectAABB(const Ray& ray, const AABB& box, float& t);
    bool IntersectHitbox(const Ray& ray, const Hitbox& hitbox, float& t);
};
```

**2.1.0-p3: WorldStateBuffer 구현**

```cpp
class WorldStateBuffer {
public:
    static constexpr int BUFFER_SIZE = 64;  // 60 TPS 기준 약 1초

    void SaveState(const WorldState& state);
    WorldState GetStateAt(int64_t timestamp);

private:
    std::array<WorldState, BUFFER_SIZE> buffer_;
    int head_ = 0;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(anticheat): define HitValidator interface"
git commit -m "feat(anticheat): implement RaycastSystem"
git commit -m "feat(anticheat): implement WorldStateBuffer"
git commit -m "feat(game): integrate server-side hit validation"
git tag -a v2.1.0 -m "v2.1.0: Server-Authoritative Hit Validation"
```

### ✅ 완료 기준
- [ ] 레이캐스트 히트 검증 테스트
- [ ] 지연 보상 과거 상태 조회 동작
- [ ] 부정 히트 거부 로그 확인

---

### v2.1.1: 이동 속도 및 월핵 탐지

**선행조건:** v2.1.0 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.1.1-p1 | MovementValidator 구현 | `anticheat/movement_validator.*` |
| 2.1.1-p2 | 텔레포트 탐지 | `anticheat/movement_validator.cpp` |
| 2.1.1-p3 | 월핵 탐지 | `anticheat/wall_clip_detector.*` |
| 2.1.1-p4 | 이동 검증 통합 | `game/game_session.cpp` |

**2.1.1-p1: MovementValidator 구현**

```cpp
struct MovementCheck {
    bool valid;
    float actual_speed;
    float max_allowed_speed;
    std::string violation;
};

class MovementValidator {
public:
    static constexpr float BASE_SPEED = 5.0f;
    static constexpr float SPRINT_MULTIPLIER = 1.5f;
    static constexpr float TOLERANCE = 1.1f;  // 10% 오차 허용

    MovementCheck ValidateMovement(
        const std::string& player_id,
        const Vec3& old_pos,
        const Vec3& new_pos,
        float delta_time);

    bool DetectTeleport(const Vec3& old_pos, const Vec3& new_pos, float delta_time);

private:
    std::unordered_map<std::string, PlayerMovementState> states_;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(anticheat): implement MovementValidator"
git commit -m "feat(anticheat): add teleport detection"
git commit -m "feat(anticheat): implement wall clip detection"
git commit -m "feat(game): integrate movement validation"
git tag -a v2.1.1 -m "v2.1.1: Speed & Wallhack Detection"
```

### ✅ 완료 기준
- [ ] 속도 초과 이동 거부 확인
- [ ] 텔레포트 감지 로그 확인
- [ ] 벽 통과 이동 거부 확인

---

### v2.1.2: 이상 행동 통계 탐지

**선행조건:** v2.1.1 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.1.2-p1 | CombatStats 수집 | `anticheat/combat_stats.*` |
| 2.1.2-p2 | AnomalyDetector 구현 | `anticheat/anomaly_detector.*` |
| 2.1.2-p3 | SuspicionSystem 구현 | `anticheat/suspicion_system.*` |
| 2.1.2-p4 | 통계 분석 통합 | `stats/match_stats.cpp` |

**2.1.2-p1: CombatStats 수집**

```cpp
struct CombatStats {
    int total_shots = 0;
    int hits = 0;
    int headshots = 0;
    std::vector<float> reaction_times;
    int max_kill_streak = 0;

    float Accuracy() const;
    float HeadshotRatio() const;
    float AvgReactionTime() const;
};
```

**2.1.2-p2: AnomalyDetector 구현**

```cpp
struct AnomalyScore {
    float accuracy;       // 0.0 ~ 1.0 (1.0 = 매우 의심)
    float headshot;
    float reaction;
    float consistency;

    float Combined() const;
};

class AnomalyDetector {
public:
    AnomalyScore Analyze(const CombatStats& stats);

private:
    // Z-score 기반 이상치 계산
    float CalculateZScore(float value, float mean, float stddev);
};
```

**2.1.2-p3: SuspicionSystem 구현**

```cpp
enum class SuspicionLevel {
    NONE,
    LOW,
    MEDIUM,
    HIGH,
    CRITICAL
};

class SuspicionSystem {
public:
    void RecordViolation(const std::string& player_id, const std::string& type, float severity);
    SuspicionLevel GetLevel(const std::string& player_id) const;
    std::vector<std::string> GetViolationHistory(const std::string& player_id) const;
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(anticheat): implement CombatStats collection"
git commit -m "feat(anticheat): implement AnomalyDetector"
git commit -m "feat(anticheat): implement SuspicionSystem"
git commit -m "feat(stats): integrate anomaly detection"
git tag -a v2.1.2 -m "v2.1.2: Statistical Anomaly Detection"
```

### ✅ 완료 기준
- [ ] 전투 통계 수집 동작
- [ ] 이상치 점수 계산 검증
- [ ] 의심 레벨 누적 확인

---

### v2.1.3: 리플레이 시스템

**선행조건:** v2.1.2 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.1.3-p1 | ReplayRecorder 구현 | `anticheat/replay_recorder.*` |
| 2.1.3-p2 | ReplayStorage 구현 | `anticheat/replay_storage.*` |
| 2.1.3-p3 | 리플레이 뷰어 API | `network/replay_api.*` |
| 2.1.3-p4 | 의심 플레이어 자동 저장 | `anticheat/suspicion_system.cpp` |

**2.1.3-p1: ReplayRecorder 구현**

```cpp
struct ReplayFrame {
    int64_t timestamp;
    std::vector<PlayerState> players;
    std::vector<ProjectileState> projectiles;
    std::vector<GameEvent> events;
};

class ReplayRecorder {
public:
    void StartRecording(const std::string& match_id);
    void RecordFrame(const ReplayFrame& frame);
    void StopRecording();
    std::vector<uint8_t> GetCompressedReplay();
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(anticheat): implement ReplayRecorder"
git commit -m "feat(anticheat): implement ReplayStorage"
git commit -m "feat(network): add replay viewer API"
git commit -m "feat(anticheat): auto-record suspicious players"
git tag -a v2.1.3 -m "v2.1.3: Replay System"
```

### ✅ 완료 기준
- [ ] 매치 리플레이 녹화/재생 동작
- [ ] 압축 저장 용량 확인
- [ ] HIGH 레벨 이상 자동 녹화 확인

---

### v2.1.4: 밴 시스템

**선행조건:** v2.1.3 완료

| 패치 ID | 작업 | 핵심 파일 |
|---------|------|----------|
| 2.1.4-p1 | BanPolicy 정의 | `anticheat/ban_policy.h` |
| 2.1.4-p2 | BanService 구현 | `anticheat/ban_service.*` |
| 2.1.4-p3 | 밴 우회 방지 | `anticheat/hardware_id.*` |
| 2.1.4-p4 | 밴 시스템 통합 | `anticheat/suspicion_system.cpp` |

**2.1.4-p1: BanPolicy 정의**

```cpp
enum class BanDuration {
    WARNING,           // 경고만
    TEMP_1_HOUR,
    TEMP_24_HOURS,
    TEMP_7_DAYS,
    TEMP_30_DAYS,
    PERMANENT
};

struct BanPolicy {
    SuspicionLevel trigger_level;
    BanDuration first_offense;
    BanDuration second_offense;
    BanDuration third_offense;
};
```

**2.1.4-p2: BanService 구현**

```cpp
struct BanRecord {
    std::string player_id;
    std::string hardware_id;
    std::string ip_address;
    BanDuration duration;
    int64_t ban_start;
    int64_t ban_end;
    std::string reason;
    std::vector<std::string> evidence;  // 리플레이 ID 등
};

class BanService {
public:
    bool IsPlayerBanned(const std::string& player_id);
    bool IsHardwareBanned(const std::string& hardware_id);
    bool IsIpBanned(const std::string& ip);

    void BanPlayer(const BanRecord& record);
    void UnbanPlayer(const std::string& player_id);

    std::vector<BanRecord> GetBanHistory(const std::string& player_id);
};
```

### 🔖 커밋 포인트
```bash
git commit -m "feat(anticheat): define BanPolicy"
git commit -m "feat(anticheat): implement BanService"
git commit -m "feat(anticheat): add hardware ID ban support"
git commit -m "feat(anticheat): integrate auto-ban with suspicion system"
git tag -a v2.1.4 -m "v2.1.4: Ban System"
```

### ✅ 완료 기준
- [ ] 밴 적용/해제 동작 확인
- [ ] 하드웨어 ID 밴 우회 방지
- [ ] 의심 레벨에 따른 자동 밴 동작