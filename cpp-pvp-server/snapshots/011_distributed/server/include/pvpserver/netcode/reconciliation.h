#pragma once

#include <cstdint>

#include "pvpserver/game/player_state.h"
#include "pvpserver/netcode/client_prediction.h"

namespace pvpserver::netcode {

/**
 * @brief 리컨실리에이션 결과
 */
struct ReconciliationResult {
    bool mismatch_detected{false};
    float position_error{0.0f};
    float velocity_error{0.0f};
    PlayerState corrected_state;
};

/**
 * @brief 리컨실리에이션 시스템
 * 
 * 서버 상태와 클라이언트 예측을 비교하고 불일치 시 보정합니다.
 */
class Reconciliation {
   public:
    // 보정 임계값
    static constexpr float POSITION_THRESHOLD = 0.1f;   // 10cm
    static constexpr float VELOCITY_THRESHOLD = 0.5f;   // 0.5 m/s

    /**
     * @brief 서버 상태와 비교하여 리컨실리에이션 수행
     */
    ReconciliationResult Reconcile(
        const PlayerState& server_state,
        std::uint32_t server_input_sequence,
        ClientPrediction& prediction,
        float delta_time
    );

    /**
     * @brief 부드러운 보정 (스냅이 아닌 보간)
     */
    static PlayerState SmoothCorrection(
        const PlayerState& current,
        const PlayerState& target,
        float blend_factor
    );

    /**
     * @brief 리컨실리에이션 통계
     */
    struct Stats {
        std::uint64_t total_reconciliations{0};
        std::uint64_t mismatches{0};
        float avg_position_error{0.0f};
    };
    Stats GetStats() const { return stats_; }

   private:
    /**
     * @brief 미확인 입력 재시뮬레이션
     */
    PlayerState Resimulate(
        const PlayerState& server_state,
        const std::vector<InputCommand>& inputs,
        float delta_time
    );

    Stats stats_;
};

}  // namespace pvpserver::netcode
