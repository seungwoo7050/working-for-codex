#include <gtest/gtest.h>
#include "pvpserver/game/game_session.h"

namespace pvpserver {
namespace {

TEST(GameSessionTest, UpsertPlayer) {
    GameSession session(60.0);
    session.UpsertPlayer("player1");

    auto state = session.GetPlayer("player1");
    EXPECT_EQ(state.player_id, "player1");
    EXPECT_DOUBLE_EQ(state.x, 0.0);
    EXPECT_DOUBLE_EQ(state.y, 0.0);
}

TEST(GameSessionTest, PlayerMovement) {
    GameSession session(60.0);
    session.UpsertPlayer("player1");

    MovementInput input;
    input.sequence = 1;
    input.right = true;
    input.mouse_x = 1.0;
    input.mouse_y = 0.0;

    session.ApplyInput("player1", input, 1.0);  // 1 second at 5 m/s

    auto state = session.GetPlayer("player1");
    EXPECT_NEAR(state.x, 5.0, 0.01);  // 5 meters moved right
    EXPECT_DOUBLE_EQ(state.y, 0.0);
}

TEST(GameSessionTest, DiagonalMovementNormalized) {
    GameSession session(60.0);
    session.UpsertPlayer("player1");

    MovementInput input;
    input.sequence = 1;
    input.right = true;
    input.down = true;
    input.mouse_x = 1.0;
    input.mouse_y = 1.0;

    session.ApplyInput("player1", input, 1.0);

    auto state = session.GetPlayer("player1");
    // 대각선 이동은 정규화됨 (5 * 0.707 ≈ 3.54)
    EXPECT_NEAR(state.x, 3.54, 0.1);
    EXPECT_NEAR(state.y, 3.54, 0.1);
}

TEST(GameSessionTest, Snapshot) {
    GameSession session(60.0);
    session.UpsertPlayer("player1");
    session.UpsertPlayer("player2");

    auto states = session.Snapshot();
    EXPECT_EQ(states.size(), 2);
}

}  // namespace
}  // namespace pvpserver
