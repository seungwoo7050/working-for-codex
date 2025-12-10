#include <gtest/gtest.h>
#include "pvpserver/core/config.h"

namespace pvpserver {
namespace {

TEST(GameConfigTest, DefaultValues) {
    auto config = GameConfig::FromEnv();
    EXPECT_EQ(config.port(), 8080);
    EXPECT_EQ(config.metrics_port(), 9090);
    EXPECT_DOUBLE_EQ(config.tick_rate(), 60.0);
}

}  // namespace
}  // namespace pvpserver
