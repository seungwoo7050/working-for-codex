#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pvpserver::anticheat {

// 간단한 3D 벡터
struct Vec3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3() = default;
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    float Dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    float Length() const;
    Vec3 Normalized() const;
    static float Distance(const Vec3& a, const Vec3& b);
};

// 히트박스 타입
enum class HitboxType { NONE, HEAD, BODY, LIMB };

// AABB (Axis-Aligned Bounding Box)
struct AABB {
    Vec3 min;
    Vec3 max;

    bool Contains(const Vec3& point) const;
};

// 캡슐 형태 (캐릭터 히트박스용)
struct Capsule {
    Vec3 start;
    Vec3 end;
    float radius;
};

// 플레이어 히트박스 구성
struct PlayerHitbox {
    std::string player_id;
    Vec3 position;
    AABB bounds;      // 전체 AABB
    Capsule body;     // 몸통 캡슐
    Capsule head;     // 머리 캡슐

    void UpdateFromPosition(const Vec3& pos);
};

// 레이
struct Ray {
    Vec3 origin;
    Vec3 direction;

    Vec3 PointAt(float t) const { return origin + direction * t; }
};

// 레이캐스트 히트 결과
struct RaycastHit {
    std::string entity_id;
    Vec3 hit_point;
    Vec3 hit_normal;
    float distance;
    HitboxType hitbox;
};

// 월드 상태 스냅샷
struct PlayerState {
    std::string player_id;
    Vec3 position;
    Vec3 velocity;
    float health;
    bool is_alive;
};

struct WorldState {
    int64_t tick;
    int64_t timestamp;
    std::vector<PlayerState> players;

    const PlayerState* GetPlayer(const std::string& id) const;
};

// 히트 요청
struct HitRequest {
    std::string shooter_id;
    std::string target_id;
    Vec3 origin;
    Vec3 direction;
    int64_t client_timestamp;  // 지연 보상용
    float max_distance = 100.0f;
};

// 히트 검증 결과
struct HitResult {
    bool valid = false;
    std::string target_id;
    Vec3 hit_point;
    float damage = 0.0f;
    HitboxType hitbox = HitboxType::NONE;
    std::string reject_reason;
};

// 레이캐스트 시스템
class RaycastSystem {
public:
    std::optional<RaycastHit> Cast(const WorldState& world, const Vec3& origin,
                                   const Vec3& direction, float max_distance,
                                   const std::vector<std::string>& ignore_list);

private:
    bool IntersectAABB(const Ray& ray, const AABB& box, float& t_out);
    bool IntersectCapsule(const Ray& ray, const Capsule& capsule, float& t_out);
    HitboxType GetHitboxType(const PlayerHitbox& hitbox, const Vec3& point);

    PlayerHitbox BuildHitbox(const PlayerState& player);
};

// 월드 상태 버퍼 (지연 보상용)
class WorldStateBuffer {
public:
    static constexpr int BUFFER_SIZE = 64;       // 약 1초 @ 60 TPS
    static constexpr int MAX_REWIND_MS = 200;    // 최대 200ms 되감기

    void SaveState(int64_t tick, const WorldState& state);
    WorldState GetStateAt(int64_t timestamp);

private:
    struct TimestampedState {
        int64_t timestamp = 0;
        WorldState state;
    };

    std::array<TimestampedState, BUFFER_SIZE> buffer_;
    int head_ = 0;
    int count_ = 0;

    WorldState InterpolateState(const WorldState& a, const WorldState& b, float t);
};

// 히트 검증기
class HitValidator {
public:
    HitValidator();

    // 월드 상태 저장 (매 틱 호출)
    void RecordWorldState(int64_t tick, const WorldState& state);

    // 히트 검증
    HitResult ValidateHit(const HitRequest& request);

    // 데미지 계산
    float CalculateDamage(HitboxType hitbox, float base_damage = 20.0f);

private:
    RaycastSystem raycast_system_;
    WorldStateBuffer state_buffer_;

    static constexpr float BASE_DAMAGE = 20.0f;
    static constexpr float HEADSHOT_MULTIPLIER = 2.5f;
    static constexpr float LIMB_MULTIPLIER = 0.75f;
};

}  // namespace pvpserver::anticheat
