#include <gtest/gtest.h>

#include "pvpserver/anticheat/hit_validator.h"

using namespace pvpserver::anticheat;

class HitValidatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 테스트용 월드 상태 구성
        world_.tick = 100;
        world_.timestamp = 1000;

        PlayerState player1;
        player1.player_id = "player1";
        player1.position = {0.0f, 0.0f, 0.0f};
        player1.is_alive = true;
        player1.health = 100.0f;

        PlayerState player2;
        player2.player_id = "player2";
        player2.position = {10.0f, 0.0f, 0.0f};
        player2.is_alive = true;
        player2.health = 100.0f;

        world_.players = {player1, player2};
    }

    WorldState world_;
    HitValidator validator_;
};

TEST_F(HitValidatorTest, Vec3Operations) {
    Vec3 a{1.0f, 2.0f, 3.0f};
    Vec3 b{4.0f, 5.0f, 6.0f};

    Vec3 sum = a + b;
    EXPECT_FLOAT_EQ(sum.x, 5.0f);
    EXPECT_FLOAT_EQ(sum.y, 7.0f);
    EXPECT_FLOAT_EQ(sum.z, 9.0f);

    Vec3 diff = b - a;
    EXPECT_FLOAT_EQ(diff.x, 3.0f);
    EXPECT_FLOAT_EQ(diff.y, 3.0f);
    EXPECT_FLOAT_EQ(diff.z, 3.0f);

    Vec3 scaled = a * 2.0f;
    EXPECT_FLOAT_EQ(scaled.x, 2.0f);
    EXPECT_FLOAT_EQ(scaled.y, 4.0f);
    EXPECT_FLOAT_EQ(scaled.z, 6.0f);
}

TEST_F(HitValidatorTest, Vec3Distance) {
    Vec3 a{0.0f, 0.0f, 0.0f};
    Vec3 b{3.0f, 4.0f, 0.0f};

    float dist = Vec3::Distance(a, b);
    EXPECT_FLOAT_EQ(dist, 5.0f);
}

TEST_F(HitValidatorTest, Vec3Normalized) {
    Vec3 v{3.0f, 4.0f, 0.0f};
    Vec3 n = v.Normalized();

    EXPECT_FLOAT_EQ(n.x, 0.6f);
    EXPECT_FLOAT_EQ(n.y, 0.8f);
    EXPECT_FLOAT_EQ(n.z, 0.0f);
}

TEST_F(HitValidatorTest, AABBContains) {
    AABB box{{0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f}};

    EXPECT_TRUE(box.Contains({5.0f, 5.0f, 5.0f}));
    EXPECT_TRUE(box.Contains({0.0f, 0.0f, 0.0f}));
    EXPECT_TRUE(box.Contains({10.0f, 10.0f, 10.0f}));
    EXPECT_FALSE(box.Contains({-1.0f, 5.0f, 5.0f}));
    EXPECT_FALSE(box.Contains({11.0f, 5.0f, 5.0f}));
}

TEST_F(HitValidatorTest, PlayerHitboxUpdate) {
    PlayerHitbox hitbox;
    hitbox.UpdateFromPosition({0.0f, 0.0f, 0.0f});

    // AABB 확인
    EXPECT_FLOAT_EQ(hitbox.bounds.min.x, -0.5f);
    EXPECT_FLOAT_EQ(hitbox.bounds.max.x, 0.5f);
    EXPECT_FLOAT_EQ(hitbox.bounds.min.y, 0.0f);
    EXPECT_FLOAT_EQ(hitbox.bounds.max.y, 2.0f);
}

TEST_F(HitValidatorTest, WorldStateGetPlayer) {
    const PlayerState* p1 = world_.GetPlayer("player1");
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(p1->player_id, "player1");

    const PlayerState* unknown = world_.GetPlayer("unknown");
    EXPECT_EQ(unknown, nullptr);
}

TEST_F(HitValidatorTest, RaycastSystemHitPlayer) {
    RaycastSystem raycast;

    // player1(0,0,0)에서 player2(10,0,0) 방향으로 레이캐스트
    auto hit = raycast.Cast(world_, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 100.0f, {"player1"});

    ASSERT_TRUE(hit.has_value());
    EXPECT_EQ(hit->entity_id, "player2");
}

TEST_F(HitValidatorTest, RaycastSystemMiss) {
    RaycastSystem raycast;

    // 빗나가는 방향
    auto hit = raycast.Cast(world_, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 100.0f, {"player1"});

    EXPECT_FALSE(hit.has_value());
}

TEST_F(HitValidatorTest, RaycastIgnoresList) {
    RaycastSystem raycast;

    // player2도 무시
    auto hit =
        raycast.Cast(world_, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 100.0f, {"player1", "player2"});

    EXPECT_FALSE(hit.has_value());
}

TEST_F(HitValidatorTest, WorldStateBufferSaveAndGet) {
    WorldStateBuffer buffer;

    for (int i = 0; i < 10; ++i) {
        WorldState state;
        state.tick = i;
        state.timestamp = i * 16;  // 16ms per tick
        buffer.SaveState(i, state);
    }

    WorldState retrieved = buffer.GetStateAt(80);
    EXPECT_EQ(retrieved.tick, 5);
}

TEST_F(HitValidatorTest, ValidateHitValid) {
    validator_.RecordWorldState(100, world_);

    HitRequest request;
    request.shooter_id = "player1";
    request.target_id = "player2";
    request.origin = {0.0f, 1.0f, 0.0f};
    request.direction = {1.0f, 0.0f, 0.0f};
    request.client_timestamp = 1000;
    request.max_distance = 100.0f;

    HitResult result = validator_.ValidateHit(request);

    EXPECT_TRUE(result.valid);
    EXPECT_EQ(result.target_id, "player2");
    EXPECT_GT(result.damage, 0.0f);
}

TEST_F(HitValidatorTest, ValidateHitNoTarget) {
    validator_.RecordWorldState(100, world_);

    HitRequest request;
    request.shooter_id = "player1";
    request.origin = {0.0f, 1.0f, 0.0f};
    request.direction = {0.0f, 1.0f, 0.0f};  // 위쪽으로
    request.client_timestamp = 1000;

    HitResult result = validator_.ValidateHit(request);

    EXPECT_FALSE(result.valid);
    EXPECT_EQ(result.reject_reason, "no_hit");
}

TEST_F(HitValidatorTest, CalculateDamage) {
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::HEAD), 50.0f);
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::BODY), 20.0f);
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::LIMB), 15.0f);
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::NONE), 0.0f);
}

TEST_F(HitValidatorTest, CalculateDamageCustomBase) {
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::HEAD, 10.0f), 25.0f);
    EXPECT_FLOAT_EQ(validator_.CalculateDamage(HitboxType::BODY, 10.0f), 10.0f);
}
