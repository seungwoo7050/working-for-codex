// [FILE]
// - 목적: 게임 세션 상태 관리 (플레이어, 발사체, 충돌, 전투)
// - 주요 역할: 플레이어 입력 처리, 발사체 이동, 충돌 검출, 데미지 적용
// - 관련 클론 가이드 단계: [CG-v1.0.0] 기본 서버, [CG-v1.1.0] 전투 시스템
// - 권장 읽는 순서: 상수 정의 → UpsertPlayer → ApplyInput → Tick → 충돌 처리
//
// [LEARN] 이 파일은 "서버 권위(Server Authority)" 모델의 핵심.
//         모든 게임 상태는 서버에서 계산하고, 클라이언트는 결과만 받음.
//         C에서 전역 상태 + 함수들로 구현하던 것을 클래스로 측슐화.

#include "pvpserver/game/game_session.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pvpserver {

// [Order 1] 게임 상수 정의
// - 게임 밸런스에 영향을 주는 핵심 값들
// - 클론 가이드 단계: [v1.1.0]
// [LEARN] namespace {} (익명 네임스페이스)는 C의 static과 동일.
//         이 파일 내에서만 접근 가능한 상수/함수를 정의.
namespace {
constexpr double kPlayerSpeed = 5.0;   // 플레이어 이동 속도 (m/s)
constexpr double kPlayerRadius = 0.5;  // 플레이어 충돌 반경 (m)
constexpr double kFireCooldown = 0.1;  // 발사 쿨다운 (초, 초당 10발)
constexpr double kSpawnOffset = 0.3;   // 발사체 생성 오프셋 (플레이어 앞)
constexpr int kDamagePerHit = 20;      // 발사체 적중 데미지 (HP)
}  // namespace

// [Order 2] 생성자
// - combat_log_(32): 최근 32개의 전투 이벤트를 링 버퍼로 저장
GameSession::GameSession(double /*tick_rate*/) : speed_per_second_(kPlayerSpeed), combat_log_(32) {}

// [Order 3] UpsertPlayer - 플레이어 생성 또는 갱신
// - 새 플레이어면 초기 상태로 생성, 기존 플레이어면 체력 리셋
// - 클론 가이드 단계: [v1.0.0]
// [LEARN] std::lock_guard는 함수 종료 시 자동으로 unlock됨 (RAII).
//         C에서 함수 끝마다 pthread_mutex_unlock 호출하던 것을 자동화.
void GameSession::UpsertPlayer(const std::string& player_id) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto& runtime = players_[player_id];  // 없으면 자동 생성됨
    if (runtime.state.player_id.empty()) {
        runtime.state.player_id = player_id;
        runtime.state.x = 0.0;
        runtime.state.y = 0.0;
        runtime.state.facing_radians = 0.0;
        runtime.state.last_sequence = 0;
        runtime.shots_fired = 0;
        runtime.hits_landed = 0;
        runtime.deaths = 0;
        runtime.state.shots_fired = 0;
        runtime.state.hits_landed = 0;
        runtime.state.deaths = 0;
    }
    runtime.health.Reset();
    runtime.state.health = runtime.health.current();
    runtime.state.is_alive = runtime.health.is_alive();
    runtime.death_announced = false;
    runtime.last_fire_time = std::numeric_limits<double>::lowest();
}

// [Order 4] RemovePlayer - 플레이어 제거
// - 플레이어 상태 삭제 + 해당 플레이어의 발사체도 제거
// [LEARN] std::remove_if + erase는 "erase-remove idiom"이라는 C++ 관용구.
//         조건에 맞는 요소를 벡터 끝으로 이동 후 삭제.
void GameSession::RemovePlayer(const std::string& player_id) {
    std::lock_guard<std::mutex> lk(mutex_);
    players_.erase(player_id);
    // 해당 플레이어가 발사한 발사체도 함께 제거
    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(),
                                      [&](const Projectile& projectile) {
                                          return projectile.owner_id() == player_id;
                                      }),
                       projectiles_.end());
}

// [Order 5] ApplyInput - 클라이언트 입력 처리
// - 서버 권위 모델: 클라이언트가 보낸 입력을 서버에서 검증 후 적용
// - sequence 번호로 중복/순서 역전 방지
// - 클론 가이드 단계: [v1.0.0]
void GameSession::ApplyInput(const std::string& player_id, const MovementInput& input,
                             double delta_seconds) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = players_.find(player_id);
    if (it == players_.end()) {
        return;  // 존재하지 않는 플레이어 무시
    }

    PlayerRuntimeState& runtime = it->second;
    PlayerState& state = runtime.state;

    // 시퀀스 검사: 이전 입력보다 오래된 입력은 무시 (네트워크 지연 대응)
    if (input.sequence < state.last_sequence) {
        return;
    }
    state.last_sequence = input.sequence;

    // 마우스 위치로 플레이어 바라보는 방향 계산
    // [LEARN] atan2(y, x)는 C 표준 라이브러리 함수. 라디안 각도 반환.
    state.facing_radians = std::atan2(input.mouse_y, input.mouse_x);

    if (state.is_alive) {
        // WASD 입력을 방향 벡터로 변환
        double dx = 0.0;
        double dy = 0.0;
        if (input.up) {
            dy -= 1.0;  // 화면 좌표계: 위쪽이 -y
        }
        if (input.down) {
            dy += 1.0;
        }
        if (input.left) {
            dx -= 1.0;
        }
        if (input.right) {
            dx += 1.0;
        }

        // 대각선 이동 시 정규화 (속도 일정하게)
        double magnitude = std::sqrt(dx * dx + dy * dy);
        if (magnitude > 0.0) {
            dx /= magnitude;
            dy /= magnitude;
        }

        // 위치 업데이트: 거리 = 속도 × 시간
        const double distance = speed_per_second_ * delta_seconds;
        state.x += dx * distance;
        state.y += dy * distance;
    }

    // 발사 입력 처리
    TrySpawnProjectile(runtime, input);
}

// [Order 6] Tick - 매 틱마다 호출되는 게임 상태 업데이트
// - GameLoop에서 60 TPS로 호출
// - 발사체 이동 + 충돌 처리
// - 클론 가이드 단계: [v1.0.0], [v1.1.0]
void GameSession::Tick(std::uint64_t tick, double delta_seconds) {
    std::lock_guard<std::mutex> lk(mutex_);
    UpdateProjectilesLocked(tick, delta_seconds);
}

PlayerState GameSession::GetPlayer(const std::string& player_id) const {
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = players_.find(player_id);
    if (it == players_.end()) {
        throw std::runtime_error("player not found");
    }
    return it->second.state;
}

std::vector<PlayerState> GameSession::Snapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::vector<PlayerState> states;
    states.reserve(players_.size());
    for (const auto& kv : players_) {
        states.push_back(kv.second.state);
    }
    return states;
}

std::vector<CombatEvent> GameSession::ConsumeDeathEvents() {
    std::lock_guard<std::mutex> lk(mutex_);
    std::vector<CombatEvent> events = std::move(pending_deaths_);
    pending_deaths_.clear();
    return events;
}

std::vector<CombatEvent> GameSession::CombatLogSnapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    return combat_log_.Snapshot();
}

std::string GameSession::MetricsSnapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::ostringstream oss;
    const auto active_projectiles =
        std::count_if(projectiles_.begin(), projectiles_.end(),
                      [](const Projectile& projectile) { return projectile.active(); });
    oss << "# TYPE projectiles_active gauge\n";
    oss << "projectiles_active " << active_projectiles << "\n";
    oss << "# TYPE projectiles_spawned_total counter\n";
    oss << "projectiles_spawned_total " << projectiles_spawned_total_ << "\n";
    oss << "# TYPE projectiles_hits_total counter\n";
    oss << "projectiles_hits_total " << projectiles_hits_total_ << "\n";
    oss << "# TYPE players_dead_total counter\n";
    oss << "players_dead_total " << players_dead_total_ << "\n";
    oss << "# TYPE collisions_checked_total counter\n";
    oss << "collisions_checked_total " << collisions_checked_total_ << "\n";
    return oss.str();
}

std::size_t GameSession::ActiveProjectileCount() const {
    std::lock_guard<std::mutex> lk(mutex_);
    return std::count_if(projectiles_.begin(), projectiles_.end(),
                         [](const Projectile& projectile) { return projectile.active(); });
}

void GameSession::AppendCombatEvent(const CombatEvent& event) { combat_log_.Add(event); }

// [Order 7] TrySpawnProjectile - 발사체 생성 시도
// - 쿨다운 체크, 방향 계산, 발사체 객체 생성
// - 클론 가이드 단계: [v1.1.0]
bool GameSession::TrySpawnProjectile(PlayerRuntimeState& runtime, const MovementInput& input) {
    // 발사 버튼을 누르지 않았거나 죽은 상태면 무시
    if (!input.fire || !runtime.state.is_alive) {
        return false;
    }

    // 조준 방향 벡터 정규화
    const double aim_magnitude =
        std::sqrt(input.mouse_x * input.mouse_x + input.mouse_y * input.mouse_y);
    if (aim_magnitude < 1e-6) {
        return false;  // 조준 방향이 없으면 무시
    }

    // 발사 쿨다운 체크 (초당 10발 제한)
    if ((elapsed_time_ - runtime.last_fire_time) < kFireCooldown) {
        return false;
    }

    runtime.last_fire_time = elapsed_time_;

    // 발사체 생성 위치: 플레이어 위치 + 방향 × 오프셋
    const double dir_x = input.mouse_x / aim_magnitude;
    const double dir_y = input.mouse_y / aim_magnitude;
    const double spawn_x = runtime.state.x + dir_x * kSpawnOffset;
    const double spawn_y = runtime.state.y + dir_y * kSpawnOffset;

    // 고유 ID 생성 + Projectile 객체 생성
    std::ostringstream id_stream;
    id_stream << "projectile-" << ++projectile_counter_;
    Projectile projectile(id_stream.str(), runtime.state.player_id, spawn_x, spawn_y, dir_x, dir_y,
                          elapsed_time_);
    std::cout << "projectile spawn " << projectile.id() << " owner=" << runtime.state.player_id
              << std::endl;
    projectiles_.push_back(std::move(projectile));
    ++projectiles_spawned_total_;
    ++runtime.shots_fired;
    runtime.state.shots_fired = runtime.shots_fired;
    return true;
}

// [Order 8] UpdateProjectilesLocked - 발사체 이동 + 충돌 검출
// - 매 틱마다 호출되어 모든 발사체를 이동시키고 충돌 검사
// - 원-원 충돌 검출 (Circle-Circle Collision)
// - 클론 가이드 단계: [v1.1.0]
void GameSession::UpdateProjectilesLocked(std::uint64_t tick, double delta_seconds) {
    elapsed_time_ += delta_seconds;

    // 모든 발사체 이동 + 만료 체크
    for (auto& projectile : projectiles_) {
        projectile.Advance(delta_seconds);
        if (projectile.IsExpired(elapsed_time_)) {
            projectile.Deactivate();
        }
    }

    // [LEARN] O(N×M) 충돌 검사 - 발사체 N개 × 플레이어 M명
    //         대규모 게임에서는 공간 분할(Quadtree, Grid)로 최적화 필요.
    //         지금은 1v1 게임이라 브루트포스로 충분.
    std::uint64_t pairs_checked = 0;
    for (auto& projectile : projectiles_) {
        if (!projectile.active()) {
            continue;
        }
        for (auto& kv : players_) {
            PlayerRuntimeState& runtime = kv.second;
            // 자신이 발사한 발사체에는 맞지 않음, 죽은 플레이어도 스킵
            if (!runtime.state.is_alive || runtime.state.player_id == projectile.owner_id()) {
                continue;
            }
            ++pairs_checked;

            // AABB 사전 검사 (빠른 배제)
            const double dx = projectile.x() - runtime.state.x;
            const double dy = projectile.y() - runtime.state.y;
            const double radius_sum = projectile.radius() + kPlayerRadius;
            if (std::abs(dx) > radius_sum || std::abs(dy) > radius_sum) {
                continue;  // 확실히 충돌 안 함
            }

            // 원-원 충돌 검사: 거리² ≤ (r1 + r2)²
            const double distance_sq = dx * dx + dy * dy;
            if (distance_sq <= radius_sum * radius_sum) {
                // 충돌 발생!
                projectile.Deactivate();
                CombatEvent hit_event;
                hit_event.type = CombatEventType::Hit;
                hit_event.shooter_id = projectile.owner_id();
                hit_event.target_id = runtime.state.player_id;
                hit_event.projectile_id = projectile.id();
                hit_event.damage = kDamagePerHit;
                hit_event.tick = tick;
                AppendCombatEvent(hit_event);
                std::cout << "hit " << hit_event.shooter_id << "->" << hit_event.target_id
                          << " dmg=" << hit_event.damage << std::endl;
                ++projectiles_hits_total_;

                // 데미지 적용 + 사망 체크
                const bool died = runtime.health.ApplyDamage(kDamagePerHit);
                runtime.state.health = runtime.health.current();
                runtime.state.is_alive = runtime.health.is_alive();

                // 발사한 플레이어의 적중 횟수 증가
                auto shooter_it = players_.find(projectile.owner_id());
                if (shooter_it != players_.end()) {
                    ++shooter_it->second.hits_landed;
                    shooter_it->second.state.hits_landed = shooter_it->second.hits_landed;
                }

                if (died && !runtime.death_announced) {
                    // 사망 이벤트 발생 (한 번만)
                    runtime.death_announced = true;
                    CombatEvent death_event;
                    death_event.type = CombatEventType::Death;
                    death_event.shooter_id = projectile.owner_id();
                    death_event.target_id = runtime.state.player_id;
                    death_event.projectile_id = projectile.id();
                    death_event.tick = tick;
                    pending_deaths_.push_back(death_event);  // 클라이언트에 브로드캐스트용
                    AppendCombatEvent(death_event);
                    ++players_dead_total_;
                    ++runtime.deaths;
                    runtime.state.deaths = runtime.deaths;
                    std::cout << "death " << runtime.state.player_id << std::endl;
                }
                break;  // 발사체는 한 명만 맞추면 소멸
            }
        }
    }

    collisions_checked_total_ += pairs_checked;

    // 비활성화된 발사체 제거 (erase-remove idiom)
    projectiles_.erase(
        std::remove_if(projectiles_.begin(), projectiles_.end(),
                       [](const Projectile& projectile) { return !projectile.active(); }),
        projectiles_.end());
}

}  // namespace pvpserver

// [Reader Notes]
// ================================================================================
// 이 파일에서 처음 등장한 게임 서버 개념:
//
// 1. 서버 권위 모델 (Server Authority)
//    - 모든 게임 상태(위치, HP, 발사체)는 서버에서 계산
//    - 클라이언트는 입력만 보내고, 서버가 계산한 결과를 받아서 표시
//    - 치트 방지의 핵심: 클라이언트는 결과를 조작할 수 없음
//
// 2. 고정 틱레이트 시뮬레이션
//    - delta_seconds가 항상 1/60초에 가깝게 유지됨
//    - 물리 계산이 프레임률에 독립적으로 동작
//    - 위치 = 속도 × 시간 (적분)
//
// 3. 원-원 충돌 검출 (Circle-Circle Collision)
//    - 두 원의 중심 거리² ≤ (반경1 + 반경2)²이면 충돌
//    - AABB 사전 검사로 불필요한 sqrt 계산 회피
//
// 4. 입력 시퀀스 번호
//    - 네트워크 지연으로 순서가 뒤바뀐 입력 무시
//    - 클라이언트가 sequence++를 붙여서 전송
//
// 관련 설계 문서:
// - design/v1.0.0-game-server.md (서버 권위 모델)
// - design/v1.1.0-combat.md (전투 시스템)
//
// 이 파일을 이해한 다음, 이어서 보면 좋은 파일:
// - server/src/game/projectile.cpp (발사체 물리)
// - server/src/game/combat.cpp (HP/데미지 시스템)
// - server/src/network/websocket_server.cpp (상태 브로드캐스트)
// ================================================================================
