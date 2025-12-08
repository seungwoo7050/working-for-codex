#pragma once

#include "consistent_hash.h"

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace pvpserver {
namespace distributed {

/**
 * 서버 정보
 */
struct ServerInfo {
    std::string server_id;
    std::string host;
    int udp_port = 0;
    int grpc_port = 0;
    int current_connections = 0;
    int max_connections = 1000;
    int64_t last_heartbeat = 0;
    bool healthy = true;
    std::string region;

    // 직렬화
    std::string Serialize() const;
    static std::optional<ServerInfo> Deserialize(const std::string& data);

    // 용량 여유 확인
    bool HasCapacity() const {
        return current_connections < max_connections;
    }

    // 부하율 (0.0 ~ 1.0)
    double GetLoadFactor() const {
        if (max_connections == 0) return 1.0;
        return static_cast<double>(current_connections) / max_connections;
    }
};

/**
 * 로드 밸런싱 전략
 */
enum class LoadBalanceStrategy {
    CONSISTENT_HASH,     // 플레이어 ID 기반 고정 할당
    ROUND_ROBIN,         // 순환 분배
    LEAST_CONNECTIONS    // 최소 연결 우선
};

/**
 * 로드 밸런서
 * 여러 분배 전략을 지원합니다.
 */
class LoadBalancer {
public:
    /**
     * 생성자
     * @param strategy 분배 전략 (기본: Consistent Hash)
     */
    explicit LoadBalancer(
        LoadBalanceStrategy strategy = LoadBalanceStrategy::CONSISTENT_HASH);
    ~LoadBalancer() = default;

    /**
     * 서버 등록
     * @param server 서버 정보
     */
    void RegisterServer(const ServerInfo& server);

    /**
     * 서버 해제
     * @param server_id 서버 식별자
     */
    void UnregisterServer(const std::string& server_id);

    /**
     * 서버 선택
     * @param player_id 플레이어 ID (Consistent Hash 시 사용)
     * @return 선택된 서버 (없으면 nullopt)
     */
    std::optional<ServerInfo> SelectServer(const std::string& player_id);

    /**
     * 서버 부하 업데이트
     * @param server_id 서버 식별자
     * @param connections 현재 연결 수
     */
    void UpdateServerLoad(const std::string& server_id, int connections);

    /**
     * 서버 상태 설정
     * @param server_id 서버 식별자
     * @param healthy 정상 여부
     */
    void MarkServerHealthy(const std::string& server_id, bool healthy);

    /**
     * 모든 서버 조회
     * @return 서버 목록
     */
    std::vector<ServerInfo> GetAllServers() const;

    /**
     * 정상 서버만 조회
     * @return 정상 서버 목록
     */
    std::vector<ServerInfo> GetHealthyServers() const;

    /**
     * 서버 정보 조회
     * @param server_id 서버 식별자
     * @return 서버 정보 (없으면 nullopt)
     */
    std::optional<ServerInfo> GetServer(const std::string& server_id) const;

    /**
     * 분배 전략 변경
     * @param strategy 새 전략
     */
    void SetStrategy(LoadBalanceStrategy strategy);

    /**
     * 현재 전략 조회
     * @return 현재 분배 전략
     */
    LoadBalanceStrategy GetStrategy() const;

private:
    LoadBalanceStrategy strategy_;
    ConsistentHashRing hash_ring_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, ServerInfo> servers_;
    std::atomic<size_t> round_robin_index_{0};

    std::optional<ServerInfo> SelectByConsistentHash(const std::string& player_id);
    std::optional<ServerInfo> SelectByRoundRobin();
    std::optional<ServerInfo> SelectByLeastConnections();
};

}  // namespace distributed
}  // namespace pvpserver
