#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pvpserver {

/**
 * @brief UDP 패킷 타입 정의
 */
enum class PacketType : std::uint8_t {
    // 연결 관리
    CONNECT = 0x01,
    CONNECT_ACK = 0x02,
    DISCONNECT = 0x03,
    HEARTBEAT = 0x04,
    HEARTBEAT_ACK = 0x05,

    // 게임 입력 (클라이언트 → 서버)
    INPUT = 0x10,
    INPUT_ACK = 0x11,

    // 게임 상태 (서버 → 클라이언트)
    STATE_FULL = 0x20,
    STATE_DELTA = 0x21,

    // 이벤트
    EVENT = 0x30,
};

/**
 * @brief 패킷 헤더 (4 바이트)
 * 
 * +--------+--------+--------+--------+
 * | Type   | SeqNum (BE)     | Length |
 * | (1B)   | (2B)            | (1B)   |
 * +--------+--------+--------+--------+
 */
struct PacketHeader {
    PacketType type;
    std::uint16_t sequence;
    std::uint8_t length;  // 페이로드 길이 (헤더 제외)

    static constexpr std::size_t SIZE = 4;

    std::vector<std::uint8_t> Serialize() const;
    static PacketHeader Deserialize(const std::vector<std::uint8_t>& data);
    static bool IsValid(const std::vector<std::uint8_t>& data);
};

/**
 * @brief 연결 요청 패킷
 */
struct ConnectPacket {
    std::string player_id;
    std::uint32_t client_version;

    std::vector<std::uint8_t> Serialize() const;
    static ConnectPacket Deserialize(const std::vector<std::uint8_t>& payload);
};

/**
 * @brief 연결 승인 패킷
 */
struct ConnectAckPacket {
    std::string assigned_id;
    std::uint32_t server_tick;
    std::uint16_t tick_rate;

    std::vector<std::uint8_t> Serialize() const;
    static ConnectAckPacket Deserialize(const std::vector<std::uint8_t>& payload);
};

/**
 * @brief 입력 커맨드
 */
struct InputCommand {
    std::uint32_t sequence;
    std::uint64_t client_timestamp;
    float move_x;
    float move_y;
    float aim_radians;
    bool fire;
    
    std::vector<std::uint8_t> Serialize() const;
    static InputCommand Deserialize(const std::vector<std::uint8_t>& payload);
};

/**
 * @brief 플레이어 상태 (스냅샷용)
 */
struct PlayerSnapshot {
    std::string player_id;
    float x;
    float y;
    float facing_radians;
    std::int32_t health;
    bool is_alive;
    std::uint32_t last_input_sequence;

    std::vector<std::uint8_t> Serialize() const;
    static PlayerSnapshot Deserialize(const std::vector<std::uint8_t>& data, std::size_t& offset);
};

/**
 * @brief 발사체 상태 (스냅샷용)
 */
struct ProjectileSnapshot {
    std::uint32_t id;
    std::string owner_id;
    float x;
    float y;
    float velocity_x;
    float velocity_y;

    std::vector<std::uint8_t> Serialize() const;
    static ProjectileSnapshot Deserialize(const std::vector<std::uint8_t>& data, std::size_t& offset);
};

/**
 * @brief 게임 이벤트
 */
enum class GameEventType : std::uint8_t {
    PLAYER_JOINED = 0x01,
    PLAYER_LEFT = 0x02,
    PLAYER_DEATH = 0x10,
    PLAYER_RESPAWN = 0x11,
    PROJECTILE_HIT = 0x20,
    MATCH_START = 0x30,
    MATCH_END = 0x31,
};

struct GameEvent {
    GameEventType type;
    std::uint64_t timestamp;
    std::string data;  // JSON 또는 간단한 문자열

    std::vector<std::uint8_t> Serialize() const;
    static GameEvent Deserialize(const std::vector<std::uint8_t>& payload);
};

}  // namespace pvpserver
