#pragma once

#include "session_store.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace pvpserver {
namespace storage {

/**
 * 인메모리 세션 저장소
 * 개발 및 테스트용 구현체입니다.
 */
class InMemorySessionStore : public SessionStore {
public:
    /**
     * 생성자
     * @param ttl_seconds 세션 TTL (기본 1시간)
     * @param cleanup_interval_seconds 정리 주기 (기본 60초)
     */
    explicit InMemorySessionStore(int ttl_seconds = 3600,
                                  int cleanup_interval_seconds = 60);
    ~InMemorySessionStore() override;

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

    // 추가 유틸리티
    void CleanExpiredSessions();
    void StopCleanup();

private:
    struct SessionEntry {
        SessionData data;
        int64_t expiry_time;  // epoch ms
    };

    int ttl_seconds_;
    int cleanup_interval_seconds_;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, SessionEntry> sessions_;
    std::unordered_map<std::string, std::string> player_to_session_;  // player_id -> session_id

    std::atomic<bool> running_{true};
    std::thread cleanup_thread_;

    void CleanupLoop();
    bool IsExpired(const SessionEntry& entry) const;
};

}  // namespace storage
}  // namespace pvpserver
