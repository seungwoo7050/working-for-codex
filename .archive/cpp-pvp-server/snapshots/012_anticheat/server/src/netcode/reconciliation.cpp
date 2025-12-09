#include "pvpserver/netcode/reconciliation.h"

#include <cmath>

namespace pvpserver::netcode {

namespace {

float CalculatePositionError(const PlayerState& a, const PlayerState& b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    return std::sqrt(dx * dx + dy * dy);
}

PlayerState SimulateInput(
    const PlayerState& state,
    const InputCommand& input,
    float delta_time
) {
    PlayerState result = state;
    
    constexpr float SPEED = 5.0f;
    
    float dx = input.move_x * SPEED * delta_time;
    float dy = input.move_y * SPEED * delta_time;
    
    result.x += dx;
    result.y += dy;
    result.facing_radians = input.aim_radians;
    result.last_sequence = input.sequence;
    
    return result;
}

}  // namespace

ReconciliationResult Reconciliation::Reconcile(
    const PlayerState& server_state,
    std::uint32_t server_input_sequence,
    ClientPrediction& prediction,
    float delta_time
) {
    ReconciliationResult result;
    stats_.total_reconciliations++;
    
    // 서버 시퀀스에 해당하는 예측 찾기
    auto predicted = prediction.GetPrediction(server_input_sequence);
    if (!predicted) {
        // 예측이 없으면 서버 상태 그대로 사용
        result.corrected_state = server_state;
        return result;
    }
    
    // 오차 계산
    result.position_error = CalculatePositionError(server_state, predicted->state);
    
    // 임계값 초과 시 리컨실리에이션
    if (result.position_error > POSITION_THRESHOLD) {
        result.mismatch_detected = true;
        stats_.mismatches++;
        
        // 미확인 입력 재시뮬레이션
        auto unacked_inputs = prediction.GetUnacknowledgedInputs(server_input_sequence);
        result.corrected_state = Resimulate(server_state, unacked_inputs, delta_time);
        
        // 예측 정확도 기록
        prediction.RecordPredictionResult(false);
    } else {
        result.corrected_state = predicted->state;
        prediction.RecordPredictionResult(true);
    }
    
    // 확인된 시퀀스까지 정리
    prediction.AcknowledgeUpTo(server_input_sequence);
    
    // 평균 오차 업데이트
    stats_.avg_position_error = 
        (stats_.avg_position_error * (stats_.total_reconciliations - 1) + result.position_error) 
        / stats_.total_reconciliations;
    
    return result;
}

PlayerState Reconciliation::SmoothCorrection(
    const PlayerState& current,
    const PlayerState& target,
    float blend_factor
) {
    PlayerState result = current;
    
    float t = std::clamp(blend_factor, 0.0f, 1.0f);
    
    result.x = current.x + t * (target.x - current.x);
    result.y = current.y + t * (target.y - current.y);
    result.facing_radians = current.facing_radians + 
        t * (target.facing_radians - current.facing_radians);
    
    // 정수 값은 즉시 갱신
    result.health = target.health;
    result.is_alive = target.is_alive;
    result.last_sequence = target.last_sequence;
    
    return result;
}

PlayerState Reconciliation::Resimulate(
    const PlayerState& server_state,
    const std::vector<InputCommand>& inputs,
    float delta_time
) {
    PlayerState state = server_state;
    
    for (const auto& input : inputs) {
        state = SimulateInput(state, input, delta_time);
    }
    
    return state;
}

}  // namespace pvpserver::netcode
