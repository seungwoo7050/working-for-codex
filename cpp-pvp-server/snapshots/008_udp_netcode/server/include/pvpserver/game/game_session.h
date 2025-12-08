#pragma once

#include <chrono>
#include <limits>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "pvpserver/game/combat.h"
#include "pvpserver/game/movement.h"
#include "pvpserver/game/player_state.h"
#include "pvpserver/game/projectile.h"

namespace pvpserver {

class GameSession {
   public:
    explicit GameSession(double tick_rate);

    void UpsertPlayer(const std::string& player_id);
    void RemovePlayer(const std::string& player_id);

    void ApplyInput(const std::string& player_id, const MovementInput& input, double delta_seconds);

    void Tick(std::uint64_t tick, double delta_seconds);

    PlayerState GetPlayer(const std::string& player_id) const;
    std::vector<PlayerState> Snapshot() const;

    std::vector<CombatEvent> ConsumeDeathEvents();
    std::vector<CombatEvent> CombatLogSnapshot() const;
    std::string MetricsSnapshot() const;
    std::size_t ActiveProjectileCount() const;

   private:
    struct PlayerRuntimeState {
        PlayerState state;
        HealthComponent health;
        double last_fire_time{std::numeric_limits<double>::lowest()};
        bool death_announced{false};
        int shots_fired{0};
        int hits_landed{0};
        int deaths{0};
    };

    void AppendCombatEvent(const CombatEvent& event);
    bool TrySpawnProjectile(PlayerRuntimeState& runtime, const MovementInput& input);
    void UpdateProjectilesLocked(std::uint64_t tick, double delta_seconds);

    double speed_per_second_;
    double elapsed_time_{0.0};
    std::uint64_t projectile_counter_{0};
    CombatLog combat_log_;

    std::vector<Projectile> projectiles_;
    std::vector<CombatEvent> pending_deaths_;
    std::uint64_t projectiles_spawned_total_{0};
    std::uint64_t projectiles_hits_total_{0};
    std::uint64_t players_dead_total_{0};
    std::uint64_t collisions_checked_total_{0};

    mutable std::mutex mutex_;
    std::unordered_map<std::string, PlayerRuntimeState> players_;
};

}  // namespace pvpserver
