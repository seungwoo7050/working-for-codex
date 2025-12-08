#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "pvpserver/core/game_loop.h"

namespace pvpserver {
namespace {

TEST(GameLoopTest, TickRateAccuracy) {
    GameLoop loop(60.0);
    std::atomic<int> ticks{0};

    loop.SetUpdateCallback([&](const TickInfo&) {
        ticks++;
    });

    loop.Start();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    loop.Stop();
    loop.Join();

    // 60 TPS × 1초 = 60 ±5 틱
    EXPECT_NEAR(ticks.load(), 60, 5);
}

TEST(GameLoopTest, TargetDelta) {
    GameLoop loop(60.0);
    EXPECT_NEAR(loop.TargetDelta(), 1.0 / 60.0, 0.0001);
}

}  // namespace
}  // namespace pvpserver
