#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

namespace pvpserver {
namespace storage {

/**
 * 세션 데이터 구조체
 * 플레이어 세션 정보를 저장합니다.
 */
struct SessionData {
    std::string player_id;
    std::string player_name;
    std::string server_id;        // 할당된 게임 서버
    int64_t created_at;           // 생성 타임스탬프 (epoch ms)
    int64_t last_activity;        // 마지막 활동 타임스탬프
    int elo_rating;
    std::string match_id;         // 현재 매치 (있는 경우)

    // 직렬화
    std::string Serialize() const;
    static std::optional<SessionData> Deserialize(const std::string& data);

    // 현재 시간으로 last_activity 갱신
    void Touch();

    // 유효성 검증
    bool IsValid() const;
};

/**
 * 세션 저장소 추상 인터페이스
 * InMemory 또는 Redis 구현을 추상화합니다.
 */
class SessionStore {
public:
    virtual ~SessionStore() = default;

    /**
     * 세션 저장
     * @param session_id 세션 고유 ID
     * @param data 세션 데이터
     * @return 성공 여부
     */
    virtual bool SaveSession(const std::string& session_id,
                            const SessionData& data) = 0;

    /**
     * 세션 조회
     * @param session_id 세션 고유 ID
     * @return 세션 데이터 (없으면 nullopt)
     */
    virtual std::optional<SessionData> GetSession(
        const std::string& session_id) = 0;

    /**
     * 세션 삭제
     * @param session_id 세션 고유 ID
     * @return 성공 여부
     */
    virtual bool DeleteSession(const std::string& session_id) = 0;

    /**
     * 세션 갱신 (TTL 연장)
     * @param session_id 세션 고유 ID
     * @return 성공 여부
     */
    virtual bool RefreshSession(const std::string& session_id) = 0;

    /**
     * 모든 세션 ID 조회
     * @return 세션 ID 목록
     */
    virtual std::vector<std::string> GetAllSessionIds() = 0;

    /**
     * player_id로 세션 조회
     * @param player_id 플레이어 ID
     * @return 세션 ID (없으면 nullopt)
     */
    virtual std::optional<std::string> GetSessionByPlayerId(
        const std::string& player_id) = 0;

    /**
     * 활성 세션 수 조회
     * @return 활성 세션 수
     */
    virtual size_t GetActiveSessionCount() = 0;

    /**
     * 세션 존재 여부 확인
     * @param session_id 세션 고유 ID
     * @return 존재 여부
     */
    virtual bool SessionExists(const std::string& session_id) = 0;
};

/**
 * 세션 ID 생성 유틸리티
 */
std::string GenerateSessionId();

/**
 * 현재 타임스탬프 (epoch milliseconds)
 */
int64_t GetCurrentTimestamp();

}  // namespace storage
}  // namespace pvpserver
