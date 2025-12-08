// [FILE]
// - 목적: 발사체(Projectile) 물리 시뮬레이션
// - 주요 역할: 발사체 이동, 수명 관리, 충돌 반경 제공
// - 관련 클론 가이드 단계: [v1.1.0] 전투 시스템
// - 권장 읽는 순서: 생성자 → Advance() → IsExpired()
//
// [LEARN] 발사체는 매 틱마다 Advance()로 이동.
//         충돌 검사는 GameSession에서 수행, 여기는 물리만 담당.

#include "pvpserver/game/projectile.h"

#include <cmath>
#include <stdexcept>

namespace pvpserver {

namespace {
constexpr double kEpsilon = 1e-9;  // 부동소수점 비교용 작은 값
}

// [Order 1] 생성자 - 발사체 초기화
// - id: 고유 식별자 (예: "projectile-42")
// - owner_id: 발사한 플레이어 ID (자기 발사체에 안 맞기 위함)
// - x, y: 생성 위치
// - dir_x, dir_y: 이동 방향 (정규화됨)
// - spawn_time_seconds: 생성 시점 (수명 계산용)
Projectile::Projectile(std::string id, std::string owner_id, double x, double y, double dir_x,
                       double dir_y, double spawn_time_seconds)
    : id_(std::move(id)),
      owner_id_(std::move(owner_id)),
      x_(x),
      y_(y),
      dir_x_(dir_x),
      dir_y_(dir_y),
      spawn_time_(spawn_time_seconds) {
    // 방향 벡터 정규화 (단위 벡터로 만들기)
    const double magnitude = std::sqrt(dir_x_ * dir_x_ + dir_y_ * dir_y_);
    if (magnitude < kEpsilon) {
        throw std::invalid_argument("Projectile direction must be non-zero");
    }
    dir_x_ /= magnitude;
    dir_y_ /= magnitude;
}

// [Order 2] Advance - 발사체 이동
// - 거리 = 속도 × 시간 (등속 직선 운동)
// - kSpeed_ = 30.0 m/s (헤더에서 정의)
void Projectile::Advance(double delta_seconds) {
    if (!active_) {
        return;  // 비활성화된 발사체는 이동 안 함
    }
    x_ += dir_x_ * kSpeed_ * delta_seconds;
    y_ += dir_y_ * kSpeed_ * delta_seconds;
}

// [Order 3] IsExpired - 수명 만료 체크
// - kLifetime_ = 1.5초 (헤더에서 정의)
// - 발사 후 1.5초 지나면 만료
bool Projectile::IsExpired(double now_seconds) const {
    if (!active_) {
        return true;
    }
    return (now_seconds - spawn_time_) >= kLifetime_;
}

// 발사체 비활성화 (충돌 또는 만료 시 호출)
void Projectile::Deactivate() { active_ = false; }

const std::string& Projectile::id() const noexcept { return id_; }

const std::string& Projectile::owner_id() const noexcept { return owner_id_; }

double Projectile::x() const noexcept { return x_; }

double Projectile::y() const noexcept { return y_; }

double Projectile::direction_x() const noexcept { return dir_x_; }

double Projectile::direction_y() const noexcept { return dir_y_; }

double Projectile::spawn_time() const noexcept { return spawn_time_; }

bool Projectile::active() const noexcept { return active_; }

double Projectile::radius() const noexcept { return kRadius_; }

double Projectile::Speed() noexcept { return kSpeed_; }

double Projectile::Lifetime() noexcept { return kLifetime_; }

}  // namespace pvpserver
