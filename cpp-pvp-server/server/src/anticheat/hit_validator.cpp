// [FILE]
// - 목적: 서버 측 히트 검증 (안티치트)
// - 주요 역할: 클라이언트 히트 요청을 서버에서 레이캐스트로 검증
// - 관련 클론 가이드 단계: [CG-02.10] 안티치트 시스템
// - 권장 읽는 순서: PlayerHitbox → RaycastSystem::Cast → HitValidator
//
// [LEARN] 클라이언트가 "맞았다"고 보고해도 서버가 검증해야 함.
//         히트박스(충돌 영역)를 플레이어 위치에서 생성하고,
//         서버에서 직접 레이캐스트해서 실제로 맞았는지 확인.

#include "pvpserver/anticheat/hit_validator.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace pvpserver::anticheat {

// [Order 1] Vec3 - 3차원 벡터 유틸리티
// [LEARN] 게임에서 가장 많이 쓰는 수학 구조.
//         C에서 struct { float x,y,z; }로 정의하고 함수를 따로 만들던 것을
//         C++에서는 멤버 함수로 통합.
float Vec3::Length() const { return std::sqrt(x * x + y * y + z * z); }

Vec3 Vec3::Normalized() const {
    float len = Length();
    if (len < 0.0001f) return {0, 0, 0};  // 제로 벡터 방지
    return {x / len, y / len, z / len};
}

float Vec3::Distance(const Vec3& a, const Vec3& b) { return (b - a).Length(); }

// [Order 2] AABB - 축 정렬 경계 상자 (Axis-Aligned Bounding Box)
// [LEARN] 가장 단순한 충돌 영역. 회전하지 않는 직육면체.
//         빠른 거부 테스트(broad phase)에 사용.
bool AABB::Contains(const Vec3& point) const {
    return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

// [Order 3] PlayerHitbox - 플레이어 충돌 영역 생성
// - 전체 AABB + 몸통 캡슐 + 머리 캡슐
// - 머리 적중 시 추가 데미지 등 구분 가능
void PlayerHitbox::UpdateFromPosition(const Vec3& pos) {
    position = pos;

    // 전체 AABB (2m 높이, 0.5m 반지름 가정)
    bounds.min = {pos.x - 0.5f, pos.y, pos.z - 0.5f};
    bounds.max = {pos.x + 0.5f, pos.y + 2.0f, pos.z + 0.5f};

    // 몸통 캡슐 (0.4m ~ 1.4m 높이)
    body.start = {pos.x, pos.y + 0.4f, pos.z};
    body.end = {pos.x, pos.y + 1.4f, pos.z};
    body.radius = 0.35f;

    // 머리 캡슐 (1.5m ~ 1.9m 높이, 헤드샷용)
    head.start = {pos.x, pos.y + 1.5f, pos.z};
    head.end = {pos.x, pos.y + 1.9f, pos.z};
    head.radius = 0.15f;
}

// WorldState 구현 - 특정 플레이어 조회
const PlayerState* WorldState::GetPlayer(const std::string& id) const {
    for (const auto& p : players) {
        if (p.player_id == id) return &p;
    }
    return nullptr;
}

// [Order 4] RaycastSystem::Cast - 서버 측 레이캐스트
// - 발사 위치 + 방향으로 플레이어 히트 판정
// - AABB로 빠른 거부 → 캡슐로 정밀 검사
// [LEARN] 레이캐스트는 "광선이 무엇과 교차하는지" 판정.
//         FPS/TPS 게임의 총알 히트 판정에 필수.
std::optional<RaycastHit> RaycastSystem::Cast(const WorldState& world, const Vec3& origin,
                                              const Vec3& direction, float max_distance,
                                              const std::vector<std::string>& ignore_list) {
    Ray ray{origin, direction.Normalized()};
    std::optional<RaycastHit> closest_hit;
    float closest_t = max_distance;

    for (const auto& player : world.players) {
        // 무시 목록 확인 (자기 자신 등)
        if (std::find(ignore_list.begin(), ignore_list.end(), player.player_id) !=
            ignore_list.end()) {
            continue;
        }

        if (!player.is_alive) continue;  // 죽은 플레이어 스킵

        PlayerHitbox hitbox = BuildHitbox(player);

        // [Broad Phase] AABB 먼저 확인 - 빠른 거부
        float t;
        if (!IntersectAABB(ray, hitbox.bounds, t)) continue;
        if (t > closest_t) continue;  // 이미 더 가까운 히트 있음

        // [Narrow Phase] 상세 히트박스 확인 (머리 먼저 = 헤드샷 우선)
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

// [Order 5] IntersectAABB - 레이-AABB 교차 검사
// [LEARN] 슬랩(Slab) 알고리즘: 각 축별로 교차 구간 계산 후 교집합
bool RaycastSystem::IntersectAABB(const Ray& ray, const AABB& box, float& t_out) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();

    // 각 축(X, Y, Z)별로 교차 구간 계산
    for (int i = 0; i < 3; ++i) {
        float origin = (i == 0) ? ray.origin.x : (i == 1) ? ray.origin.y : ray.origin.z;
        float dir = (i == 0) ? ray.direction.x : (i == 1) ? ray.direction.y : ray.direction.z;
        float bmin = (i == 0) ? box.min.x : (i == 1) ? box.min.y : box.min.z;
        float bmax = (i == 0) ? box.max.x : (i == 1) ? box.max.y : box.max.z;

        if (std::abs(dir) < 0.0001f) {
            // 평행한 경우: 범위 밖이면 교차 안 함
            if (origin < bmin || origin > bmax) return false;
        } else {
            float t1 = (bmin - origin) / dir;
            float t2 = (bmax - origin) / dir;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);  // 교차 구간의 최대 시작점
            tmax = std::min(tmax, t2);  // 교차 구간의 최소 끝점
            if (tmin > tmax) return false;  // 교차 구간 없음
        }
    }

    t_out = tmin;
    return true;
}

// [Order 6] IntersectCapsule - 레이-캡슐 교차 검사
// [LEARN] 캡슐 = 선분(중심축) + 반지름의 구체.
//         인체 형태를 근사하는 가장 일반적인 히트박스 모양.
//         복잡한 메쉬 충돌 대신 수학적으로 해석 가능.
bool RaycastSystem::IntersectCapsule(const Ray& ray, const Capsule& capsule, float& t_out) {
    // 캡슐을 선분으로 단순화하고 원통 교차 테스트
    Vec3 segment = capsule.end - capsule.start;
    Vec3 w0 = ray.origin - capsule.start;

    // 2차 방정식 계수 (at² + bt + c = 0)
    float a = ray.direction.Dot(ray.direction) -
              std::pow(ray.direction.Dot(segment.Normalized()), 2);
    float b = 2.0f * (ray.direction.Dot(w0) -
                      ray.direction.Dot(segment.Normalized()) * w0.Dot(segment.Normalized()));
    float c = w0.Dot(w0) - std::pow(w0.Dot(segment.Normalized()), 2) -
              capsule.radius * capsule.radius;

    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) return false;  // 교차 없음

    // 가까운 교점 먼저 시도
    float t = (-b - std::sqrt(discriminant)) / (2.0f * a);
    if (t < 0) {
        t = (-b + std::sqrt(discriminant)) / (2.0f * a);
        if (t < 0) return false;  // 레이 뒤에서 교차
    }

    t_out = t;
    return true;
}

// GetHitboxType - 충돌 지점에서 히트박스 타입 결정
HitboxType RaycastSystem::GetHitboxType(const PlayerHitbox& hitbox, const Vec3& point) {
    // Y 좌표 기준으로 간단히 판정
    float local_y = point.y - hitbox.position.y;
    if (local_y > 1.4f) return HitboxType::HEAD;    // 머리
    if (local_y > 0.4f) return HitboxType::BODY;    // 몸통
    return HitboxType::LIMB;                         // 팔다리
}

// BuildHitbox - PlayerState에서 히트박스 생성
PlayerHitbox RaycastSystem::BuildHitbox(const PlayerState& player) {
    PlayerHitbox hitbox;
    hitbox.player_id = player.player_id;
    hitbox.UpdateFromPosition(player.position);
    return hitbox;
}

// [Order 7] WorldStateBuffer - 과거 월드 상태 저장 (지연 보상용)
// [LEARN] 링버퍼로 최근 N개 상태 저장.
//         클라이언트 요청 시점의 월드 상태를 복원하여 공정한 히트 판정.
void WorldStateBuffer::SaveState(int64_t tick, const WorldState& state) {
    buffer_[head_] = {state.timestamp, state};
    head_ = (head_ + 1) % BUFFER_SIZE;
    if (count_ < BUFFER_SIZE) ++count_;
}

// GetStateAt - 특정 타임스탬프에 가장 가까운 상태 반환
WorldState WorldStateBuffer::GetStateAt(int64_t timestamp) {
    if (count_ == 0) return {};

    // 가장 가까운 상태 찾기 (선형 탐색)
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

    // MAX_REWIND_MS 제한 확인 - 너무 오래된 요청은 거부
    if (best_diff > MAX_REWIND_MS) {
        // 최신 상태 반환 (악용 방지)
        int latest = (head_ - 1 + BUFFER_SIZE) % BUFFER_SIZE;
        return buffer_[latest].state;
    }

    return buffer_[best_idx].state;
}

// InterpolateState - 두 상태 사이 보간 (더 정밀한 시점 복원용)
WorldState WorldStateBuffer::InterpolateState(const WorldState& a, const WorldState& b, float t) {
    WorldState result;
    result.tick = a.tick;
    result.timestamp = a.timestamp + static_cast<int64_t>((b.timestamp - a.timestamp) * t);

    // 플레이어 위치 선형 보간 (Lerp)
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

// [Order 8] HitValidator - 히트 검증 메인 클래스
// [LEARN] 클라이언트 히트 요청 → 과거 월드 복원 → 레이캐스트 → 결과 반환
HitValidator::HitValidator() = default;

// 매 틱마다 월드 상태 저장 (지연 보상 데이터)
void HitValidator::RecordWorldState(int64_t tick, const WorldState& state) {
    state_buffer_.SaveState(tick, state);
}

// ValidateHit - 핵심: 클라이언트 히트 주장을 서버에서 검증
HitResult HitValidator::ValidateHit(const HitRequest& request) {
    HitResult result;

    // [지연 보상] 클라이언트 타임스탬프 기준 월드 상태 복원
    // - 클라이언트 RTT를 고려해 과거로 되감기
    WorldState world = state_buffer_.GetStateAt(request.client_timestamp);

    // 슈터 상태 확인
    const auto* shooter = world.GetPlayer(request.shooter_id);
    if (!shooter || !shooter->is_alive) {
        result.reject_reason = "shooter_not_alive";
        return result;
    }

    // [서버 측 레이캐스트] - 클라이언트 주장과 무관하게 직접 계산
    auto hit = raycast_system_.Cast(world, request.origin, request.direction, request.max_distance,
                                    {request.shooter_id});  // 자기 자신은 제외

    if (!hit) {
        result.reject_reason = "no_hit";  // 서버에서 히트 없음 = 치트 가능성
        return result;
    }

    // 타겟 확인 (클라이언트가 주장한 타겟과 다를 수 있음)
    if (!request.target_id.empty() && hit->entity_id != request.target_id) {
        // 히트는 있지만 다른 플레이어 - 서버 판정 우선
    }

    result.valid = true;
    result.target_id = hit->entity_id;
    result.hit_point = hit->hit_point;
    result.hitbox = hit->hitbox;
    result.damage = CalculateDamage(hit->hitbox);

    return result;
}

// CalculateDamage - 히트박스 타입별 데미지 계산
// [LEARN] 헤드샷 보너스, 팔다리 감소 등 히트박스 기반 데미지 시스템
float HitValidator::CalculateDamage(HitboxType hitbox, float base_damage) {
    switch (hitbox) {
        case HitboxType::HEAD:
            return base_damage * HEADSHOT_MULTIPLIER;  // 머리 = 2배 데미지
        case HitboxType::BODY:
            return base_damage;                         // 몸통 = 기본 데미지
        case HitboxType::LIMB:
            return base_damage * LIMB_MULTIPLIER;      // 팔다리 = 50% 데미지
        default:
            return 0.0f;
    }
}

// ========================================================================
// [Reader Notes] 서버 측 히트 검증 (안티치트)
// ========================================================================
// 1. 왜 서버에서 검증하나?
//    - 클라이언트는 조작 가능 (에임봇, 월핵 등)
//    - "클라이언트가 맞았다고 해도 서버가 확인해야 함"
//    - 서버 권위(Server Authority) 원칙의 핵심
//
// 2. 지연 보상 (Lag Compensation)
//    - 클라이언트 쏜 시점 ≠ 서버 받은 시점 (RTT 때문)
//    - 서버가 "그 시점"으로 되감아서 판정 → 공정성
//    - WorldStateBuffer: 과거 N틱 상태 저장
//
// 3. 레이캐스팅 (Raycasting)
//    - 발사 원점 + 방향으로 "광선" 발사
//    - 모든 플레이어 히트박스와 교차 검사
//    - 가장 가까운 교차점 = 맞은 대상
//
// 4. Broad Phase vs Narrow Phase
//    - Broad Phase: AABB로 빠른 거부 (대부분 걸러짐)
//    - Narrow Phase: 캡슐로 정밀 검사 (실제 히트 확인)
//    - 성능 최적화의 핵심 패턴
//
// 5. C vs C++ 비교
//    - C: 별도 함수 vec3_length(), vec3_dot() 등
//    - C++: 멤버 함수 Vec3::Length(), 연산자 오버로딩
//    - C++: std::optional로 "히트 없음"을 타입으로 표현
//
// [CG-02.10] 안티치트 - 서버 권위로 모든 클라이언트 행동 검증
// ========================================================================

}  // namespace pvpserver::anticheat
