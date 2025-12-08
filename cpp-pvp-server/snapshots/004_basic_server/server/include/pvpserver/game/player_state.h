#pragma once

#include <cstdint>
#include <string>

namespace pvpserver {

struct PlayerState {
    std::string player_id;
    double x{0.0};
    double y{0.0};
    double facing_radians{0.0};
    std::uint64_t last_sequence{0};
    int health{100};
    bool is_alive{true};
};

}  // namespace pvpserver
