#include "pvpserver/anticheat/movement_validator.h"

#include <cmath>

namespace pvpserver::anticheat {

MovementCheck MovementValidator::ValidateMovement(const std::string& player_id, const Vec3& old_pos,
                                                  const Vec3& new_pos, float delta_time) {
    MovementCheck result;

    if (delta_time <= 0.0f) {
        result.valid = true;
        return result;
    }

    // 플레이어 상태 가져오기 (없으면 기본 상태)
    auto it = states_.find(player_id);
    PlayerMovementState state;
    if (it != states_.end()) {
        state = it->second;
    }

    // 최대 허용 속도 계산
    float max_speed = BASE_SPEED;
    if (state.is_sprinting) {
        max_speed *= SPRINT_MULTIPLIER;
    }
    if (state.is_slowed) {
        max_speed *= 0.5f;  // 슬로우 상태
    }
    max_speed *= state.speed_modifier;
    max_speed *= TOLERANCE;  // 네트워크 지연 허용

    result.max_allowed_speed = max_speed;

    // 텔레포트 감지 (최우선)
    if (DetectTeleport(old_pos, new_pos, delta_time)) {
        result.valid = false;
        result.violation = ViolationType::TELEPORT;
        result.actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
        result.details = "Position jump detected";

        if (it != states_.end()) {
            it->second.violation_count++;
        }
        return result;
    }

    // 월핵 감지
    if (DetectWallClip(old_pos, new_pos)) {
        result.valid = false;
        result.violation = ViolationType::WALLCLIP;
        result.actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
        result.details = "Wall clipping detected";

        if (it != states_.end()) {
            it->second.violation_count++;
        }
        return result;
    }

    // 속도핵 감지
    float actual_speed;
    if (DetectSpeedHack(old_pos, new_pos, delta_time, max_speed, actual_speed)) {
        result.valid = false;
        result.violation = ViolationType::SPEEDHACK;
        result.actual_speed = actual_speed;
        result.details = "Speed exceeds maximum allowed";

        if (it != states_.end()) {
            it->second.violation_count++;
        }
        return result;
    }

    result.valid = true;
    result.actual_speed = actual_speed;

    // 상태 업데이트
    if (it != states_.end()) {
        it->second.last_position = new_pos;
    }

    return result;
}

void MovementValidator::SetPlayerState(const std::string& player_id,
                                       const PlayerMovementState& state) {
    states_[player_id] = state;
}

void MovementValidator::RemovePlayer(const std::string& player_id) { states_.erase(player_id); }

PlayerMovementState* MovementValidator::GetPlayerState(const std::string& player_id) {
    auto it = states_.find(player_id);
    return it != states_.end() ? &it->second : nullptr;
}

void MovementValidator::SetObstacles(const std::vector<Obstacle>& obstacles) {
    obstacles_ = obstacles;
}

void MovementValidator::ClearObstacles() { obstacles_.clear(); }

int MovementValidator::GetViolationCount(const std::string& player_id) const {
    auto it = states_.find(player_id);
    return it != states_.end() ? it->second.violation_count : 0;
}

bool MovementValidator::DetectSpeedHack(const Vec3& old_pos, const Vec3& new_pos, float delta_time,
                                        float max_speed, float& actual_speed) {
    actual_speed = Vec3::Distance(old_pos, new_pos) / delta_time;
    return actual_speed > max_speed;
}

bool MovementValidator::DetectTeleport(const Vec3& old_pos, const Vec3& new_pos, float delta_time) {
    float distance = Vec3::Distance(old_pos, new_pos);
    float max_possible =
        BASE_SPEED * SPRINT_MULTIPLIER * delta_time * TELEPORT_THRESHOLD_MULTIPLIER;

    return distance > max_possible;
}

bool MovementValidator::DetectWallClip(const Vec3& old_pos, const Vec3& new_pos) {
    if (obstacles_.empty()) return false;
    return RaycastObstacles(old_pos, new_pos);
}

bool MovementValidator::RaycastObstacles(const Vec3& start, const Vec3& end) {
    for (const auto& obstacle : obstacles_) {
        if (!obstacle.is_solid) continue;

        // 간단한 AABB 선분 교차 테스트
        Vec3 dir = end - start;
        float tmin = 0.0f;
        float tmax = 1.0f;

        for (int i = 0; i < 3; ++i) {
            float s = (i == 0) ? start.x : (i == 1) ? start.y : start.z;
            float d = (i == 0) ? dir.x : (i == 1) ? dir.y : dir.z;
            float bmin = (i == 0) ? obstacle.bounds.min.x
                                  : (i == 1) ? obstacle.bounds.min.y : obstacle.bounds.min.z;
            float bmax = (i == 0) ? obstacle.bounds.max.x
                                  : (i == 1) ? obstacle.bounds.max.y : obstacle.bounds.max.z;

            if (std::abs(d) < 0.0001f) {
                if (s < bmin || s > bmax) {
                    tmin = 2.0f;  // 교차 없음
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
            return true;  // 장애물과 교차
        }
    }

    return false;
}

}  // namespace pvpserver::anticheat
