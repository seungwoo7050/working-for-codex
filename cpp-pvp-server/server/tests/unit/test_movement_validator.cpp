#include <gtest/gtest.h>

#include "pvpserver/anticheat/movement_validator.h"

using namespace pvpserver::anticheat;

class MovementValidatorTest : public ::testing::Test {
protected:
    MovementValidator validator_;
};

TEST_F(MovementValidatorTest, ValidMovement) {
    // 정상 속도 이동
    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{0.5f, 0.0f, 0.0f};  // 5m/s * 0.1s = 0.5m
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.violation, ViolationType::NONE);
}

TEST_F(MovementValidatorTest, SpeedHackDetection) {
    // 스피드핵은 텔레포트 임계값 미만이지만 최대 속도를 초과하는 경우
    // 최대 속도: 5 * 1.1 = 5.5m/s, 텔레포트 임계: 5 * 1.5 * 3 = 22.5m/s
    // 테스트: 15m/s (스피드핵 범위)
    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{1.5f, 0.0f, 0.0f};  // 1.5m / 0.1s = 15m/s
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.violation, ViolationType::SPEEDHACK);
}

TEST_F(MovementValidatorTest, TeleportDetection) {
    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{100.0f, 0.0f, 0.0f};  // 1000m/s - 명확한 텔레포트
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.violation, ViolationType::TELEPORT);
}

TEST_F(MovementValidatorTest, SprintingAllowsHigherSpeed) {
    PlayerMovementState state;
    state.is_sprinting = true;
    validator_.SetPlayerState("player1", state);

    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{0.75f, 0.0f, 0.0f};  // 7.5m/s * 0.1s (스프린트: 7.5m/s 허용)
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_TRUE(result.valid);
}

TEST_F(MovementValidatorTest, SlowedPlayerHasLowerSpeed) {
    PlayerMovementState state;
    state.is_slowed = true;
    validator_.SetPlayerState("player1", state);

    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{0.5f, 0.0f, 0.0f};  // 5m/s (슬로우 상태에서는 2.75m/s까지만)
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.violation, ViolationType::SPEEDHACK);
}

TEST_F(MovementValidatorTest, WallClipDetection) {
    // 장애물 설정
    Obstacle wall;
    wall.bounds = {{5.0f, 0.0f, -1.0f}, {6.0f, 3.0f, 1.0f}};
    wall.is_solid = true;
    validator_.SetObstacles({wall});

    Vec3 old_pos{0.0f, 1.0f, 0.0f};
    Vec3 new_pos{10.0f, 1.0f, 0.0f};  // 벽 통과
    float delta_time = 2.0f;           // 속도는 정상

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.violation, ViolationType::WALLCLIP);
}

TEST_F(MovementValidatorTest, NoWallClipWithoutObstacles) {
    validator_.ClearObstacles();

    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{0.4f, 0.0f, 0.0f};
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_TRUE(result.valid);
}

TEST_F(MovementValidatorTest, PlayerStateManagement) {
    PlayerMovementState state;
    state.is_sprinting = true;
    state.speed_modifier = 1.2f;

    validator_.SetPlayerState("player1", state);

    auto* retrieved = validator_.GetPlayerState("player1");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_TRUE(retrieved->is_sprinting);
    EXPECT_FLOAT_EQ(retrieved->speed_modifier, 1.2f);

    validator_.RemovePlayer("player1");
    EXPECT_EQ(validator_.GetPlayerState("player1"), nullptr);
}

TEST_F(MovementValidatorTest, ViolationCounting) {
    // 여러 번 위반
    for (int i = 0; i < 3; ++i) {
        Vec3 old_pos{0.0f, 0.0f, 0.0f};
        Vec3 new_pos{10.0f, 0.0f, 0.0f};
        validator_.ValidateMovement("cheater", old_pos, new_pos, 0.1f);
    }

    EXPECT_EQ(validator_.GetViolationCount("cheater"), 3);
}

TEST_F(MovementValidatorTest, ZeroDeltaTime) {
    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{10.0f, 0.0f, 0.0f};

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, 0.0f);

    // delta_time = 0이면 검증 스킵
    EXPECT_TRUE(result.valid);
}

TEST_F(MovementValidatorTest, SpeedModifier) {
    PlayerMovementState state;
    state.speed_modifier = 2.0f;  // 부스트 아이템 등
    validator_.SetPlayerState("player1", state);

    Vec3 old_pos{0.0f, 0.0f, 0.0f};
    Vec3 new_pos{1.0f, 0.0f, 0.0f};  // 10m/s (modifier 2.0 적용 시 허용)
    float delta_time = 0.1f;

    MovementCheck result = validator_.ValidateMovement("player1", old_pos, new_pos, delta_time);

    EXPECT_TRUE(result.valid);
}
