#include "pvpserver/game/game_session.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace pvpserver {

namespace {
constexpr double kPlayerSpeed = 5.0;  // meters per second
}  // namespace

GameSession::GameSession(double /*tick_rate*/) : speed_per_second_(kPlayerSpeed) {}

void GameSession::UpsertPlayer(const std::string& player_id) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto& runtime = players_[player_id];
    if (runtime.state.player_id.empty()) {
        runtime.state.player_id = player_id;
        runtime.state.x = 0.0;
        runtime.state.y = 0.0;
        runtime.state.facing_radians = 0.0;
        runtime.state.last_sequence = 0;
    }
}

void GameSession::RemovePlayer(const std::string& player_id) {
    std::lock_guard<std::mutex> lk(mutex_);
    players_.erase(player_id);
}

void GameSession::ApplyInput(const std::string& player_id, const MovementInput& input,
                             double delta_seconds) {
    std::lock_guard<std::mutex> lk(mutex_);
    auto it = players_.find(player_id);
    if (it == players_.end()) {
        return;
    }

    PlayerRuntimeState& runtime = it->second;
    PlayerState& state = runtime.state;
    if (input.sequence < state.last_sequence) {
        return;
    }
    state.last_sequence = input.sequence;

    state.facing_radians = std::atan2(input.mouse_y, input.mouse_x);

    double dx = 0.0;
    double dy = 0.0;
    if (input.up) {
        dy -= 1.0;
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

    double magnitude = std::sqrt(dx * dx + dy * dy);
    if (magnitude > 0.0) {
        dx /= magnitude;
        dy /= magnitude;
    }

    const double distance = speed_per_second_ * delta_seconds;
    state.x += dx * distance;
    state.y += dy * distance;
}

void GameSession::Tick(std::uint64_t /*tick*/, double delta_seconds) {
    std::lock_guard<std::mutex> lk(mutex_);
    elapsed_time_ += delta_seconds;
    // v1.0.0: No projectiles or combat, just movement
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

std::string GameSession::MetricsSnapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::ostringstream oss;
    oss << "# TYPE players_count gauge\n";
    oss << "players_count " << players_.size() << "\n";
    return oss.str();
}

}  // namespace pvpserver
