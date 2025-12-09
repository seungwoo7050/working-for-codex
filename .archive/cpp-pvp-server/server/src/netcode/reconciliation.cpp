// [FILE]
// - 목적: 리컨실리에이션 (서버 상태와 클라이언트 예측 동기화)
// - 주요 역할: 서버 권위 상태 수신 시 클라이언트 예측 오차 보정
// - 관련 클론 가이드 단계: [v1.4.1] 클라이언트 예측/리컨실리에이션
// - 권장 읽는 순서: Reconcile → SmoothCorrection → Resimulate
//
// [LEARN] 리컨실리에이션 = 예측 오류 수정.
//         - 서버 상태 수신 → 예측과 비교 → 오차 > 임계값이면 보정
//         - 미확인 입력 재시뮬레이션으로 부드러운 보정
//         - "rubberbanding" (플레이어 튕김) 방지가 목표

#include "pvpserver/netcode/reconciliation.h"

#include <cmath>

namespace pvpserver::netcode {

namespace {

// 두 상태 간 위치 오차 계산
float CalculatePositionError(const PlayerState& a, const PlayerState& b) {
    float dx = static_cast<float>(a.x - b.x);
    float dy = static_cast<float>(a.y - b.y);
    return std::sqrt(dx * dx + dy * dy);
}

// [LEARN] 단일 입력 시뮬레이션 - 예측/재시뮬레이션에 사용
PlayerState SimulateInput(
    const PlayerState& state,
    const InputCommand& input,
    float delta_time
) {
    PlayerState result = state;
    
    constexpr float SPEED = 5.0f;  // 이동 속도 상수
    
    // 입력 방향으로 이동
    float dx = input.move_x * SPEED * delta_time;
    float dy = input.move_y * SPEED * delta_time;
    
    result.x += dx;
    result.y += dy;
    result.facing_radians = input.aim_radians;
    result.last_sequence = input.sequence;
    
    return result;
}

}  // namespace

// [Order 1] Reconcile - 리컨실리에이션 메인 함수
// [LEARN] 서버 상태 수신 시 호출:
//         1. 서버 시퀀스에 해당하는 예측 찾기
//         2. 오차 계산
//         3. 임계값 초과 시 재시뮬레이션
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
    
    // 오차 계산 (예측 vs 서버)
    result.position_error = CalculatePositionError(server_state, predicted->state);
    
    // 임계값 초과 시 리컨실리에이션 수행
    if (result.position_error > POSITION_THRESHOLD) {
        result.mismatch_detected = true;
        stats_.mismatches++;
        
        // [핵심] 미확인 입력 재시뮬레이션
        // - 서버가 확인한 시점부터 현재까지의 입력을 다시 적용
        auto unacked_inputs = prediction.GetUnacknowledgedInputs(server_input_sequence);
        result.corrected_state = Resimulate(server_state, unacked_inputs, delta_time);
        
        // 예측 정확도 기록 (통계용)
        prediction.RecordPredictionResult(false);
    } else {
        result.corrected_state = predicted->state;
        prediction.RecordPredictionResult(true);
    }
    
    // 확인된 시퀀스까지 버퍼 정리
    prediction.AcknowledgeUpTo(server_input_sequence);
    
    // 평균 오차 업데이트 (런닝 평균)
    stats_.avg_position_error = 
        (stats_.avg_position_error * (stats_.total_reconciliations - 1) + result.position_error) 
        / stats_.total_reconciliations;
    
    return result;
}

// [Order 2] SmoothCorrection - 부드러운 보정 (갑작스러운 튕김 방지)
// [LEARN] 선형 보간(Lerp)으로 점진적 위치 수정.
//         즉각 보정 = 눈에 띄는 텔레포트 → 나쁜 UX
PlayerState Reconciliation::SmoothCorrection(
    const PlayerState& current,
    const PlayerState& target,
    float blend_factor
) {
    PlayerState result = current;
    
    float t = std::clamp(blend_factor, 0.0f, 1.0f);
    
    // 위치는 선형 보간
    result.x = current.x + t * (target.x - current.x);
    result.y = current.y + t * (target.y - current.y);
    result.facing_radians = current.facing_radians + 
        t * (target.facing_radians - current.facing_radians);
    
    // HP, 생존 여부 등 정수 값은 즉시 갱신 (보간 불가)
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
