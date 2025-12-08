#include <gtest/gtest.h>

#include "pvpserver/network/packet_types.h"

using namespace pvpserver;

class PacketTypesTest : public ::testing::Test {};

TEST_F(PacketTypesTest, PacketHeaderSerializeDeserialize) {
    PacketHeader header;
    header.type = PacketType::INPUT;
    header.sequence = 12345;
    header.length = 100;

    auto serialized = header.Serialize();
    ASSERT_EQ(serialized.size(), PacketHeader::SIZE);

    auto deserialized = PacketHeader::Deserialize(serialized);
    EXPECT_EQ(deserialized.type, header.type);
    EXPECT_EQ(deserialized.sequence, header.sequence);
    EXPECT_EQ(deserialized.length, header.length);
}

TEST_F(PacketTypesTest, PacketHeaderIsValid) {
    std::vector<std::uint8_t> valid_data(PacketHeader::SIZE, 0);
    EXPECT_TRUE(PacketHeader::IsValid(valid_data));

    std::vector<std::uint8_t> invalid_data(PacketHeader::SIZE - 1, 0);
    EXPECT_FALSE(PacketHeader::IsValid(invalid_data));

    std::vector<std::uint8_t> empty_data;
    EXPECT_FALSE(PacketHeader::IsValid(empty_data));
}

TEST_F(PacketTypesTest, ConnectPacketSerializeDeserialize) {
    ConnectPacket packet;
    packet.player_id = "player123";
    packet.client_version = 10200;

    auto serialized = packet.Serialize();
    auto deserialized = ConnectPacket::Deserialize(serialized);

    EXPECT_EQ(deserialized.player_id, packet.player_id);
    EXPECT_EQ(deserialized.client_version, packet.client_version);
}

TEST_F(PacketTypesTest, ConnectAckPacketSerializeDeserialize) {
    ConnectAckPacket packet;
    packet.assigned_id = "player456";
    packet.server_tick = 1000;
    packet.tick_rate = 60;

    auto serialized = packet.Serialize();
    auto deserialized = ConnectAckPacket::Deserialize(serialized);

    EXPECT_EQ(deserialized.assigned_id, packet.assigned_id);
    EXPECT_EQ(deserialized.server_tick, packet.server_tick);
    EXPECT_EQ(deserialized.tick_rate, packet.tick_rate);
}

TEST_F(PacketTypesTest, InputCommandSerializeDeserialize) {
    InputCommand cmd;
    cmd.sequence = 999;
    cmd.client_timestamp = 1234567890123ULL;
    cmd.move_x = 0.5f;
    cmd.move_y = -0.3f;
    cmd.aim_radians = 1.57f;
    cmd.fire = true;

    auto serialized = cmd.Serialize();
    auto deserialized = InputCommand::Deserialize(serialized);

    EXPECT_EQ(deserialized.sequence, cmd.sequence);
    EXPECT_EQ(deserialized.client_timestamp, cmd.client_timestamp);
    EXPECT_FLOAT_EQ(deserialized.move_x, cmd.move_x);
    EXPECT_FLOAT_EQ(deserialized.move_y, cmd.move_y);
    EXPECT_FLOAT_EQ(deserialized.aim_radians, cmd.aim_radians);
    EXPECT_EQ(deserialized.fire, cmd.fire);
}

TEST_F(PacketTypesTest, PlayerSnapshotSerializeDeserialize) {
    PlayerSnapshot snapshot;
    snapshot.player_id = "test_player";
    snapshot.x = 100.5f;
    snapshot.y = 200.25f;
    snapshot.facing_radians = 3.14f;
    snapshot.health = 75;
    snapshot.is_alive = true;
    snapshot.last_input_sequence = 42;

    auto serialized = snapshot.Serialize();
    std::size_t offset = 0;
    auto deserialized = PlayerSnapshot::Deserialize(serialized, offset);

    EXPECT_EQ(deserialized.player_id, snapshot.player_id);
    EXPECT_FLOAT_EQ(deserialized.x, snapshot.x);
    EXPECT_FLOAT_EQ(deserialized.y, snapshot.y);
    EXPECT_FLOAT_EQ(deserialized.facing_radians, snapshot.facing_radians);
    EXPECT_EQ(deserialized.health, snapshot.health);
    EXPECT_EQ(deserialized.is_alive, snapshot.is_alive);
    EXPECT_EQ(deserialized.last_input_sequence, snapshot.last_input_sequence);
}

TEST_F(PacketTypesTest, ProjectileSnapshotSerializeDeserialize) {
    ProjectileSnapshot snapshot;
    snapshot.id = 12345;
    snapshot.owner_id = "shooter";
    snapshot.x = 50.0f;
    snapshot.y = 75.0f;
    snapshot.velocity_x = 10.0f;
    snapshot.velocity_y = -5.0f;

    auto serialized = snapshot.Serialize();
    std::size_t offset = 0;
    auto deserialized = ProjectileSnapshot::Deserialize(serialized, offset);

    EXPECT_EQ(deserialized.id, snapshot.id);
    EXPECT_EQ(deserialized.owner_id, snapshot.owner_id);
    EXPECT_FLOAT_EQ(deserialized.x, snapshot.x);
    EXPECT_FLOAT_EQ(deserialized.y, snapshot.y);
    EXPECT_FLOAT_EQ(deserialized.velocity_x, snapshot.velocity_x);
    EXPECT_FLOAT_EQ(deserialized.velocity_y, snapshot.velocity_y);
}

TEST_F(PacketTypesTest, GameEventSerializeDeserialize) {
    GameEvent event;
    event.type = GameEventType::PLAYER_DEATH;
    event.timestamp = 9876543210ULL;
    event.data = "victim_id:player1";

    auto serialized = event.Serialize();
    auto deserialized = GameEvent::Deserialize(serialized);

    EXPECT_EQ(deserialized.type, event.type);
    EXPECT_EQ(deserialized.timestamp, event.timestamp);
    EXPECT_EQ(deserialized.data, event.data);
}

TEST_F(PacketTypesTest, EmptyStringHandling) {
    ConnectPacket packet;
    packet.player_id = "";
    packet.client_version = 100;

    auto serialized = packet.Serialize();
    auto deserialized = ConnectPacket::Deserialize(serialized);

    EXPECT_EQ(deserialized.player_id, "");
    EXPECT_EQ(deserialized.client_version, 100);
}

TEST_F(PacketTypesTest, LongStringTruncation) {
    ConnectPacket packet;
    // 256자 이상의 문자열
    packet.player_id = std::string(300, 'a');
    packet.client_version = 100;

    auto serialized = packet.Serialize();
    auto deserialized = ConnectPacket::Deserialize(serialized);

    // 255자로 잘려야 함
    EXPECT_EQ(deserialized.player_id.size(), 255);
}
