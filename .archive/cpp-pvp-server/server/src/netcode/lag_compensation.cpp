// [FILE]
// - 목적: 지연 보상(Lag Compensation) - 공정한 히트 판정
// - 주요 역할: 과거 게임 상태 조회, 지연 보상 히트 검증
// - 관련 클론 가이드 단계: [v1.4.1] 클라이언트 예측/리컨실리에이션
// - 권장 읽는 순서: SaveWorldState → GetWorldStateAt → ValidateHitWithCompensation
//
// [LEARN] 지연 보상은 "내 화면에서 맞았으면 서버도 맞춘 걸로"를 구현.
//         서버가 과거 게임 상태를 저장해두고, 클라이언트가 발사한 시점의
//         상태로 되감아서 히트 판정. 네트워크 지연이 있어도 공정한 게임.

#include "pvpserver/netcode/lag_compensation.h"

#include <algorithm>
#include <cmath>

namespace pvpserver::netcode {

namespace {

constexpr float PLAYER_RADIUS = 0.5f;  // 플레이어 충돌 반경

float Distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
}

// [Order 1] RayCircleIntersect - 레이-원 교차 검사
// - 발사체 경로(레이)가 플레이어(원)와 교차하는지 판정
// - 2차 방정식의 판별식으로 교차 여부 계산
// [LEARN] 게임에서 히트스캔 무기(총알 즉시 적중)는 레이캐스트 사용.
//         발사체 무기(로켓 등)는 충돌 검사를 매 프레임 수행.
bool RayCircleIntersect(
    float ray_ox, float ray_oy,     // 레이 시작점
    float ray_dx, float ray_dy,     // 레이 방향
    float circle_x, float circle_y, // 원 중심
    float radius,                    // 원 반경
    float& t                         // 교차 거리 (출력)
) {
    // 레이 시작점에서 원 중심까지의 벡터
    float fx = ray_ox - circle_x;
    float fy = ray_oy - circle_y;
    
    // 2차 방정식 계수: at² + bt + c = 0
    float a = ray_dx * ray_dx + ray_dy * ray_dy;
    float b = 2.0f * (fx * ray_dx + fy * ray_dy);
    float c = fx * fx + fy * fy - radius * radius;
    
    // 판별식으로 교차 여부 확인
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
        return false;  // 교차 안 함
    }
    
    // 두 교차점 중 가까운 것 선택
    discriminant = std::sqrt(discriminant);
    float t1 = (-b - discriminant) / (2.0f * a);
    float t2 = (-b + discriminant) / (2.0f * a);
    
    if (t1 >= 0) {
        t = t1;  // 첫 번째 교차점 (더 가까움)
        return true;
    }
    if (t2 >= 0) {
        t = t2;  // 두 번째 교차점
        return true;
    }
    
    return false;  // 레이 시작점이 원 안에 있고 반대 방향
}

}  // namespace

// [Order 2] SaveWorldState - 과거 상태 저장
// - 매 틱마다 호출하여 게임 상태 스냅샷 저장
// - HISTORY_SIZE만큼만 유지 (오래된 것은 버림)
void LagCompensation::SaveWorldState(const WorldState& state) {
    history_.push_back(state);
    while (history_.size() > HISTORY_SIZE) {
        history_.pop_front();
    }
}

// [Order 3] GetWorldStateAt - 특정 시점의 상태 조회
// - 정확한 시점이 없으면 보간(interpolation)으로 추정
// [LEARN] 시간 t가 스냅샷 A(t1)와 B(t2) 사이면:
//         보간 비율 = (t - t1) / (t2 - t1)
//         위치 = A.위치 + 비율 × (B.위치 - A.위치)
std::optional<WorldState> LagCompensation::GetWorldStateAt(
    std::uint64_t timestamp
) const {
    if (history_.empty()) {
        return std::nullopt;
    }
    
    // 범위 밖 체크: 가장 오래된/최신 상태 반환
    if (timestamp <= history_.front().timestamp) {
        return history_.front();
    }
    if (timestamp >= history_.back().timestamp) {
        return history_.back();
    }
    
    // 보간할 두 상태 찾기
    for (std::size_t i = 1; i < history_.size(); ++i) {
        if (history_[i].timestamp >= timestamp) {
            const auto& before = history_[i - 1];
            const auto& after = history_[i];
            
            // 보간 비율 계산
            float t = static_cast<float>(timestamp - before.timestamp) /
                      static_cast<float>(after.timestamp - before.timestamp);
            
            return Interpolate(before, after, t);
        }
    }
    
    return history_.back();
}

// [Order 4] ValidateHitWithCompensation - 지연 보상 히트 검증
// - 클라이언트가 "맞았다"고 주장 → 서버가 과거 상태로 검증
// - 최대 되감기 시간(MAX_REWIND_MS) 제한으로 치트 방지
HitResult LagCompensation::ValidateHitWithCompensation(
    const HitRequest& request,
    std::uint64_t server_time
) {
    HitResult result;
    stats_.hits_validated++;
    
    // 미래 시간은 거부 (시간 치트 방지)
    if (server_time < request.client_timestamp) {
        result.reject_reason = "Client timestamp in future";
        stats_.hits_rejected++;
        return result;
    }
    
    // 되감기 시간 검증 (너무 오래된 요청 거부)
    std::uint64_t rewind_amount = server_time - request.client_timestamp;
    if (rewind_amount > static_cast<std::uint64_t>(MAX_REWIND_MS)) {
        result.reject_reason = "Rewind exceeds maximum";
        stats_.hits_rejected++;
        return result;
    }
    
    // 과거 상태 조회
    auto past_state = GetWorldStateAt(request.client_timestamp);
    if (!past_state) {
        result.reject_reason = "No historical state available";
        stats_.hits_rejected++;
        return result;
    }
    
    // 과거 상태에서 레이캐스트 수행
    auto hit = RaycastPlayers(
        *past_state,
        request.shooter_id,
        request.origin_x, request.origin_y,
        request.direction_x, request.direction_y
    );
    
    if (!hit) {
        result.reject_reason = "No hit detected";
        stats_.hits_rejected++;
        return result;
    }
    
    // 히트 확정
    result.valid = true;
    result.hit_player_id = hit->first;
    result.hit_x = hit->second.first;
    result.hit_y = hit->second.second;
    result.damage = 20.0f;  // 기본 데미지
    
    stats_.hits_accepted++;
    stats_.avg_rewind_ms = 
        (stats_.avg_rewind_ms * (stats_.hits_validated - 1) + rewind_amount) 
        / stats_.hits_validated;
    
    return result;
}

std::uint64_t LagCompensation::CalculateRewindTime(
    std::uint64_t client_timestamp,
    std::uint64_t server_time,
    std::uint32_t client_rtt
) const {
    // 클라이언트 타임스탬프 + RTT/2 보정
    std::uint64_t estimated_send_time = client_timestamp;
    std::uint64_t half_rtt = client_rtt / 2;
    
    if (estimated_send_time + half_rtt <= server_time) {
        return server_time - (estimated_send_time + half_rtt);
    }
    
    return 0;
}

WorldState LagCompensation::Interpolate(
    const WorldState& a,
    const WorldState& b,
    float t
) const {
    WorldState result;
    result.timestamp = a.timestamp + 
        static_cast<std::uint64_t>(t * (b.timestamp - a.timestamp));
    
    // 플레이어 보간
    for (const auto& player_b : b.players) {
        PlayerState interp = player_b;
        
        for (const auto& player_a : a.players) {
            if (player_a.player_id == player_b.player_id) {
                interp.x = player_a.x + t * (player_b.x - player_a.x);
                interp.y = player_a.y + t * (player_b.y - player_a.y);
                interp.facing_radians = player_a.facing_radians + 
                    t * (player_b.facing_radians - player_a.facing_radians);
                break;
            }
        }
        
        result.players.push_back(interp);
    }
    
    // 발사체 보간 (Projectile은 불변 클래스이므로 최신 상태 사용)
    // 발사체는 빠르게 움직이므로 보간 없이 가장 가까운 상태 사용
    for (const auto& proj_b : b.projectiles) {
        result.projectiles.push_back(proj_b);
    }
    
    return result;
}

std::optional<std::pair<std::string, std::pair<float, float>>> 
LagCompensation::RaycastPlayers(
    const WorldState& state,
    const std::string& shooter_id,
    float origin_x,
    float origin_y,
    float dir_x,
    float dir_y
) const {
    float closest_t = std::numeric_limits<float>::max();
    std::string closest_player;
    float hit_x = 0, hit_y = 0;
    
    for (const auto& player : state.players) {
        // 자기 자신 제외
        if (player.player_id == shooter_id) {
            continue;
        }
        
        // 죽은 플레이어 제외
        if (!player.is_alive) {
            continue;
        }
        
        float t;
        if (RayCircleIntersect(
            origin_x, origin_y,
            dir_x, dir_y,
            static_cast<float>(player.x), static_cast<float>(player.y),
            PLAYER_RADIUS,
            t
        )) {
            if (t < closest_t) {
                closest_t = t;
                closest_player = player.player_id;
                hit_x = origin_x + t * dir_x;
                hit_y = origin_y + t * dir_y;
            }
        }
    }
    
    if (closest_player.empty()) {
        return std::nullopt;
    }
    
    return std::make_pair(closest_player, std::make_pair(hit_x, hit_y));
}

}  // namespace pvpserver::netcode
