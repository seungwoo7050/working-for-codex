#pragma once

#include <string>
#include <unordered_map>

#include "pvpserver/anticheat/hit_validator.h"  // Vec3

namespace pvpserver::anticheat {

// 위반 타입
enum class ViolationType { NONE, SPEEDHACK, TELEPORT, WALLCLIP };

// 이동 검증 결과
struct MovementCheck {
    bool valid = true;
    float actual_speed = 0.0f;
    float max_allowed_speed = 0.0f;
    ViolationType violation = ViolationType::NONE;
    std::string details;
};

// 장애물 정보 (월핵 탐지용)
struct Obstacle {
    AABB bounds;
    bool is_solid;
};

// 플레이어 이동 상태
struct PlayerMovementState {
    bool is_sprinting = false;
    bool is_slowed = false;
    float speed_modifier = 1.0f;  // 아이템/스킬 효과 등
    Vec3 last_position;
    int64_t last_update_tick = 0;
    int violation_count = 0;
};

// 이동 검증기
class MovementValidator {
public:
    // 기본 속도 상수
    static constexpr float BASE_SPEED = 5.0f;
    static constexpr float SPRINT_MULTIPLIER = 1.5f;
    static constexpr float TOLERANCE = 1.1f;     // 10% 오차 허용
    static constexpr float TELEPORT_THRESHOLD_MULTIPLIER =
        3.0f;  // 최대 속도의 3배 이상이면 텔레포트

    // 이동 검증
    MovementCheck ValidateMovement(const std::string& player_id, const Vec3& old_pos,
                                   const Vec3& new_pos, float delta_time);

    // 플레이어 상태 관리
    void SetPlayerState(const std::string& player_id, const PlayerMovementState& state);
    void RemovePlayer(const std::string& player_id);
    PlayerMovementState* GetPlayerState(const std::string& player_id);

    // 장애물 설정 (맵 로드 시)
    void SetObstacles(const std::vector<Obstacle>& obstacles);
    void ClearObstacles();

    // 위반 카운터 조회
    int GetViolationCount(const std::string& player_id) const;

private:
    std::unordered_map<std::string, PlayerMovementState> states_;
    std::vector<Obstacle> obstacles_;

    // 속도핵 탐지
    bool DetectSpeedHack(const Vec3& old_pos, const Vec3& new_pos, float delta_time,
                         float max_speed, float& actual_speed);

    // 텔레포트 탐지
    bool DetectTeleport(const Vec3& old_pos, const Vec3& new_pos, float delta_time);

    // 월핵 탐지 (벽 통과)
    bool DetectWallClip(const Vec3& old_pos, const Vec3& new_pos);

    // 레이캐스트 (장애물 충돌)
    bool RaycastObstacles(const Vec3& start, const Vec3& end);
};

}  // namespace pvpserver::anticheat
