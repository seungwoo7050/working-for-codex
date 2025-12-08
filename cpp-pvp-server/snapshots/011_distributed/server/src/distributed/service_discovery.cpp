#include "pvpserver/distributed/service_discovery.h"

#include <chrono>

namespace pvpserver {
namespace distributed {

// RedisClient 스텁 (실제 구현 시 redis-plus-plus 사용)
class RedisClient {
public:
    bool set(const std::string& /*key*/, const std::string& /*value*/,
             int /*ttl_seconds*/) { return true; }
    std::optional<std::string> get(const std::string& /*key*/) {
        return std::nullopt;
    }
    bool del(const std::string& /*key*/) { return true; }
    bool expire(const std::string& /*key*/, int /*seconds*/) { return true; }
    std::vector<std::string> smembers(const std::string& /*key*/) { return {}; }
    bool sadd(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }
    bool srem(const std::string& /*key*/, const std::string& /*member*/) {
        return true;
    }
    void publish(const std::string& /*channel*/, const std::string& /*msg*/) {}
    void subscribe(const std::string& /*channel*/,
                  std::function<void(const std::string&)> /*handler*/) {}
    bool ping() { return true; }
};

// ==================== ServiceDiscovery ====================

ServiceDiscovery::ServiceDiscovery(std::shared_ptr<RedisClient> redis)
    : redis_(redis) {}

ServiceDiscovery::~ServiceDiscovery() {
    StopHeartbeat();
}

void ServiceDiscovery::Register(const ServerInfo& self) {
    self_ = self;
    PublishServerInfo();
    StartHeartbeat();
}

void ServiceDiscovery::Unregister() {
    StopHeartbeat();

    if (redis_) {
        // 서버 정보 삭제
        redis_->del("pvp:server:" + self_.server_id);
        redis_->srem("pvp:servers", self_.server_id);

        // 제거 이벤트 발행
        redis_->publish("pvp:server:events",
                       "{\"event\":\"removed\",\"server_id\":\"" +
                       self_.server_id + "\"}");
    }
}

std::vector<ServerInfo> ServiceDiscovery::GetAvailableServers() {
    std::vector<ServerInfo> servers;

    if (!redis_) return servers;

    auto server_ids = redis_->smembers("pvp:servers");
    for (const auto& id : server_ids) {
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

std::optional<ServerInfo> ServiceDiscovery::GetServer(const std::string& server_id) {
    if (!redis_) return std::nullopt;

    auto data = redis_->get("pvp:server:" + server_id);
    if (!data) return std::nullopt;

    return ServerInfo::Deserialize(*data);
}

void ServiceDiscovery::OnServerAdded(std::function<void(const ServerInfo&)> callback) {
    on_server_added_ = callback;
}

void ServiceDiscovery::OnServerRemoved(std::function<void(const std::string&)> callback) {
    on_server_removed_ = callback;
}

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
