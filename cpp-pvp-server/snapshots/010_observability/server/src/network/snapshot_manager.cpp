#include "pvpserver/network/snapshot_manager.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>

namespace pvpserver {

namespace {

// 직렬화 헬퍼
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
    return data[offset++];
}

std::uint16_t ReadUint16BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    std::uint16_t value = (static_cast<std::uint16_t>(data[offset]) << 8) |
                          static_cast<std::uint16_t>(data[offset + 1]);
    offset += 2;
    return value;
}

std::uint32_t ReadUint32BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
    std::uint32_t value = (static_cast<std::uint32_t>(data[offset]) << 24) |
                          (static_cast<std::uint32_t>(data[offset + 1]) << 16) |
                          (static_cast<std::uint32_t>(data[offset + 2]) << 8) |
                          static_cast<std::uint32_t>(data[offset + 3]);
    offset += 4;
    return value;
}

std::uint64_t ReadUint64BE(const std::vector<std::uint8_t>& data, std::size_t& offset) {
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
    std::string str(data.begin() + offset, data.begin() + offset + len);
    offset += len;
    return str;
}

std::uint64_t CurrentTimeMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

}  // namespace

// Snapshot
std::vector<std::uint8_t> Snapshot::Serialize() const {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(EstimatedSize());
    
    WriteUint32BE(buffer, sequence);
    WriteUint64BE(buffer, timestamp);
    
    // 플레이어 수
    WriteUint8(buffer, static_cast<std::uint8_t>(std::min(players.size(), static_cast<std::size_t>(255))));
    
    for (const auto& player : players) {
        WriteString(buffer, player.player_id);
        WriteFloat(buffer, static_cast<float>(player.x));
        WriteFloat(buffer, static_cast<float>(player.y));
        WriteFloat(buffer, static_cast<float>(player.facing_radians));
        WriteUint32BE(buffer, static_cast<std::uint32_t>(player.health));
        WriteUint8(buffer, player.is_alive ? 1 : 0);
        WriteUint32BE(buffer, static_cast<std::uint32_t>(player.last_sequence));
    }
    
    // 발사체 수
    WriteUint8(buffer, static_cast<std::uint8_t>(std::min(projectiles.size(), static_cast<std::size_t>(255))));
    
    for (const auto& proj : projectiles) {
        WriteUint32BE(buffer, static_cast<std::uint32_t>(proj.id));
        WriteString(buffer, proj.owner_id);
        WriteFloat(buffer, static_cast<float>(proj.x));
        WriteFloat(buffer, static_cast<float>(proj.y));
        WriteFloat(buffer, static_cast<float>(proj.vx));
        WriteFloat(buffer, static_cast<float>(proj.vy));
    }
    
    return buffer;
}

Snapshot Snapshot::Deserialize(const std::vector<std::uint8_t>& data) {
    Snapshot snapshot;
    std::size_t offset = 0;
    
    snapshot.sequence = ReadUint32BE(data, offset);
    snapshot.timestamp = ReadUint64BE(data, offset);
    
    std::uint8_t player_count = ReadUint8(data, offset);
    snapshot.players.reserve(player_count);
    
    for (std::uint8_t i = 0; i < player_count; ++i) {
        PlayerState player;
        player.player_id = ReadString(data, offset);
        player.x = ReadFloat(data, offset);
        player.y = ReadFloat(data, offset);
        player.facing_radians = ReadFloat(data, offset);
        player.health = static_cast<int>(ReadUint32BE(data, offset));
        player.is_alive = ReadUint8(data, offset) != 0;
        player.last_sequence = ReadUint32BE(data, offset);
        snapshot.players.push_back(player);
    }
    
    std::uint8_t proj_count = ReadUint8(data, offset);
    snapshot.projectiles.reserve(proj_count);
    
    for (std::uint8_t i = 0; i < proj_count; ++i) {
        Projectile proj;
        proj.id = ReadUint32BE(data, offset);
        proj.owner_id = ReadString(data, offset);
        proj.x = ReadFloat(data, offset);
        proj.y = ReadFloat(data, offset);
        proj.vx = ReadFloat(data, offset);
        proj.vy = ReadFloat(data, offset);
        snapshot.projectiles.push_back(proj);
    }
    
    return snapshot;
}

std::size_t Snapshot::EstimatedSize() const {
    // 헤더: sequence(4) + timestamp(8) + player_count(1) + proj_count(1)
    std::size_t size = 14;
    
    // 각 플레이어: id_len(1) + id + x(4) + y(4) + facing(4) + health(4) + alive(1) + seq(4)
    for (const auto& player : players) {
        size += 1 + player.player_id.size() + 21;
    }
    
    // 각 발사체: id(4) + owner_len(1) + owner + x(4) + y(4) + vx(4) + vy(4)
    for (const auto& proj : projectiles) {
        size += 21 + proj.owner_id.size();
    }
    
    return size;
}

// Delta
std::vector<std::uint8_t> Delta::Serialize() const {
    std::vector<std::uint8_t> buffer;
    buffer.reserve(8 + changes.size());
    
    WriteUint32BE(buffer, base_sequence);
    WriteUint32BE(buffer, target_sequence);
    buffer.insert(buffer.end(), changes.begin(), changes.end());
    
    return buffer;
}

Delta Delta::Deserialize(const std::vector<std::uint8_t>& data) {
    Delta delta;
    std::size_t offset = 0;
    
    delta.base_sequence = ReadUint32BE(data, offset);
    delta.target_sequence = ReadUint32BE(data, offset);
    delta.changes.assign(data.begin() + offset, data.end());
    
    return delta;
}

// SnapshotManager
Snapshot SnapshotManager::CreateSnapshot(
    const std::vector<PlayerState>& players,
    const std::vector<Projectile>& projectiles
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Snapshot snapshot;
    snapshot.sequence = ++current_sequence_;
    snapshot.timestamp = CurrentTimeMs();
    snapshot.players = players;
    snapshot.projectiles = projectiles;
    
    return snapshot;
}

void SnapshotManager::SaveSnapshot(const Snapshot& snapshot) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    buffer_[head_] = snapshot;
    head_ = (head_ + 1) % BUFFER_SIZE;
    if (count_ < BUFFER_SIZE) {
        ++count_;
    }
}

std::optional<Snapshot> SnapshotManager::GetSnapshot(std::uint32_t sequence) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto idx = FindIndex(sequence);
    if (!idx) {
        return std::nullopt;
    }
    
    return buffer_[*idx];
}

std::optional<Snapshot> SnapshotManager::GetLatestSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (count_ == 0) {
        return std::nullopt;
    }
    
    std::size_t latest = (head_ + BUFFER_SIZE - 1) % BUFFER_SIZE;
    return buffer_[latest];
}

std::optional<Snapshot> SnapshotManager::GetSnapshotAt(std::uint64_t timestamp) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (count_ == 0) {
        return std::nullopt;
    }
    
    // 타임스탬프에 가장 가까운 두 스냅샷 찾기
    std::size_t before_idx = 0;
    std::size_t after_idx = 0;
    bool found_before = false;
    bool found_after = false;
    
    for (std::size_t i = 0; i < count_; ++i) {
        std::size_t idx = (head_ + BUFFER_SIZE - 1 - i) % BUFFER_SIZE;
        const auto& snap = buffer_[idx];
        
        if (snap.timestamp <= timestamp) {
            before_idx = idx;
            found_before = true;
            if (i > 0) {
                after_idx = (idx + 1) % BUFFER_SIZE;
                found_after = true;
            }
            break;
        }
    }
    
    if (!found_before) {
        // 모든 스냅샷이 요청된 시간 이후
        std::size_t oldest = (head_ + BUFFER_SIZE - count_) % BUFFER_SIZE;
        return buffer_[oldest];
    }
    
    if (!found_after) {
        // 정확히 일치하거나 이전 스냅샷만 있음
        return buffer_[before_idx];
    }
    
    // 보간
    const auto& before = buffer_[before_idx];
    const auto& after = buffer_[after_idx];
    
    if (after.timestamp == before.timestamp) {
        return after;
    }
    
    float t = static_cast<float>(timestamp - before.timestamp) /
              static_cast<float>(after.timestamp - before.timestamp);
    t = std::clamp(t, 0.0f, 1.0f);
    
    // 보간된 스냅샷 생성
    Snapshot interpolated;
    interpolated.sequence = after.sequence;
    interpolated.timestamp = timestamp;
    
    // 플레이어 보간
    for (const auto& after_player : after.players) {
        PlayerState interp_player = after_player;
        
        // 이전 스냅샷에서 같은 플레이어 찾기
        for (const auto& before_player : before.players) {
            if (before_player.player_id == after_player.player_id) {
                interp_player.x = before_player.x + t * (after_player.x - before_player.x);
                interp_player.y = before_player.y + t * (after_player.y - before_player.y);
                // facing은 각도 보간 (단순 선형)
                interp_player.facing_radians = before_player.facing_radians + 
                    t * (after_player.facing_radians - before_player.facing_radians);
                break;
            }
        }
        
        interpolated.players.push_back(interp_player);
    }
    
    // 발사체 보간
    for (const auto& after_proj : after.projectiles) {
        Projectile interp_proj = after_proj;
        
        for (const auto& before_proj : before.projectiles) {
            if (before_proj.id == after_proj.id) {
                interp_proj.x = before_proj.x + t * (after_proj.x - before_proj.x);
                interp_proj.y = before_proj.y + t * (after_proj.y - before_proj.y);
                break;
            }
        }
        
        interpolated.projectiles.push_back(interp_proj);
    }
    
    return interpolated;
}

std::optional<Delta> SnapshotManager::CalculateDelta(
    std::uint32_t base_seq,
    std::uint32_t target_seq
) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto base_idx = FindIndex(base_seq);
    auto target_idx = FindIndex(target_seq);
    
    if (!base_idx || !target_idx) {
        return std::nullopt;
    }
    
    const auto& base = buffer_[*base_idx];
    const auto& target = buffer_[*target_idx];
    
    Delta delta;
    delta.base_sequence = base_seq;
    delta.target_sequence = target_seq;
    
    // 변경된 플레이어만 기록
    // 형식: [player_count][{player_id, change_flags, changed_fields}...]
    
    std::vector<std::uint8_t> changes;
    
    std::uint8_t changed_player_count = 0;
    std::vector<std::uint8_t> player_changes;
    
    for (const auto& target_player : target.players) {
        // 기준 스냅샷에서 찾기
        const PlayerState* base_player = nullptr;
        for (const auto& bp : base.players) {
            if (bp.player_id == target_player.player_id) {
                base_player = &bp;
                break;
            }
        }
        
        if (!base_player) {
            // 새 플레이어
            WriteString(player_changes, target_player.player_id);
            WriteUint8(player_changes, 0xFF);  // 모든 필드 변경
            WriteFloat(player_changes, static_cast<float>(target_player.x));
            WriteFloat(player_changes, static_cast<float>(target_player.y));
            WriteFloat(player_changes, static_cast<float>(target_player.facing_radians));
            WriteUint32BE(player_changes, static_cast<std::uint32_t>(target_player.health));
            WriteUint8(player_changes, target_player.is_alive ? 1 : 0);
            ++changed_player_count;
        } else {
            std::uint8_t flags = ComparePlayerStates(*base_player, target_player);
            if (flags != 0) {
                WriteString(player_changes, target_player.player_id);
                WriteUint8(player_changes, flags);
                
                if (flags & 0x01) WriteFloat(player_changes, static_cast<float>(target_player.x));
                if (flags & 0x02) WriteFloat(player_changes, static_cast<float>(target_player.y));
                if (flags & 0x04) WriteFloat(player_changes, static_cast<float>(target_player.facing_radians));
                if (flags & 0x08) WriteUint32BE(player_changes, static_cast<std::uint32_t>(target_player.health));
                if (flags & 0x10) WriteUint8(player_changes, target_player.is_alive ? 1 : 0);
                
                ++changed_player_count;
            }
        }
    }
    
    WriteUint8(changes, changed_player_count);
    changes.insert(changes.end(), player_changes.begin(), player_changes.end());
    
    delta.changes = std::move(changes);
    return delta;
}

Snapshot SnapshotManager::ApplyDelta(const Snapshot& base, const Delta& delta) {
    Snapshot result = base;
    result.sequence = delta.target_sequence;
    
    if (delta.changes.empty()) {
        return result;
    }
    
    std::size_t offset = 0;
    std::uint8_t changed_count = ReadUint8(delta.changes, offset);
    
    for (std::uint8_t i = 0; i < changed_count && offset < delta.changes.size(); ++i) {
        std::string player_id = ReadString(delta.changes, offset);
        std::uint8_t flags = ReadUint8(delta.changes, offset);
        
        // 결과에서 플레이어 찾기 또는 추가
        PlayerState* player = nullptr;
        for (auto& p : result.players) {
            if (p.player_id == player_id) {
                player = &p;
                break;
            }
        }
        
        if (!player) {
            result.players.push_back(PlayerState{});
            player = &result.players.back();
            player->player_id = player_id;
        }
        
        if (flags & 0x01) player->x = ReadFloat(delta.changes, offset);
        if (flags & 0x02) player->y = ReadFloat(delta.changes, offset);
        if (flags & 0x04) player->facing_radians = ReadFloat(delta.changes, offset);
        if (flags & 0x08) player->health = static_cast<int>(ReadUint32BE(delta.changes, offset));
        if (flags & 0x10) player->is_alive = ReadUint8(delta.changes, offset) != 0;
    }
    
    return result;
}

std::size_t SnapshotManager::BufferedCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return count_;
}

std::optional<std::size_t> SnapshotManager::FindIndex(std::uint32_t sequence) const {
    for (std::size_t i = 0; i < count_; ++i) {
        std::size_t idx = (head_ + BUFFER_SIZE - 1 - i) % BUFFER_SIZE;
        if (buffer_[idx].sequence == sequence) {
            return idx;
        }
    }
    return std::nullopt;
}

std::uint8_t SnapshotManager::ComparePlayerStates(
    const PlayerState& base,
    const PlayerState& target
) {
    std::uint8_t flags = 0;
    
    constexpr float EPSILON = 0.001f;
    
    if (std::abs(base.x - target.x) > EPSILON) flags |= 0x01;
    if (std::abs(base.y - target.y) > EPSILON) flags |= 0x02;
    if (std::abs(base.facing_radians - target.facing_radians) > EPSILON) flags |= 0x04;
    if (base.health != target.health) flags |= 0x08;
    if (base.is_alive != target.is_alive) flags |= 0x10;
    
    return flags;
}

}  // namespace pvpserver
