#pragma once

#include <cstdint>

namespace pvpserver {

struct MovementInput {
    std::uint64_t sequence{0};
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    double mouse_x{0.0};
    double mouse_y{0.0};
};

}  // namespace pvpserver
