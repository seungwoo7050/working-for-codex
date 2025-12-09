#include "pvpserver/anticheat/replay_recorder.h"

#include <algorithm>
#include <chrono>
#include <cstring>

namespace pvpserver::anticheat {

// ReplayRecorder 구현
ReplayRecorder::ReplayRecorder() = default;
ReplayRecorder::~ReplayRecorder() = default;

void ReplayRecorder::StartRecording(const std::string& match_id) {
    match_id_ = match_id;
    frames_.clear();
    recording_ = true;
    start_time_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void ReplayRecorder::RecordFrame(const ReplayFrame& frame) {
    if (!recording_) return;
    frames_.push_back(frame);
}

void ReplayRecorder::StopRecording() { recording_ = false; }

std::vector<uint8_t> ReplayRecorder::GetCompressedReplay() { return Compress(frames_); }

std::vector<uint8_t> ReplayRecorder::GetPlayerPOV(const std::string& player_id) {
    auto pov_frames = ExtractPOVFrames(player_id);
    return Compress(pov_frames);
}

ReplayMetadata ReplayRecorder::BuildMetadata() const {
    ReplayMetadata meta;
    meta.match_id = match_id_;
    meta.recorded_at = start_time_;

    if (!frames_.empty()) {
        meta.duration_seconds =
            static_cast<int>((frames_.back().timestamp - frames_.front().timestamp) / 1000);

        // 참여 플레이어 수집
        std::set<std::string> players;
        for (const auto& frame : frames_) {
            for (const auto& p : frame.players) {
                players.insert(p.player_id);
            }
        }
        meta.players = std::vector<std::string>(players.begin(), players.end());
    }

    return meta;
}

std::vector<uint8_t> ReplayRecorder::Compress(const std::vector<ReplayFrame>& frames) {
    // 간단한 직렬화 (실제로는 protobuf/flatbuffers + zstd 압축 사용)
    std::vector<uint8_t> data;

    // 헤더: 프레임 수
    uint32_t frame_count = static_cast<uint32_t>(frames.size());
    data.resize(sizeof(uint32_t));
    std::memcpy(data.data(), &frame_count, sizeof(uint32_t));

    // 각 프레임 직렬화 (간소화된 버전)
    for (const auto& frame : frames) {
        // tick, timestamp
        size_t pos = data.size();
        data.resize(pos + sizeof(int64_t) * 2);
        std::memcpy(data.data() + pos, &frame.tick, sizeof(int64_t));
        std::memcpy(data.data() + pos + sizeof(int64_t), &frame.timestamp, sizeof(int64_t));

        // 플레이어 수
        uint32_t player_count = static_cast<uint32_t>(frame.players.size());
        pos = data.size();
        data.resize(pos + sizeof(uint32_t));
        std::memcpy(data.data() + pos, &player_count, sizeof(uint32_t));

        // 각 플레이어 (ID 길이 + ID + position)
        for (const auto& p : frame.players) {
            pos = data.size();
            uint32_t id_len = static_cast<uint32_t>(p.player_id.size());
            data.resize(pos + sizeof(uint32_t) + id_len + sizeof(float) * 3);
            std::memcpy(data.data() + pos, &id_len, sizeof(uint32_t));
            std::memcpy(data.data() + pos + sizeof(uint32_t), p.player_id.data(), id_len);
            std::memcpy(data.data() + pos + sizeof(uint32_t) + id_len, &p.position.x,
                        sizeof(float) * 3);
        }
    }

    return data;
}

std::vector<ReplayFrame> ReplayRecorder::ExtractPOVFrames(const std::string& player_id) {
    std::vector<ReplayFrame> pov_frames;
    pov_frames.reserve(frames_.size());

    for (const auto& frame : frames_) {
        ReplayFrame pov_frame;
        pov_frame.tick = frame.tick;
        pov_frame.timestamp = frame.timestamp;

        // 해당 플레이어 관점에서 보이는 정보만 포함
        for (const auto& p : frame.players) {
            // 본인 또는 가시 범위 내 플레이어만 포함
            // (실제로는 가시성 체크 필요)
            pov_frame.players.push_back(p);
        }

        // 관련 이벤트만 포함
        for (const auto& e : frame.events) {
            if (e.player_id == player_id || e.target_id == player_id) {
                pov_frame.events.push_back(e);
            }
        }

        pov_frames.push_back(std::move(pov_frame));
    }

    return pov_frames;
}

// InMemoryReplayStorage 구현
void InMemoryReplayStorage::SaveReplay(const std::string& match_id,
                                       const std::vector<uint8_t>& data) {
    replays_[match_id] = data;
}

std::optional<std::vector<uint8_t>> InMemoryReplayStorage::LoadReplay(const std::string& match_id) {
    auto it = replays_.find(match_id);
    if (it == replays_.end()) return std::nullopt;
    return it->second;
}

void InMemoryReplayStorage::DeleteReplay(const std::string& match_id) {
    replays_.erase(match_id);
    metadata_.erase(match_id);
}

void InMemoryReplayStorage::SaveMetadata(const std::string& match_id,
                                         const ReplayMetadata& metadata) {
    metadata_[match_id] = metadata;
}

std::optional<ReplayMetadata> InMemoryReplayStorage::GetMetadata(const std::string& match_id) {
    auto it = metadata_.find(match_id);
    if (it == metadata_.end()) return std::nullopt;
    return it->second;
}

std::vector<ReplayMetadata> InMemoryReplayStorage::GetReplaysByPlayer(
    const std::string& player_id) {
    std::vector<ReplayMetadata> result;
    for (const auto& [id, meta] : metadata_) {
        if (std::find(meta.players.begin(), meta.players.end(), player_id) != meta.players.end()) {
            result.push_back(meta);
        }
    }
    return result;
}

void InMemoryReplayStorage::CleanupOldReplays(int64_t older_than_timestamp) {
    std::vector<std::string> to_delete;
    for (const auto& [id, meta] : metadata_) {
        if (meta.recorded_at < older_than_timestamp) {
            to_delete.push_back(id);
        }
    }
    for (const auto& id : to_delete) {
        DeleteReplay(id);
    }
}

}  // namespace pvpserver::anticheat
