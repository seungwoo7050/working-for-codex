#include "pvpserver/anticheat/hit_validator.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace pvpserver::anticheat {

// Vec3 구현
float Vec3::Length() const { return std::sqrt(x * x + y * y + z * z); }

Vec3 Vec3::Normalized() const {
    float len = Length();
    if (len < 0.0001f) return {0, 0, 0};
    return {x / len, y / len, z / len};
}

float Vec3::Distance(const Vec3& a, const Vec3& b) { return (b - a).Length(); }

// AABB 구현
bool AABB::Contains(const Vec3& point) const {
    return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

// PlayerHitbox 구현
void PlayerHitbox::UpdateFromPosition(const Vec3& pos) {
    position = pos;

    // 전체 AABB (2m 높이, 0.5m 반지름 가정)
    bounds.min = {pos.x - 0.5f, pos.y, pos.z - 0.5f};
    bounds.max = {pos.x + 0.5f, pos.y + 2.0f, pos.z + 0.5f};

    // 몸통 캡슐 (0.4m ~ 1.6m)
    body.start = {pos.x, pos.y + 0.4f, pos.z};
    body.end = {pos.x, pos.y + 1.4f, pos.z};
    body.radius = 0.35f;

    // 머리 캡슐 (1.4m ~ 2.0m)
    head.start = {pos.x, pos.y + 1.5f, pos.z};
    head.end = {pos.x, pos.y + 1.9f, pos.z};
    head.radius = 0.15f;
}

// WorldState 구현
const PlayerState* WorldState::GetPlayer(const std::string& id) const {
    for (const auto& p : players) {
        if (p.player_id == id) return &p;
    }
    return nullptr;
}

// RaycastSystem 구현
std::optional<RaycastHit> RaycastSystem::Cast(const WorldState& world, const Vec3& origin,
                                              const Vec3& direction, float max_distance,
                                              const std::vector<std::string>& ignore_list) {
    Ray ray{origin, direction.Normalized()};
    std::optional<RaycastHit> closest_hit;
    float closest_t = max_distance;

    for (const auto& player : world.players) {
        // 무시 목록 확인
        if (std::find(ignore_list.begin(), ignore_list.end(), player.player_id) !=
            ignore_list.end()) {
            continue;
        }

        if (!player.is_alive) continue;

        PlayerHitbox hitbox = BuildHitbox(player);

        // AABB 먼저 확인 (빠른 거부)
        float t;
        if (!IntersectAABB(ray, hitbox.bounds, t)) continue;
        if (t > closest_t) continue;

        // 상세 히트박스 확인 (머리 먼저)
        float head_t, body_t;
        bool head_hit = IntersectCapsule(ray, hitbox.head, head_t);
        bool body_hit = IntersectCapsule(ray, hitbox.body, body_t);

        if (head_hit && head_t < closest_t) {
            closest_t = head_t;
            closest_hit = RaycastHit{player.player_id, ray.PointAt(head_t), Vec3{0, 1, 0}, head_t,
                                     HitboxType::HEAD};
        } else if (body_hit && body_t < closest_t) {
            closest_t = body_t;
            closest_hit = RaycastHit{player.player_id, ray.PointAt(body_t), Vec3{0, 0, 1}, body_t,
                                     HitboxType::BODY};
        }
    }

    return closest_hit;
}

bool RaycastSystem::IntersectAABB(const Ray& ray, const AABB& box, float& t_out) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        float origin = (i == 0) ? ray.origin.x : (i == 1) ? ray.origin.y : ray.origin.z;
        float dir = (i == 0) ? ray.direction.x : (i == 1) ? ray.direction.y : ray.direction.z;
        float bmin = (i == 0) ? box.min.x : (i == 1) ? box.min.y : box.min.z;
        float bmax = (i == 0) ? box.max.x : (i == 1) ? box.max.y : box.max.z;

        if (std::abs(dir) < 0.0001f) {
            if (origin < bmin || origin > bmax) return false;
        } else {
            float t1 = (bmin - origin) / dir;
            float t2 = (bmax - origin) / dir;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }

    t_out = tmin;
    return true;
}

bool RaycastSystem::IntersectCapsule(const Ray& ray, const Capsule& capsule, float& t_out) {
    // 캡슐을 선분으로 단순화하고 구체 교차 테스트
    Vec3 segment = capsule.end - capsule.start;
    Vec3 w0 = ray.origin - capsule.start;

    float a = ray.direction.Dot(ray.direction) -
              std::pow(ray.direction.Dot(segment.Normalized()), 2);
    float b = 2.0f * (ray.direction.Dot(w0) -
                      ray.direction.Dot(segment.Normalized()) * w0.Dot(segment.Normalized()));
    float c = w0.Dot(w0) - std::pow(w0.Dot(segment.Normalized()), 2) -
              capsule.radius * capsule.radius;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) return false;

    float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
    if (t < 0) {
        t = (-b + std::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) return false;
    }

    t_out = t;
    return true;
}

HitboxType RaycastSystem::GetHitboxType(const PlayerHitbox& hitbox, const Vec3& point) {
    // Y 좌표 기준으로 간단히 판정
    float local_y = point.y - hitbox.position.y;
    if (local_y > 1.4f) return HitboxType::HEAD;
    if (local_y > 0.4f) return HitboxType::BODY;
    return HitboxType::LIMB;
}

PlayerHitbox RaycastSystem::BuildHitbox(const PlayerState& player) {
    PlayerHitbox hitbox;
    hitbox.player_id = player.player_id;
    hitbox.UpdateFromPosition(player.position);
    return hitbox;
}

// WorldStateBuffer 구현
void WorldStateBuffer::SaveState(int64_t tick, const WorldState& state) {
    buffer_[head_] = {state.timestamp, state};
    head_ = (head_ + 1) % BUFFER_SIZE;
    if (count_ < BUFFER_SIZE) ++count_;
}

WorldState WorldStateBuffer::GetStateAt(int64_t timestamp) {
    if (count_ == 0) return {};

    // 가장 가까운 상태 찾기
    int best_idx = 0;
    int64_t best_diff = std::numeric_limits<int64_t>::max();

    for (int i = 0; i < count_; ++i) {
        int idx = (head_ - 1 - i + BUFFER_SIZE) % BUFFER_SIZE;
        int64_t diff = std::abs(buffer_[idx].timestamp - timestamp);
        if (diff < best_diff) {
            best_diff = diff;
            best_idx = idx;
        }
    }

    // MAX_REWIND_MS 제한 확인
    if (best_diff > MAX_REWIND_MS) {
        // 최신 상태 반환
        int latest = (head_ - 1 + BUFFER_SIZE) % BUFFER_SIZE;
        return buffer_[latest].state;
    }

    return buffer_[best_idx].state;
}

WorldState WorldStateBuffer::InterpolateState(const WorldState& a, const WorldState& b, float t) {
    WorldState result;
    result.tick = a.tick;
    result.timestamp = a.timestamp + static_cast<int64_t>((b.timestamp - a.timestamp) * t);

    // 플레이어 위치 보간
    for (const auto& pa : a.players) {
        PlayerState ps = pa;
        if (const auto* pb = b.GetPlayer(pa.player_id)) {
            ps.position =
                pa.position + (pb->position - pa.position) * t;
        }
        result.players.push_back(ps);
    }

    return result;
}

// HitValidator 구현
HitValidator::HitValidator() = default;

void HitValidator::RecordWorldState(int64_t tick, const WorldState& state) {
    state_buffer_.SaveState(tick, state);
}

HitResult HitValidator::ValidateHit(const HitRequest& request) {
    HitResult result;

    // 지연 보상: 클라이언트 타임스탬프 기준 월드 상태 가져오기
    WorldState world = state_buffer_.GetStateAt(request.client_timestamp);

    // 슈터가 존재하는지 확인
    const auto* shooter = world.GetPlayer(request.shooter_id);
    if (!shooter || !shooter->is_alive) {
        result.reject_reason = "shooter_not_alive";
        return result;
    }

    // 레이캐스트 실행
    auto hit = raycast_system_.Cast(world, request.origin, request.direction, request.max_distance,
                                    {request.shooter_id});

    if (!hit) {
        result.reject_reason = "no_hit";
        return result;
    }

    // 타겟 확인 (클라이언트가 주장한 타겟과 일치하는지)
    if (!request.target_id.empty() && hit->entity_id != request.target_id) {
        // 히트는 있지만 다른 플레이어 - 그래도 유효
    }

    result.valid = true;
    result.target_id = hit->entity_id;
    result.hit_point = hit->hit_point;
    result.hitbox = hit->hitbox;
    result.damage = CalculateDamage(hit->hitbox);

    return result;
}

float HitValidator::CalculateDamage(HitboxType hitbox, float base_damage) {
    switch (hitbox) {
        case HitboxType::HEAD:
            return base_damage * HEADSHOT_MULTIPLIER;
        case HitboxType::BODY:
            return base_damage;
        case HitboxType::LIMB:
            return base_damage * LIMB_MULTIPLIER;
        default:
            return 0.0f;
    }
}

}  // namespace pvpserver::anticheat
