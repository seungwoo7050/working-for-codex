// [FILE]
// - 목적: 로드 밸런서 (서버 부하 분산)
// - 주요 역할: 여러 게임 서버 중 최적 서버 선택
// - 관련 클론 가이드 단계: [CG-02.00] 분산 시스템
// - 권장 읽는 순서: RegisterServer → GetBestServer → 전략별 선택
//
// [LEARN] 로드 밸런싱 전략:
//         - ROUND_ROBIN: 순차 분배 (단순하지만 부하 무시)
//         - LEAST_CONNECTIONS: 연결 수 최소 서버 선택
//         - WEIGHTED: 서버 성능별 가중치
//         - CONSISTENT_HASH: 세션 고정 (같은 플레이어 → 같은 서버)

#include "pvpserver/distributed/load_balancer.h"

#include <algorithm>
#include <limits>
#include <sstream>

namespace pvpserver {
namespace distributed {

// [Order 1] ServerInfo 직렬화/역직렬화
// [LEARN] Redis 등 외부 저장소에 서버 정보 저장용.
//         JSON 형식으로 직렬화 (실제로는 Protobuf 권장).
std::string ServerInfo::Serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"server_id\":\"" << server_id << "\",";
    oss << "\"host\":\"" << host << "\",";
    oss << "\"udp_port\":" << udp_port << ",";
    oss << "\"grpc_port\":" << grpc_port << ",";
    oss << "\"current_connections\":" << current_connections << ",";
    oss << "\"max_connections\":" << max_connections << ",";
    oss << "\"last_heartbeat\":" << last_heartbeat << ",";
    oss << "\"healthy\":" << (healthy ? "true" : "false") << ",";
    oss << "\"region\":\"" << region << "\"";
    oss << "}";
    return oss.str();
}

// Deserialize - 간단한 JSON 파싱 (실제로는 nlohmann/json 사용)
std::optional<ServerInfo> ServerInfo::Deserialize(const std::string& data) {
    ServerInfo info;

    // 람다로 키-값 추출 (간소화된 파서)
    auto extract_string = [&data](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\":\"";
        auto pos = data.find(search);
        if (pos == std::string::npos) return "";
        pos += search.length();
        auto end = data.find("\"", pos);
        if (end == std::string::npos) return "";
        return data.substr(pos, end - pos);
    };

    auto extract_int = [&data](const std::string& key) -> int {
        std::string search = "\"" + key + "\":";
        auto pos = data.find(search);
        if (pos == std::string::npos) return 0;
        pos += search.length();
        auto end = data.find_first_of(",}", pos);
        if (end == std::string::npos) return 0;
        try {
            return std::stoi(data.substr(pos, end - pos));
        } catch (...) {
            return 0;
        }
    };

    auto extract_int64 = [&data](const std::string& key) -> int64_t {
        std::string search = "\"" + key + "\":";
        auto pos = data.find(search);
        if (pos == std::string::npos) return 0;
        pos += search.length();
        auto end = data.find_first_of(",}", pos);
        if (end == std::string::npos) return 0;
        try {
            return std::stoll(data.substr(pos, end - pos));
        } catch (...) {
            return 0;
        }
    };

    auto extract_bool = [&data](const std::string& key) -> bool {
        std::string search = "\"" + key + "\":";
        auto pos = data.find(search);
        if (pos == std::string::npos) return false;
        pos += search.length();
        return data.substr(pos, 4) == "true";
    };

    info.server_id = extract_string("server_id");
    info.host = extract_string("host");
    info.udp_port = extract_int("udp_port");
    info.grpc_port = extract_int("grpc_port");
    info.current_connections = extract_int("current_connections");
    info.max_connections = extract_int("max_connections");
    info.last_heartbeat = extract_int64("last_heartbeat");
    info.healthy = extract_bool("healthy");
    info.region = extract_string("region");

    if (info.server_id.empty()) {
        return std::nullopt;  // 필수 필드 없으면 실패
    }

    return info;
}

// [Order 2] LoadBalancer - 부하 분산 핵심 클래스

LoadBalancer::LoadBalancer(LoadBalanceStrategy strategy)
    : strategy_(strategy) {}

// RegisterServer - 서버 등록
// [LEARN] 서버 시작 시 로드 밸런서에 자신을 등록
void LoadBalancer::RegisterServer(const ServerInfo& server) {
    std::lock_guard<std::mutex> lock(mutex_);
    servers_[server.server_id] = server;
    hash_ring_.AddNode(server.server_id);
}

void LoadBalancer::UnregisterServer(const std::string& server_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    servers_.erase(server_id);
    hash_ring_.RemoveNode(server_id);
}

std::optional<ServerInfo> LoadBalancer::SelectServer(const std::string& player_id) {
    switch (strategy_) {
        case LoadBalanceStrategy::CONSISTENT_HASH:
            return SelectByConsistentHash(player_id);
        case LoadBalanceStrategy::ROUND_ROBIN:
            return SelectByRoundRobin();
        case LoadBalanceStrategy::LEAST_CONNECTIONS:
            return SelectByLeastConnections();
        default:
            return SelectByConsistentHash(player_id);
    }
}

std::optional<ServerInfo> LoadBalancer::SelectByConsistentHash(
    const std::string& player_id) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Consistent Hash로 후보 노드 조회
    auto candidates = hash_ring_.GetNodes(player_id, 3);

    for (const auto& server_id : candidates) {
        auto it = servers_.find(server_id);
        if (it != servers_.end() &&
            it->second.healthy &&
            it->second.HasCapacity()) {
            return it->second;
        }
    }

    return std::nullopt;
}

std::optional<ServerInfo> LoadBalancer::SelectByRoundRobin() {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<ServerInfo> healthy;
    for (const auto& [id, server] : servers_) {
        if (server.healthy && server.HasCapacity()) {
            healthy.push_back(server);
        }
    }

    if (healthy.empty()) {
        return std::nullopt;
    }

    size_t index = round_robin_index_.fetch_add(1) % healthy.size();
    return healthy[index];
}

std::optional<ServerInfo> LoadBalancer::SelectByLeastConnections() {
    std::lock_guard<std::mutex> lock(mutex_);

    ServerInfo* best = nullptr;
    int min_connections = std::numeric_limits<int>::max();

    for (auto& [id, server] : servers_) {
        if (server.healthy && server.HasCapacity()) {
            if (server.current_connections < min_connections) {
                min_connections = server.current_connections;
                best = &server;
            }
        }
    }

    if (best) {
        return *best;
    }
    return std::nullopt;
}

void LoadBalancer::UpdateServerLoad(const std::string& server_id, int connections) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = servers_.find(server_id);
    if (it != servers_.end()) {
        it->second.current_connections = connections;
    }
}

void LoadBalancer::MarkServerHealthy(const std::string& server_id, bool healthy) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = servers_.find(server_id);
    if (it != servers_.end()) {
        it->second.healthy = healthy;
    }
}

std::vector<ServerInfo> LoadBalancer::GetAllServers() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<ServerInfo> result;
    result.reserve(servers_.size());
    for (const auto& [id, server] : servers_) {
        result.push_back(server);
    }
    return result;
}

std::vector<ServerInfo> LoadBalancer::GetHealthyServers() const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<ServerInfo> result;
    for (const auto& [id, server] : servers_) {
        if (server.healthy) {
            result.push_back(server);
        }
    }
    return result;
}

std::optional<ServerInfo> LoadBalancer::GetServer(const std::string& server_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = servers_.find(server_id);
    if (it != servers_.end()) {
        return it->second;
    }
    return std::nullopt;
}

void LoadBalancer::SetStrategy(LoadBalanceStrategy strategy) {
    strategy_ = strategy;
}

LoadBalanceStrategy LoadBalancer::GetStrategy() const {
    return strategy_;
}

}  // namespace distributed
}  // namespace pvpserver
