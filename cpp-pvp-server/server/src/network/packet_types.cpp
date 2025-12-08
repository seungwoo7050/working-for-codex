// [FILE]
// 목적: 패킷 타입 정의 및 직렬화 - UDP 통신용 바이너리 프로토콜
// - 관련 클론 가이드 단계: [v1.4.0] UDP 권위 서버 코어
// - 이 파일을 읽기 전에: design/v1.4.0-udp-netcode.md 참고
// - 학습 포인트:
//   [Order 1] Big-Endian 직렬화: 네트워크 바이트 순서로 다중 플랫폼 호환
//   [Order 2] memcpy로 float↔uint32 변환: IEEE 754 비트 패턴 보존
//   [Order 3] 버퍼 언더플로우 체크: 안전한 역직렬화를 위한 경계 검사
// - [LEARN] C와 유사: struct 패킹 대신 명시적 직렬화로 패딩 문제 방지
// - [Reader Notes] Protocol Buffers 등 IDL 대신 수동 직렬화 → 성능과 크기 최적화
// - 다음에 읽을 파일: snapshot_manager.cpp (이 패킷 타입을 사용하는 스냅샷 시스템)

#include "pvpserver/network/packet_types.h"

#include <cstring>
#include <stdexcept>

namespace pvpserver {

// Helper functions for serialization
namespace {

void WriteUint8(std::vector<std::uint8_t>& buffer, std::uint8_t value) {
    buffer.push_back(value);
}

void WriteUint16BE(std::vector<std::uint8_t>& buffer, std::uint16_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void WriteUint32BE(std::vector<std::uint8_t>& buffer, std::uint32_t value) {
    buffer.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<std::uint8_t>(value & 0xFF));
}

void WriteUint64BE(std::vector<std::uint8_t>& buffer, std::uint64_t value) {
    for (int i = 7; i >= 0; --i) {
        buffer.push_back(static_cast<std::uint8_t>((value >> (i * 8)) & 0xFF));
    }
}

void WriteFloat(std::vector<std::uint8_t>& buffer, float value) {
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    WriteUint32BE(buffer, bits);
}

void WriteString(std::vector<std::uint8_t>& buffer, const std::string& str) {
    std::uint8_t len = static_cast<std::uint8_t>(std::min(str.size(), static_cast<std::size_t>(255)));
    WriteUint8(buffer, len);
    buffer.insert(buffer.end(), str.begin(), str.begin() + len);
}

std::uint8_t ReadUint8(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    if (offset >= data.size()) throw std::runtime_error("Buffer underflow");
    return data[offset++];
}

std::uint16_t ReadUint16BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    if (offset + 2 > data.size()) throw std::runtime_error("Buffer underflow");
    std::uint16_t value = (static_cast<std::uint16_t>(data[offset]) << 8) |
                          static_cast<std::uint16_t>(data[offset + 1]);
    offset += 2;
    return value;
}

std::uint32_t ReadUint32BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    if (offset + 4 > data.size()) throw std::runtime_error("Buffer underflow");
    std::uint32_t value = (static_cast<std::uint32_t>(data[offset]) << 24) |
                          (static_cast<std::uint32_t>(data[offset + 1]) << 16) |
                          (static_cast<std::uint32_t>(data[offset + 2]) << 8) |
                          static_cast<std::uint32_t>(data[offset + 3]);
    offset += 4;
    return value;
}

std::uint64_t ReadUint64BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    if (offset + 8 > data.size()) throw std::runtime_error("Buffer underflow");
    std::uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | data[offset + i];
    }
    offset += 8;
    return value;
}

float ReadFloat(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    std::uint32_t bits = ReadUint32BE(data, offset);
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

std::string ReadString(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    std::uint8_t len = ReadUint8(data, offset);
    if (offset + len > data.size()) throw std::runtime_error("Buffer underflow");
    std::string str(data.begin() + offset, data.begin() + offset + len);
    offset += len;
    return str;
}

}  // namespace

// PacketHeader
std::vector<std::uint8_t> PacketHeader::Serialize() const {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(SIZE);
    WriteUint8(buffer, static_cast<std::uint8_t>(type));
    WriteUint16BE(buffer, sequence);
    WriteUint8(buffer, length);
    return buffer;
}

PacketHeader PacketHeader::Deserialize(const std::vector<std::uint8_t>& data) {
    if (data.size() < SIZE) {
        throw std::runtime_error("Invalid packet header size");
    }
    std::size_t offset = 0;
    PacketHeader header;
    header.type = static_cast<PacketType>(ReadUint8(data, offset));
    header.sequence = ReadUint16BE(data, offset);
    header.length = ReadUint8(data, offset);
    return header;
}

bool PacketHeader::IsValid(const std::vector<std::uint8_t>& data) {
    return data.size() >= SIZE;
}

// ConnectPacket
std::vector<std::uint8_t> ConnectPacket::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteString(buffer, player_id);
    WriteUint32BE(buffer, client_version);
    return buffer;
}

ConnectPacket ConnectPacket::Deserialize(const std::vector<std::uint8_t>& payload) {
    std::size_t offset = 0;
    ConnectPacket packet;
    packet.player_id = ReadString(payload, offset);
    packet.client_version = ReadUint32BE(payload, offset);
    return packet;
}

// ConnectAckPacket
std::vector<std::uint8_t> ConnectAckPacket::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteString(buffer, assigned_id);
    WriteUint32BE(buffer, server_tick);
    WriteUint16BE(buffer, tick_rate);
    return buffer;
}

ConnectAckPacket ConnectAckPacket::Deserialize(const std::vector<std::uint8_t>& payload) {
    std::size_t offset = 0;
    ConnectAckPacket packet;
    packet.assigned_id = ReadString(payload, offset);
    packet.server_tick = ReadUint32BE(payload, offset);
    packet.tick_rate = ReadUint16BE(payload, offset);
    return packet;
}

// InputCommand
std::vector<std::uint8_t> InputCommand::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteUint32BE(buffer, sequence);
    WriteUint64BE(buffer, client_timestamp);
    WriteFloat(buffer, move_x);
    WriteFloat(buffer, move_y);
    WriteFloat(buffer, aim_radians);
    WriteUint8(buffer, fire ? 1 : 0);
    return buffer;
}

InputCommand InputCommand::Deserialize(const std::vector<std::uint8_t>& payload) {
    std::size_t offset = 0;
    InputCommand cmd;
    cmd.sequence = ReadUint32BE(payload, offset);
    cmd.client_timestamp = ReadUint64BE(payload, offset);
    cmd.move_x = ReadFloat(payload, offset);
    cmd.move_y = ReadFloat(payload, offset);
    cmd.aim_radians = ReadFloat(payload, offset);
    cmd.fire = ReadUint8(payload, offset) != 0;
    return cmd;
}

// PlayerSnapshot
std::vector<std::uint8_t> PlayerSnapshot::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteString(buffer, player_id);
    WriteFloat(buffer, x);
    WriteFloat(buffer, y);
    WriteFloat(buffer, facing_radians);
    WriteUint32BE(buffer, static_cast<std::uint32_t>(health));
    WriteUint8(buffer, is_alive ? 1 : 0);
    WriteUint32BE(buffer, last_input_sequence);
    return buffer;
}

PlayerSnapshot PlayerSnapshot::Deserialize(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    PlayerSnapshot snapshot;
    snapshot.player_id = ReadString(data, offset);
    snapshot.x = ReadFloat(data, offset);
    snapshot.y = ReadFloat(data, offset);
    snapshot.facing_radians = ReadFloat(data, offset);
    snapshot.health = static_cast<std::int32_t>(ReadUint32BE(data, offset));
    snapshot.is_alive = ReadUint8(data, offset) != 0;
    snapshot.last_input_sequence = ReadUint32BE(data, offset);
    return snapshot;
}

// ProjectileSnapshot
std::vector<std::uint8_t> ProjectileSnapshot::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteUint32BE(buffer, id);
    WriteString(buffer, owner_id);
    WriteFloat(buffer, x);
    WriteFloat(buffer, y);
    WriteFloat(buffer, velocity_x);
    WriteFloat(buffer, velocity_y);
    return buffer;
}

ProjectileSnapshot ProjectileSnapshot::Deserialize(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    ProjectileSnapshot snapshot;
    snapshot.id = ReadUint32BE(data, offset);
    snapshot.owner_id = ReadString(data, offset);
    snapshot.x = ReadFloat(data, offset);
    snapshot.y = ReadFloat(data, offset);
    snapshot.velocity_x = ReadFloat(data, offset);
    snapshot.velocity_y = ReadFloat(data, offset);
    return snapshot;
}

// GameEvent
std::vector<std::uint8_t> GameEvent::Serialize() const {
    std::vector<std::uint8_t> buffer;
    WriteUint8(buffer, static_cast<std::uint8_t>(type));
    WriteUint64BE(buffer, timestamp);
    WriteString(buffer, data);
    return buffer;
}

GameEvent GameEvent::Deserialize(const std::vector<std::uint8_t>& payload) {
    std::size_t offset = 0;
    GameEvent event;
    event.type = static_cast<GameEventType>(ReadUint8(payload, offset));
    event.timestamp = ReadUint64BE(payload, offset);
    event.data = ReadString(payload, offset);
    return event;
}

}  // namespace pvpserver
