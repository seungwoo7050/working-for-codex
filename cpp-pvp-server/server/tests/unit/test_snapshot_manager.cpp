#include <gtest/gtest.h>

#include "pvpserver/network/snapshot_manager.h"

using namespace pvpserver;

class SnapshotManagerTest : public ::testing::Test {
   protected:
    SnapshotManager manager_;
};

TEST_F(SnapshotManagerTest, CreateAndSaveSnapshot) {
    std::vector<PlayerState> players;
    PlayerState p1;
    p1.player_id = "player1";
    p1.x = 10.0;
    p1.y = 20.0;
    p1.health = 100;
    p1.is_alive = true;
    players.push_back(p1);

    std::vector<Projectile> projectiles;

    auto snapshot = manager_.CreateSnapshot(players, projectiles);
    EXPECT_EQ(snapshot.sequence, 1);
    EXPECT_GT(snapshot.timestamp, 0ULL);
    EXPECT_EQ(snapshot.players.size(), 1);

    manager_.SaveSnapshot(snapshot);
    EXPECT_EQ(manager_.BufferedCount(), 1);
}

TEST_F(SnapshotManagerTest, GetSnapshot) {
    std::vector<PlayerState> players;
    PlayerState p1;
    p1.player_id = "player1";
    players.push_back(p1);

    auto snapshot1 = manager_.CreateSnapshot(players, {});
    manager_.SaveSnapshot(snapshot1);

    auto snapshot2 = manager_.CreateSnapshot(players, {});
    manager_.SaveSnapshot(snapshot2);

    auto retrieved1 = manager_.GetSnapshot(1);
    ASSERT_TRUE(retrieved1.has_value());
    EXPECT_EQ(retrieved1->sequence, 1);

    auto retrieved2 = manager_.GetSnapshot(2);
    ASSERT_TRUE(retrieved2.has_value());
    EXPECT_EQ(retrieved2->sequence, 2);

    auto not_found = manager_.GetSnapshot(999);
    EXPECT_FALSE(not_found.has_value());
}

TEST_F(SnapshotManagerTest, GetLatestSnapshot) {
    EXPECT_FALSE(manager_.GetLatestSnapshot().has_value());

    std::vector<PlayerState> players;
    auto snapshot1 = manager_.CreateSnapshot(players, {});
    manager_.SaveSnapshot(snapshot1);

    auto latest = manager_.GetLatestSnapshot();
    ASSERT_TRUE(latest.has_value());
    EXPECT_EQ(latest->sequence, 1);

    auto snapshot2 = manager_.CreateSnapshot(players, {});
    manager_.SaveSnapshot(snapshot2);

    latest = manager_.GetLatestSnapshot();
    ASSERT_TRUE(latest.has_value());
    EXPECT_EQ(latest->sequence, 2);
}

TEST_F(SnapshotManagerTest, BufferCircular) {
    std::vector<PlayerState> players;

    // 버퍼 크기보다 많이 저장
    for (std::size_t i = 0; i < SnapshotManager::BUFFER_SIZE + 10; ++i) {
        auto snapshot = manager_.CreateSnapshot(players, {});
        manager_.SaveSnapshot(snapshot);
    }

    EXPECT_EQ(manager_.BufferedCount(), SnapshotManager::BUFFER_SIZE);

    // 오래된 스냅샷은 찾을 수 없어야 함
    auto old = manager_.GetSnapshot(1);
    EXPECT_FALSE(old.has_value());

    // 최신 스냅샷은 찾을 수 있어야 함
    auto latest = manager_.GetLatestSnapshot();
    ASSERT_TRUE(latest.has_value());
    EXPECT_EQ(latest->sequence, SnapshotManager::BUFFER_SIZE + 10);
}

TEST_F(SnapshotManagerTest, SnapshotSerializeDeserialize) {
    Snapshot snapshot;
    snapshot.sequence = 42;
    snapshot.timestamp = 1234567890123ULL;

    PlayerState p1;
    p1.player_id = "player1";
    p1.x = 100.5;
    p1.y = 200.25;
    p1.facing_radians = 1.57;
    p1.health = 75;
    p1.is_alive = true;
    p1.last_sequence = 10;
    snapshot.players.push_back(p1);

    ProjectileSnapshot proj;
    proj.id = 1;
    proj.owner_id = "player1";
    proj.x = 50.0f;
    proj.y = 60.0f;
    proj.velocity_x = 10.0f;
    proj.velocity_y = 5.0f;
    snapshot.projectiles.push_back(proj);

    auto serialized = snapshot.Serialize();
    auto deserialized = Snapshot::Deserialize(serialized);

    EXPECT_EQ(deserialized.sequence, snapshot.sequence);
    EXPECT_EQ(deserialized.timestamp, snapshot.timestamp);
    ASSERT_EQ(deserialized.players.size(), 1);
    EXPECT_EQ(deserialized.players[0].player_id, "player1");
    EXPECT_DOUBLE_EQ(deserialized.players[0].x, 100.5);
    ASSERT_EQ(deserialized.projectiles.size(), 1);
    EXPECT_EQ(deserialized.projectiles[0].id, 1u);
}

TEST_F(SnapshotManagerTest, CalculateDelta) {
    std::vector<PlayerState> players1;
    PlayerState p1;
    p1.player_id = "player1";
    p1.x = 10.0;
    p1.y = 20.0;
    p1.health = 100;
    players1.push_back(p1);

    auto snapshot1 = manager_.CreateSnapshot(players1, {});
    manager_.SaveSnapshot(snapshot1);

    // 플레이어 위치 변경
    std::vector<PlayerState> players2;
    PlayerState p2;
    p2.player_id = "player1";
    p2.x = 15.0;  // 변경됨
    p2.y = 20.0;  // 동일
    p2.health = 100;  // 동일
    players2.push_back(p2);

    auto snapshot2 = manager_.CreateSnapshot(players2, {});
    manager_.SaveSnapshot(snapshot2);

    auto delta = manager_.CalculateDelta(1, 2);
    ASSERT_TRUE(delta.has_value());
    EXPECT_EQ(delta->base_sequence, 1u);
    EXPECT_EQ(delta->target_sequence, 2u);
    // 델타는 변경분만 포함해야 함
    EXPECT_GT(delta->changes.size(), 0u);
}

TEST_F(SnapshotManagerTest, ApplyDelta) {
    Snapshot base;
    base.sequence = 1;
    
    PlayerState p1;
    p1.player_id = "player1";
    p1.x = 10.0;
    p1.y = 20.0;
    p1.health = 100;
    p1.is_alive = true;
    base.players.push_back(p1);

    manager_.SaveSnapshot(base);

    // 변경된 상태
    Snapshot target;
    target.sequence = 2;
    
    PlayerState p2 = p1;
    p2.x = 15.0;  // x만 변경
    target.players.push_back(p2);

    manager_.SaveSnapshot(target);

    auto delta = manager_.CalculateDelta(1, 2);
    ASSERT_TRUE(delta.has_value());

    auto result = SnapshotManager::ApplyDelta(base, *delta);
    EXPECT_EQ(result.sequence, 2u);
    ASSERT_EQ(result.players.size(), 1);
    EXPECT_DOUBLE_EQ(result.players[0].x, 15.0);
    EXPECT_DOUBLE_EQ(result.players[0].y, 20.0);  // 변경되지 않음
}

TEST_F(SnapshotManagerTest, DeltaSerializeDeserialize) {
    Delta delta;
    delta.base_sequence = 100;
    delta.target_sequence = 105;
    delta.changes = {0x01, 0x02, 0x03, 0x04, 0x05};

    auto serialized = delta.Serialize();
    auto deserialized = Delta::Deserialize(serialized);

    EXPECT_EQ(deserialized.base_sequence, delta.base_sequence);
    EXPECT_EQ(deserialized.target_sequence, delta.target_sequence);
    EXPECT_EQ(deserialized.changes, delta.changes);
}

TEST_F(SnapshotManagerTest, EstimatedSize) {
    Snapshot snapshot;
    snapshot.sequence = 1;
    snapshot.timestamp = 1000;

    // 빈 스냅샷
    EXPECT_GT(snapshot.EstimatedSize(), 0u);

    // 플레이어 추가
    PlayerState p1;
    p1.player_id = "test";
    snapshot.players.push_back(p1);

    std::size_t size_with_player = snapshot.EstimatedSize();
    EXPECT_GT(size_with_player, 14u);  // 헤더보다 커야 함

    // 발사체 추가
    ProjectileSnapshot proj;
    proj.id = 0;
    proj.owner_id = "test";
    proj.x = 0.0f;
    proj.y = 0.0f;
    proj.velocity_x = 0.0f;
    proj.velocity_y = 0.0f;
    snapshot.projectiles.push_back(proj);

    std::size_t size_with_projectile = snapshot.EstimatedSize();
    EXPECT_GT(size_with_projectile, size_with_player);
}
