#include "pvpserver/netcode/client_prediction.h"

#include <algorithm>
#include <chrono>
#include <cmath>

namespace pvpserver::netcode {

namespace {

std::uint64_t CurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

// 입력에 따른 상태 시뮬레이션
PlayerState SimulateInput(
    const PlayerState& state,
    const InputCommand& input,
    float delta_time
) {
    PlayerState result = state;
    
    constexpr float SPEED = 5.0f;  // 초당 이동 거리
    
    float dx = input.move_x * SPEED * delta_time;
    float dy = input.move_y * SPEED * delta_time;
    
    result.x += dx;
    result.y += dy;
    result.facing_radians = input.aim_radians;
    result.last_sequence = input.sequence;
    
    return result;
}

}  // namespace

PredictedState ClientPrediction::Predict(
    const PlayerState& current_state,
    const InputCommand& input,
    float delta_time
) {
    PredictedState prediction;
    prediction.input_sequence = input.sequence;
    prediction.state = SimulateInput(current_state, input, delta_time);
    prediction.timestamp = CurrentTimeMs();
    
    // 입력 히스토리에 추가
    input_history_.push_back(input);
    if (input_history_.size() > MAX_PENDING_INPUTS) {
        input_history_.pop_front();
    }
    
    return prediction;
}

void ClientPrediction::SavePrediction(const PredictedState& prediction) {
    predictions_.push_back(prediction);
    if (predictions_.size() > MAX_PENDING_INPUTS) {
        predictions_.pop_front();
    }
    stats_.total_predictions++;
}

void ClientPrediction::AcknowledgeUpTo(std::uint32_t server_sequence) {
    last_acknowledged_ = server_sequence;
    
    // 확인된 시퀀스까지의 예측 제거
    while (!predictions_.empty() && 
           predictions_.front().input_sequence <= server_sequence) {
        predictions_.pop_front();
    }
    
    // 입력 히스토리도 정리
    while (!input_history_.empty() && 
           input_history_.front().sequence <= server_sequence) {
        input_history_.pop_front();
    }
}

std::vector<InputCommand> ClientPrediction::GetUnacknowledgedInputs(
    std::uint32_t after_sequence
) const {
    std::vector<InputCommand> result;
    for (const auto& input : input_history_) {
        if (input.sequence > after_sequence) {
            result.push_back(input);
        }
    }
    return result;
}

std::optional<PredictedState> ClientPrediction::GetLatestPrediction() const {
    if (predictions_.empty()) {
        return std::nullopt;
    }
    return predictions_.back();
}

std::optional<PredictedState> ClientPrediction::GetPrediction(
    std::uint32_t sequence
) const {
    for (const auto& pred : predictions_) {
        if (pred.input_sequence == sequence) {
            return pred;
        }
    }
    return std::nullopt;
}

void ClientPrediction::RecordPredictionResult(bool accurate) {
    if (accurate) {
        stats_.accurate_predictions++;
    }
}

}  // namespace pvpserver::netcode
