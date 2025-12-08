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

// 레이-원 교차 검사
bool RayCircleIntersect(
    float ray_ox, float ray_oy,
    float ray_dx, float ray_dy,
    float circle_x, float circle_y,
    float radius,
    float& t
) {
    float fx = ray_ox - circle_x;
    float fy = ray_oy - circle_y;
    
    float a = ray_dx * ray_dx + ray_dy * ray_dy;
    float b = 2.0f * (fx * ray_dx + fy * ray_dy);
    float c = fx * fx + fy * fy - radius * radius;
    
    float discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0) {
        return false;
    }
    
    discriminant = std::sqrt(discriminant);
    float t1 = (-b - discriminant) / (2.0f * a);
    float t2 = (-b + discriminant) / (2.0f * a);
    
    if (t1 >= 0) {
        t = t1;
        return true;
    }
    if (t2 >= 0) {
        t = t2;
        return true;
    }
    
    return false;
}

}  // namespace

void LagCompensation::SaveWorldState(const WorldState& state) {
    history_.push_back(state);
    while (history_.size() > HISTORY_SIZE) {
        history_.pop_front();
    }
}

std::optional<WorldState> LagCompensation::GetWorldStateAt(
    std::uint64_t timestamp
) const {
    if (history_.empty()) {
        return std::nullopt;
    }
    
    // 범위 밖 체크
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
            
            float t = static_cast<float>(timestamp - before.timestamp) /
                      static_cast<float>(after.timestamp - before.timestamp);
            
            return Interpolate(before, after, t);
        }
    }
    
    return history_.back();
}

HitResult LagCompensation::ValidateHitWithCompensation(
    const HitRequest& request,
    std::uint64_t server_time
) {
    HitResult result;
    stats_.hits_validated++;
    
    // 되돌리기 시간 검증
    if (server_time < request.client_timestamp) {
        result.reject_reason = "Client timestamp in future";
        stats_.hits_rejected++;
        return result;
    }
    
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
    
    // 레이캐스트
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
    
    // 발사체 보간
    for (const auto& proj_b : b.projectiles) {
        Projectile interp = proj_b;
        
        for (const auto& proj_a : a.projectiles) {
            if (proj_a.id == proj_b.id) {
                interp.x = proj_a.x + t * (proj_b.x - proj_a.x);
                interp.y = proj_a.y + t * (proj_b.y - proj_a.y);
                break;
            }
        }
        
        result.projectiles.push_back(interp);
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
