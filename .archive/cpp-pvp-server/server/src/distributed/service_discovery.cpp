// [FILE]
// - 목적: 서비스 디스커버리 (서버 자동 탐지)
// - 주요 역할: 게임 서버들의 등록/해제, 가용 서버 목록 조회
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템
// - 권장 읽는 순서: Register → GetAvailableServers → Heartbeat
//
// [LEARN] 서비스 디스커버리:
//         - 동적 서버 확장: 새 서버가 자동으로 발견됨
//         - 헬스 체크: 하트비트로 서버 생존 확인
//         - Redis Pub/Sub: 이벤트 기반 알림 (서버 추가/제거)
//
// 대안: Consul, etcd, Kubernetes Service Discovery

#include "pvpserver/distributed/service_discovery.h"

#include <chrono>

namespace pvpserver {
namespace distributed {

// [Order 1] RedisClient 스텁
// [LEARN] 실제 구현 시 redis-plus-plus 사용.
//         스텁: 인터페이스만 정의, 실제 동작 없음 (테스트/개발용)
class RedisClient {
public:
    // SET key value EX ttl
    bool set(const std::string& /*key*/, const std::string& /*value*/,
             int /*ttl_seconds*/) { return true; }
    // GET key
    std::optional<std::string> get(const std::string& /*key*/) {
        return std::nullopt;
    }
    // DEL key
    bool del(const std::string& /*key*/) { return true; }
    // EXPIRE key seconds
    bool expire(const std::string& /*key*/, int /*seconds*/) { return true; }
    // SMEMBERS key (Set 조회)
    std::vector<std::string> smembers(const std::string& /*key*/) { return {}; }
    // SADD key member (Set 추가)
    bool sadd(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }
    // SREM key member (Set 제거)
    bool srem(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }
    // PUBLISH channel message (Pub/Sub)
    void publish(const std::string& /*channel*/, const std::string& /*msg*/) {}
    // SUBSCRIBE channel
    void subscribe(const std::string& /*channel*/,
                  std::function<void(const std::string&)> /*handler*/) {}
    // PING - 연결 확인
    bool ping() { return true; }
};

// ==================== ServiceDiscovery ====================

// 생성자 - Redis 클라이언트 주입
ServiceDiscovery::ServiceDiscovery(std::shared_ptr<RedisClient> redis)
    : redis_(redis) {}

ServiceDiscovery::~ServiceDiscovery() {
    StopHeartbeat();
}

// [Order 2] Register - 서버 등록
// [LEARN] 서버 시작 시 호출:
//         1. 자신의 정보를 Redis에 저장
//         2. 하트비트 시작 (TTL 갱신)
void ServiceDiscovery::Register(const ServerInfo& self) {
    self_ = self;
    PublishServerInfo();  // Redis에 저장
    StartHeartbeat();     // 주기적 갱신 시작
}

// Unregister - 서버 등록 해제
// [LEARN] 서버 종료 시 호출. 다른 서버들에게 제거 알림.
void ServiceDiscovery::Unregister() {
    StopHeartbeat();

    if (redis_) {
        // 서버 정보 삭제
        redis_->del("pvp:server:" + self_.server_id);
        redis_->srem("pvp:servers", self_.server_id);

        // 제거 이벤트 발행 (Pub/Sub)
        redis_->publish("pvp:server:events",
                       "{\"event\":\"removed\",\"server_id\":\"" +
                       self_.server_id + "\"}");
    }
}

// [Order 3] GetAvailableServers - 가용 서버 목록 조회
// [LEARN] 로드 밸런서가 호출. Redis Set에서 서버 ID 목록 → 상세 정보 조회
std::vector<ServerInfo> ServiceDiscovery::GetAvailableServers() {
    std::vector<ServerInfo> servers;

    if (!redis_) return servers;

    // pvp:servers Set에서 모든 서버 ID 조회
    auto server_ids = redis_->smembers("pvp:servers");
    for (const auto& id : server_ids) {
        // 각 서버의 상세 정보 조회
        auto data = redis_->get("pvp:server:" + id);
        if (data) {
            auto info = ServerInfo::Deserialize(*data);
            if (info) {
                servers.push_back(*info);
            }
        }
    }

    return servers;
}

// GetServer - 특정 서버 정보 조회
std::optional<ServerInfo> ServiceDiscovery::GetServer(const std::string& server_id) {
    if (!redis_) return std::nullopt;

    auto data = redis_->get("pvp:server:" + server_id);
    if (!data) return std::nullopt;

    return ServerInfo::Deserialize(*data);
}

// 콜백 등록 - 서버 추가/제거 이벤트
void ServiceDiscovery::OnServerAdded(std::function<void(const ServerInfo&)> callback) {
    on_server_added_ = callback;
}

void ServiceDiscovery::OnServerRemoved(std::function<void(const std::string&)> callback) {
    on_server_removed_ = callback;
}

// [Order 4] StartHeartbeat - 하트비트 스레드 시작
// [LEARN] 주기적으로 TTL 갱신. 갱신 안 되면 자동 만료 → 장애 감지
void ServiceDiscovery::StartHeartbeat() {
    if (running_.load()) return;

    running_.store(true);
    heartbeat_thread_ = std::thread(&ServiceDiscovery::HeartbeatLoop, this);
}

void ServiceDiscovery::StopHeartbeat() {
    running_.store(false);
    if (heartbeat_thread_.joinable()) {
        heartbeat_thread_.join();
    }
}

void ServiceDiscovery::UpdateServerInfo(const ServerInfo& info) {
    self_ = info;
    PublishServerInfo();
}

void ServiceDiscovery::HeartbeatLoop() {
    while (running_.load()) {
        PublishServerInfo();
        std::this_thread::sleep_for(std::chrono::seconds(HEARTBEAT_INTERVAL_SEC));
    }
}

void ServiceDiscovery::PublishServerInfo() {
    if (!redis_) return;

    // 현재 시간 갱신
    self_.last_heartbeat = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    // 서버 정보 저장
    redis_->set("pvp:server:" + self_.server_id,
               self_.Serialize(), SERVER_TTL_SEC);

    // 서버 집합에 추가
    redis_->sadd("pvp:servers", self_.server_id);
}

void ServiceDiscovery::SubscriptionLoop() {
    // 실제 구현에서는 Redis Pub/Sub 구독
    // redis_->subscribe("pvp:server:events", [this](const std::string& msg) {
    //     // 이벤트 처리
    // });
}

// ==================== HealthChecker ====================

HealthChecker::HealthChecker(std::shared_ptr<ServiceDiscovery> discovery,
                             std::shared_ptr<LoadBalancer> balancer,
                             std::chrono::seconds check_interval)
    : discovery_(discovery)
    , balancer_(balancer)
    , check_interval_(check_interval) {}

HealthChecker::~HealthChecker() {
    Stop();
}

void HealthChecker::Start() {
    if (running_.load()) return;

    running_.store(true);
    check_thread_ = std::thread(&HealthChecker::CheckLoop, this);
}

void HealthChecker::Stop() {
    running_.store(false);
    if (check_thread_.joinable()) {
        check_thread_.join();
    }
}

bool HealthChecker::IsServerHealthy(const std::string& server_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = failure_counts_.find(server_id);
    if (it == failure_counts_.end()) {
        return true;  // 실패 기록 없음 = 정상
    }
    return it->second < FAILURE_THRESHOLD;
}

void HealthChecker::OnServerUnhealthy(std::function<void(const std::string&)> callback) {
    on_unhealthy_ = callback;
}

void HealthChecker::OnServerRecovered(std::function<void(const std::string&)> callback) {
    on_recovered_ = callback;
}

void HealthChecker::CheckLoop() {
    while (running_.load()) {
        auto servers = discovery_->GetAvailableServers();

        for (const auto& server : servers) {
            bool healthy = CheckServer(server);

            std::lock_guard<std::mutex> lock(mutex_);

            if (healthy) {
                // 복구 확인
                if (failure_counts_[server.server_id] >= FAILURE_THRESHOLD) {
                    if (on_recovered_) {
                        on_recovered_(server.server_id);
                    }
                    balancer_->MarkServerHealthy(server.server_id, true);
                }
                failure_counts_[server.server_id] = 0;
            } else {
                // 실패 카운트 증가
                int& count = failure_counts_[server.server_id];
                ++count;

                if (count == FAILURE_THRESHOLD) {
                    if (on_unhealthy_) {
                        on_unhealthy_(server.server_id);
                    }
                    balancer_->MarkServerHealthy(server.server_id, false);
                }
            }
        }

        std::this_thread::sleep_for(check_interval_);
    }
}

bool HealthChecker::CheckServer(const ServerInfo& server) {
    // 마지막 하트비트가 TTL 내인지 확인
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    // 15초(TTL) 이상 하트비트 없으면 비정상
    return (now - server.last_heartbeat) < 15000;
}

}  // namespace distributed
}  // namespace pvpserver
