#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

#include "pvpserver/game/player_state.h"
#include "pvpserver/game/projectile.h"

namespace pvpserver::netcode {

/**
 * @brief 월드 상태 (지연 보상용)
 */
struct WorldState {
    std::uint64_t timestamp;
    std::vector<PlayerState> players;
    std::vector<Projectile> projectiles;
};

/**
 * @brief 히트 요청
 */
struct HitRequest {
    std::string shooter_id;
    std::uint64_t client_timestamp;
    float origin_x;
    float origin_y;
    float direction_x;
    float direction_y;
};

/**
 * @brief 히트 결과
 */
struct HitResult {
    bool valid{false};
    std::string hit_player_id;
    float hit_x{0.0f};
    float hit_y{0.0f};
    float damage{0.0f};
    std::string reject_reason;
};

/**
 * @brief 지연 보상 시스템
 * 
 * 과거 월드 상태를 저장하고, 히트 판정 시 클라이언트 시점으로 되돌려 검증합니다.
 */
class LagCompensation {
   public:
    static constexpr std::size_t HISTORY_SIZE = 128;  // 약 2초
    static constexpr int MAX_REWIND_MS = 200;         // 최대 되돌리기

    /**
     * @brief 현재 상태 저장
     */
    void SaveWorldState(const WorldState& state);

    /**
     * @brief 특정 시점의 상태 조회 (보간 포함)
     */
    std::optional<WorldState> GetWorldStateAt(std::uint64_t timestamp) const;

    /**
     * @brief 지연 보상된 히트 검증
     */
    HitResult ValidateHitWithCompensation(
        const HitRequest& request,
        std::uint64_t server_time
    );

    /**
     * @brief 클라이언트 RTT 기반 되돌리기 시간 계산
     */
    std::uint64_t CalculateRewindTime(
        std::uint64_t client_timestamp,
        std::uint64_t server_time,
        std::uint32_t client_rtt
    ) const;

    /**
     * @brief 히스토리 크기
     */
    std::size_t HistorySize() const { return history_.size(); }

    /**
     * @brief 통계
     */
    struct Stats {
        std::uint64_t hits_validated{0};
        std::uint64_t hits_accepted{0};
        std::uint64_t hits_rejected{0};
        float avg_rewind_ms{0.0f};
    };
    Stats GetStats() const { return stats_; }

   private:
    /**
     * @brief 두 상태 간 보간
     */
    WorldState Interpolate(
        const WorldState& a,
        const WorldState& b,
        float t
    ) const;

    /**
     * @brief 레이캐스트 히트 검사
     */
    std::optional<std::pair<std::string, std::pair<float, float>>> RaycastPlayers(
        const WorldState& state,
        const std::string& shooter_id,
        float origin_x,
        float origin_y,
        float dir_x,
        float dir_y
    ) const;

    std::deque<WorldState> history_;
    Stats stats_;
};

}  // namespace pvpserver::netcode
