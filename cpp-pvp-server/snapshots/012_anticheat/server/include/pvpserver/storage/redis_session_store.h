#pragma once

#include "session_store.h"

#include <memory>
#include <string>
#include <vector>

namespace pvpserver {
namespace storage {

/**
 * Redis 연결 설정
 */
struct RedisConfig {
    std::vector<std::string> cluster_nodes;  // "host:port" 형태
    std::string password;
    int connection_pool_size = 10;
    int connection_timeout_ms = 5000;
    int command_timeout_ms = 1000;
    int session_ttl_seconds = 3600;
};

// Forward declaration (실제 Redis 클라이언트 라이브러리에 의존)
class RedisClusterClient;

/**
 * Redis 세션 저장소
 * 프로덕션용 Redis Cluster 지원 구현체입니다.
 */
class RedisSessionStore : public SessionStore {
public:
    /**
     * 생성자
     * @param config Redis 설정
     */
    explicit RedisSessionStore(const RedisConfig& config);
    ~RedisSessionStore() override;

    // SessionStore 인터페이스 구현
    bool SaveSession(const std::string& session_id,
                    const SessionData& data) override;

    std::optional<SessionData> GetSession(
        const std::string& session_id) override;

    bool DeleteSession(const std::string& session_id) override;

    bool RefreshSession(const std::string& session_id) override;

    std::vector<std::string> GetAllSessionIds() override;

    std::optional<std::string> GetSessionByPlayerId(
        const std::string& player_id) override;

    size_t GetActiveSessionCount() override;

    bool SessionExists(const std::string& session_id) override;

    // Redis 특화 기능
    bool IsConnected() const;
    void Reconnect();

private:
    RedisConfig config_;
    std::unique_ptr<RedisClusterClient> client_;

    static constexpr const char* SESSION_KEY_PREFIX = "pvp:session:";
    static constexpr const char* PLAYER_SESSION_PREFIX = "pvp:player_session:";
    static constexpr const char* SESSION_SET_KEY = "pvp:sessions";

    std::string MakeSessionKey(const std::string& session_id) const;
    std::string MakePlayerSessionKey(const std::string& player_id) const;
};

}  // namespace storage
}  // namespace pvpserver
