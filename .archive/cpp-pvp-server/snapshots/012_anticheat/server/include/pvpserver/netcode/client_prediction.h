#pragma once

#include <cstdint>
#include <deque>
#include <optional>
#include <vector>

#include "pvpserver/game/player_state.h"
#include "pvpserver/network/packet_types.h"

namespace pvpserver::netcode {

/**
 * @brief 예측된 상태
 */
struct PredictedState {
    std::uint32_t input_sequence;
    PlayerState state;
    std::uint64_t timestamp;
};

/**
 * @brief 클라이언트 예측 엔진
 * 
 * 클라이언트 측에서 서버 응답을 기다리지 않고 로컬 시뮬레이션을 수행합니다.
 */
class ClientPrediction {
   public:
    static constexpr std::size_t MAX_PENDING_INPUTS = 128;

    /**
     * @brief 입력 적용 및 예측 상태 생성
     */
    PredictedState Predict(
        const PlayerState& current_state,
        const InputCommand& input,
        float delta_time
    );

    /**
     * @brief 예측 상태 저장
     */
    void SavePrediction(const PredictedState& prediction);

    /**
     * @brief 서버 확인된 시퀀스까지의 예측 제거
     */
    void AcknowledgeUpTo(std::uint32_t server_sequence);

    /**
     * @brief 미확인 입력 목록 반환
     */
    std::vector<InputCommand> GetUnacknowledgedInputs(
        std::uint32_t after_sequence
    ) const;

    /**
     * @brief 가장 최근 예측 상태
     */
    std::optional<PredictedState> GetLatestPrediction() const;

    /**
     * @brief 특정 시퀀스의 예측 상태
     */
    std::optional<PredictedState> GetPrediction(std::uint32_t sequence) const;

    /**
     * @brief 예측 정확도 통계
     */
    struct Stats {
        std::uint64_t total_predictions{0};
        std::uint64_t accurate_predictions{0};
        float accuracy() const {
            return total_predictions > 0 
                ? static_cast<float>(accurate_predictions) / total_predictions 
                : 1.0f;
        }
    };
    Stats GetStats() const { return stats_; }

    /**
     * @brief 예측 결과 기록 (서버 상태와 비교 후)
     */
    void RecordPredictionResult(bool accurate);

   private:
    std::deque<PredictedState> predictions_;
    std::deque<InputCommand> input_history_;
    std::uint32_t last_acknowledged_{0};
    Stats stats_;
};

}  // namespace pvpserver::netcode
