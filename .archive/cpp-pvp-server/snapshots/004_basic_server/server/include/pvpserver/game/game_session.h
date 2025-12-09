#pragma once

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "pvpserver/game/movement.h"
#include "pvpserver/game/player_state.h"

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

    std::string MetricsSnapshot() const;

   private:
    struct PlayerRuntimeState {
        PlayerState state;
    };

    double speed_per_second_;
    double elapsed_time_{0.0};

    mutable std::mutex mutex_;
    std::unordered_map<std::string, PlayerRuntimeState> players_;
};

}  // namespace pvpserver
