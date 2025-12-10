// [FILE]
// - 목적: 클라이언트 측 예측(Client-Side Prediction) 시스템
// - 주요 역할: 입력 즉시 반영, 서버 응답 전 로컬 시뮬레이션
// - 관련 클론 가이드 단계: [v1.4.1] 클라이언트 예측/리컨실리에이션
// - 권장 읽는 순서: SimulateInput → Predict → AcknowledgeUpTo
//
// [LEARN] 클라이언트 예측은 네트워크 지연을 숨기는 핵심 기법.
//         클라이언트가 입력 즉시 로컬에서 시뮬레이션하고,
//         나중에 서버 결과와 비교하여 보정(리컨실리에이션)함.
//         이렇게 하면 100ms 지연에도 즉각적인 반응감을 유지.

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

// [Order 1] SimulateInput - 입력에 따른 상태 시뮬레이션
// - 서버와 동일한 물리 로직을 클라이언트에서도 실행
// - 중요: 서버와 100% 동일해야 예측 정확도가 높아짐
PlayerState SimulateInput(
    const PlayerState& state,
    const InputCommand& input,
    float delta_time
) {
    PlayerState result = state;
    
    constexpr float SPEED = 5.0f;  // 초당 이동 거리 (서버와 동일값!)
    
    float dx = input.move_x * SPEED * delta_time;
    float dy = input.move_y * SPEED * delta_time;
    
    result.x += dx;
    result.y += dy;
    result.facing_radians = input.aim_radians;
    result.last_sequence = input.sequence;
    
    return result;
}

}  // namespace

// [Order 2] Predict - 예측 실행 + 입력 히스토리 저장
// - 클라이언트가 입력 시 호출
// - 예측 결과 반환 + 나중에 리컨실리에이션용으로 입력 저장
PredictedState ClientPrediction::Predict(
    const PlayerState& current_state,
    const InputCommand& input,
    float delta_time
) {
    PredictedState prediction;
    prediction.input_sequence = input.sequence;
    prediction.state = SimulateInput(current_state, input, delta_time);
    prediction.timestamp = CurrentTimeMs();
    
    // 입력 히스토리에 추가 (리컨실리에이션용)
    input_history_.push_back(input);
    if (input_history_.size() > MAX_PENDING_INPUTS) {
        input_history_.pop_front();  // 오래된 입력 제거
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

// [Order 3] AcknowledgeUpTo - 서버 확인 처리
// - 서버가 특정 시퀀스까지 처리했음을 알림
// - 확인된 예측/입력 히스토리 정리
// [LEARN] 서버가 시퀀스 N을 확인하면, N 이하의 예측은 더 이상 필요 없음.
//         메모리 절약 + 리컨실리에이션 범위 축소
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

// [Order 4] GetUnacknowledgedInputs - 미확인 입력 조회
// - 리컨실리에이션 시 서버 상태부터 재시뮬레이션할 입력 목록
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

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 넷코드 개념:
//
// 1. 클라이언트 측 예측 (Client-Side Prediction)
//    - 입력 즉시 로컬에서 시뮬레이션 (서버 응답 기다리지 않음)
//    - 네트워크 지연이 있어도 즉각적인 반응감 제공
//    - 단점: 서버 결과와 불일치 시 "순간이동" 발생 가능
//
// 2. 입력 시퀀스 번호
//    - 각 입력에 순차적 번호 부여
//    - 서버가 "시퀀스 N까지 처리함"으로 응답
//    - 클라이언트는 N 이후 입력만 재시뮬레이션
//
// 3. 결정론적 시뮬레이션 (Deterministic Simulation)
//    - 같은 초기 상태 + 같은 입력 = 같은 결과
//    - 서버/클라이언트가 동일한 물리 상수 사용 필수
//    - 부동소수점 정밀도 차이도 문제가 될 수 있음
//
// 관련 설계 문서:
// - design/v1.4.1-prediction.md (클라이언트 예측 상세)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/netcode/reconciliation.cpp (서버 보정)
// - server/src/netcode/lag_compensation.cpp (히트 판정 보정)
// ================================================================================
