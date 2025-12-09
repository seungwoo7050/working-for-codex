#pragma once

#include "load_balancer.h"

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace pvpserver {
namespace distributed {

// Forward declaration
class RedisClient;

/**
 * 서비스 디스커버리
 * Redis를 사용한 서버 등록/발견 시스템입니다.
 */
class ServiceDiscovery {
public:
    /**
     * 생성자
     * @param redis Redis 클라이언트
     */
    explicit ServiceDiscovery(std::shared_ptr<RedisClient> redis);
    ~ServiceDiscovery();

    /**
     * 서버 등록 (하트비트 시작)
     * @param self 자신의 서버 정보
     */
    void Register(const ServerInfo& self);

    /**
     * 서버 해제
     */
    void Unregister();

    /**
     * 사용 가능한 서버 목록 조회
     * @return 서버 정보 목록
     */
    std::vector<ServerInfo> GetAvailableServers();

    /**
     * 특정 서버 정보 조회
     * @param server_id 서버 식별자
     * @return 서버 정보 (없으면 nullopt)
     */
    std::optional<ServerInfo> GetServer(const std::string& server_id);

    /**
     * 서버 추가 콜백 등록
     * @param callback 콜백 함수
     */
    void OnServerAdded(std::function<void(const ServerInfo&)> callback);

    /**
     * 서버 제거 콜백 등록
     * @param callback 콜백 함수
     */
    void OnServerRemoved(std::function<void(const std::string&)> callback);

    /**
     * 하트비트 시작
     */
    void StartHeartbeat();

    /**
     * 하트비트 중지
     */
    void StopHeartbeat();

    /**
     * 서버 정보 갱신
     * @param info 갱신된 서버 정보
     */
    void UpdateServerInfo(const ServerInfo& info);

private:
    std::shared_ptr<RedisClient> redis_;
    ServerInfo self_;
    std::atomic<bool> running_{false};
    std::thread heartbeat_thread_;
    std::thread subscription_thread_;

    std::function<void(const ServerInfo&)> on_server_added_;
    std::function<void(const std::string&)> on_server_removed_;

    static constexpr int HEARTBEAT_INTERVAL_SEC = 5;
    static constexpr int SERVER_TTL_SEC = 15;

    void HeartbeatLoop();
    void SubscriptionLoop();
    void PublishServerInfo();
};

/**
 * 헬스 체커
 * 서버 상태를 모니터링합니다.
 */
class HealthChecker {
public:
    /**
     * 생성자
     * @param discovery 서비스 디스커버리
     * @param balancer 로드 밸런서
     * @param check_interval 체크 주기 (기본 5초)
     */
    HealthChecker(std::shared_ptr<ServiceDiscovery> discovery,
                  std::shared_ptr<LoadBalancer> balancer,
                  std::chrono::seconds check_interval = std::chrono::seconds(5));
    ~HealthChecker();

    /**
     * 체커 시작
     */
    void Start();

    /**
     * 체커 중지
     */
    void Stop();

    /**
     * 서버 상태 조회
     * @param server_id 서버 식별자
     * @return 정상 여부
     */
    bool IsServerHealthy(const std::string& server_id) const;

    /**
     * 서버 비정상 콜백 등록
     * @param callback 콜백 함수
     */
    void OnServerUnhealthy(std::function<void(const std::string&)> callback);

    /**
     * 서버 복구 콜백 등록
     * @param callback 콜백 함수
     */
    void OnServerRecovered(std::function<void(const std::string&)> callback);

private:
    std::shared_ptr<ServiceDiscovery> discovery_;
    std::shared_ptr<LoadBalancer> balancer_;
    std::chrono::seconds check_interval_;

    std::atomic<bool> running_{false};
    std::thread check_thread_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, int> failure_counts_;

    std::function<void(const std::string&)> on_unhealthy_;
    std::function<void(const std::string&)> on_recovered_;

    static constexpr int FAILURE_THRESHOLD = 3;

    void CheckLoop();
    bool CheckServer(const ServerInfo& server);
};

}  // namespace distributed
}  // namespace pvpserver
