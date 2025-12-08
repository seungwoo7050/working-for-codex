// [FILE]
// - 목적: 이동 검증 (속도핵/텔레포트/월핵 감지)
// - 주요 역할: 클라이언트 이동 입력을 서버에서 물리적 타당성 검증
// - 관련 클론 가이드 단계: [CG-02.10] 안티치트 시스템
// - 권장 읽는 순서: ValidateMovement → DetectSpeedHack → DetectTeleport → DetectWallClip
//
// [LEARN] 치트 유형별 감지:
//         - 속도핵: 허용 속도 초과 이동
//         - 텔레포트핵: 물리적으로 불가능한 거리 점프
//         - 월핵(NoClip): 벽을 통과하는 이동

#include "pvpserver/anticheat/movement_validator.h"

#include <cmath>

namespace pvpserver::anticheat {

// [Order 1] ValidateMovement - 이동 검증 메인 함수
// - 입력: 이전 위치, 새 위치, 경과 시간
// - 출력: 유효성 + 위반 유형
// [LEARN] 모든 이동은 서버에서 검증. "클라이언트 신뢰 금지"
MovementCheck MovementValidator::ValidateMovement(const std::string& player_id, const Vec3& old_pos,
                                                  const Vec3& new_pos, float delta_time) {
    MovementCheck result;

    if (delta_time <= 0.0f) {
        result.valid = true;
        return result;
    }

    // 플레이어 상태 가져오기 (없으면 기본 상태 생성)
    auto it = states_.find(player_id);
    if (it == states_.end()) {
        states_[player_id] = PlayerMovementState{};
        it = states_.find(player_id);
    }
    PlayerMovementState& state = it->second;

    // 최대 허용 속도 계산 (상태별 보정)
    float max_speed = BASE_SPEED;
    if (state.is_sprinting) {
        max_speed *= SPRINT_MULTIPLIER;  // 달리기 중
    }
    if (state.is_slowed) {
        max_speed *= 0.5f;  // 슬로우 상태 (디버프)
    }
    max_speed *= state.speed_modifier;   // 아이템/스킬 보정
    max_speed *= TOLERANCE;              // 네트워크 지연 허용 마진

    result.max_allowed_speed = max_speed;

    // [검사 1] 텔레포트 감지 (최우선 - 가장 심각한 치트)
    if (DetectTeleport(old_pos, new_pos, delta_time)) {
        result.valid = false;
        result.violation = ViolationType::TELEPORT;
        result.actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
        result.details = "Position jump detected";
        state.violation_count++;
        return result;
    }

    // [검사 2] 월핵 감지 (벽 통과)
    if (DetectWallClip(old_pos, new_pos)) {
        result.valid = false;
        result.violation = ViolationType::WALLCLIP;
        result.actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
        result.details = "Wall clipping detected";
        state.violation_count++;
        return result;
    }

    // [검사 3] 속도핵 감지
    float actual_speed;
    if (DetectSpeedHack(old_pos, new_pos, delta_time, max_speed, actual_speed)) {
        result.valid = false;
        result.violation = ViolationType::SPEEDHACK;
        result.actual_speed = actual_speed;
        result.details = "Speed exceeds maximum allowed";
        state.violation_count++;
        return result;
    }

    result.valid = true;
    result.actual_speed = actual_speed;

    // 상태 업데이트 (다음 검증용)
    state.last_position = new_pos;

    return result;
}

// 플레이어 상태 관리 헬퍼 함수들
void MovementValidator::SetPlayerState(const std::string& player_id,
                                       const PlayerMovementState& state) {
    states_[player_id] = state;
}

void MovementValidator::RemovePlayer(const std::string& player_id) { states_.erase(player_id); }

PlayerMovementState* MovementValidator::GetPlayerState(const std::string& player_id) {
    auto it = states_.find(player_id);
    return it != states_.end() ? &it->second : nullptr;
}

// 장애물(벽) 정보 설정 - 월핵 감지용
void MovementValidator::SetObstacles(const std::vector<Obstacle>& obstacles) {
    obstacles_ = obstacles;
}

void MovementValidator::ClearObstacles() { obstacles_.clear(); }

// 위반 횟수 조회 (누적되면 밴 대상)
int MovementValidator::GetViolationCount(const std::string& player_id) const {
    auto it = states_.find(player_id);
    return it != states_.end() ? it->second.violation_count : 0;
}

// [Order 2] DetectSpeedHack - 속도핵 감지
// [LEARN] 단순 계산: 이동 거리 / 시간 > 최대 속도면 치트
bool MovementValidator::DetectSpeedHack(const Vec3& old_pos, const Vec3& new_pos, float delta_time,
                                        float max_speed, float& actual_speed) {
    actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
    return actual_speed > max_speed;
}

// [Order 3] DetectTeleport - 텔레포트핵 감지
// [LEARN] 물리적으로 불가능한 거리 이동 감지.
//         TELEPORT_THRESHOLD_MULTIPLIER로 임계값 설정.
bool MovementValidator::DetectTeleport(const Vec3& old_pos, const Vec3& new_pos, float delta_time) {
    float distance = Vec3::Distance(old_pos, new_pos);
    // 최대 달리기 속도로도 불가능한 거리 = 텔레포트
    float max_possible =
        BASE_SPEED * SPRINT_MULTIPLIER * delta_time * TELEPORT_THRESHOLD_MULTIPLIER;

    return distance > max_possible;
}

// [Order 4] DetectWallClip - 월핵(NoClip) 감지
// [LEARN] 이전 위치 → 새 위치 사이에 벽이 있으면 치트
bool MovementValidator::DetectWallClip(const Vec3& old_pos, const Vec3& new_pos) {
    if (obstacles_.empty()) return false;  // 장애물 정보 없으면 스킵
    return RaycastObstacles(old_pos, new_pos);
}

// [Order 5] RaycastObstacles - 선분-장애물 교차 검사
// [LEARN] 이동 경로가 장애물(AABB)과 교차하는지 확인.
//         슬랩 알고리즘으로 선분-AABB 교차 판정.
bool MovementValidator::RaycastObstacles(const Vec3& start, const Vec3& end) {
    for (const auto& obstacle : obstacles_) {
        if (!obstacle.is_solid) continue;  // 통과 가능 장애물 스킵

        // 선분-AABB 교차 테스트 (슬랩 알고리즘)
        Vec3 dir = end - start;
        float tmin = 0.0f;
        float tmax = 1.0f;  // 선분이므로 [0, 1] 범위

        for (int i = 0; i < 3; ++i) {
            float s = (i == 0) ? start.x : (i == 1) ? start.y : start.z;
            float d = (i == 0) ? dir.x : (i == 1) ? dir.y : dir.z;
            float bmin = (i == 0) ? obstacle.bounds.min.x
                                  : (i == 1) ? obstacle.bounds.min.y : obstacle.bounds.min.z;
            float bmax = (i == 0) ? obstacle.bounds.max.x
                                  : (i == 1) ? obstacle.bounds.max.y : obstacle.bounds.max.z;

            if (std::abs(d) < 0.0001f) {
                // 평행한 경우
                if (s < bmin || s > bmax) {
                    tmin = 2.0f;  // 교차 없음 표시
                    break;
                }
            } else {
                float t1 = (bmin - s) / d;
                float t2 = (bmax - s) / d;
                if (t1 > t2) std::swap(t1, t2);
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
            }
        }

        if (tmin <= tmax && tmin <= 1.0f && tmax >= 0.0f) {
            return true;  // 장애물과 교차 = 월핵 감지
        }
    }

    return false;
}

// ========================================================================
// [Reader Notes] 이동 검증 시스템
// ========================================================================
// 1. 치트 유형별 감지 전략
//    - 속도핵: distance / time > max_speed
//    - 텔레포트: 물리적 불가능한 점프 거리
//    - 월핵: 이동 경로가 벽 통과
//
// 2. TOLERANCE (허용 마진)
//    - 네트워크 지연, 프레임 불안정 고려
//    - 너무 엄격하면 정상 플레이어도 오탐
//    - 보통 1.2~1.5 배수 사용
//
// 3. violation_count (위반 횟수 누적)
//    - 1회 위반 = 경고 또는 보정
//    - N회 이상 = 밴 대상
//    - AnomalyDetector와 연계
//
// 4. 장애물 정보 (obstacles_)
//    - 맵의 벽/장애물 AABB 목록
//    - 맵 로드 시 SetObstacles()로 설정
//    - 월핵 감지에 필수
//
// [CG-02.10] 안티치트 - 이동 행동 검증
// ========================================================================

}  // namespace pvpserver::anticheat
