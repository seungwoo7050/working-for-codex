// [FILE]
// - 목적: 리플레이 녹화 (치트 증거 수집)
// - 주요 역할: 매치 데이터를 프레임별로 기록하여 나중에 검토
// - 관련 클론 가이드 단계: [CG-02.10] 안티치트 시스템
// - 권장 읽는 순서: StartRecording → RecordFrame → GetCompressedReplay
//
// [LEARN] 리플레이 = 게임 상태의 타임라인.
//         - 신고 처리: 관리자가 리플레이로 치트 확인
//         - 자동 분석: 의심 플레이어 행동 패턴 분석
//         - 증거 보존: 항소 시 증거로 활용

#include "pvpserver/anticheat/replay_recorder.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <set>

namespace pvpserver::anticheat {

// [Order 1] ReplayRecorder 생성/소멸
ReplayRecorder::ReplayRecorder() = default;
ReplayRecorder::~ReplayRecorder() = default;

// StartRecording - 녹화 시작
// [LEARN] 매치 시작 시 호출. match_id로 나중에 조회.
void ReplayRecorder::StartRecording(const std::string& match_id) {
    match_id_ = match_id;
    frames_.clear();
    recording_ = true;
    start_time_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
}

// [Order 2] RecordFrame - 매 틱마다 프레임 기록
// [LEARN] 게임 루프에서 호출. 모든 플레이어 위치/상태 저장.
void ReplayRecorder::RecordFrame(const ReplayFrame& frame) {
    if (!recording_) return;
    frames_.push_back(frame);
}

// StopRecording - 녹화 종료
void ReplayRecorder::StopRecording() { recording_ = false; }

// [Order 3] GetCompressedReplay - 압축된 리플레이 데이터 반환
// [LEARN] 저장 공간 절약을 위해 압축. 실제로는 Protobuf + zstd 사용.
std::vector<uint8_t> ReplayRecorder::GetCompressedReplay() { return Compress(frames_); }

// GetPlayerPOV - 특정 플레이어 시점만 추출
// [LEARN] 신고 대상 플레이어의 행동만 집중 분석용
std::vector<uint8_t> ReplayRecorder::GetPlayerPOV(const std::string& player_id) {
    auto pov_frames = ExtractPOVFrames(player_id);
    return Compress(pov_frames);
}

// BuildMetadata - 리플레이 메타데이터 생성
// [LEARN] 인덱싱/검색용. match_id, 플레이어 목록, 녹화 시각 등.
ReplayMetadata ReplayRecorder::BuildMetadata() const {
    ReplayMetadata meta;
    meta.match_id = match_id_;
    meta.recorded_at = start_time_;

    if (!frames_.empty()) {
        meta.duration_seconds =
            static_cast<int>((frames_.back().timestamp - frames_.front().timestamp) / 1000);

        // 참여 플레이어 수집 (std::set으로 중복 제거)
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

// [Order 4] Compress - 프레임 데이터 직렬화
// [LEARN] 간단한 바이너리 직렬화. 실제로는:
//         - Protobuf/FlatBuffers: 구조화된 직렬화
//         - zstd/lz4: 고속 압축
std::vector<uint8_t> ReplayRecorder::Compress(const std::vector<ReplayFrame>& frames) {
    // 간단한 직렬화 (실제로는 protobuf/flatbuffers + zstd 압축 사용)
    std::vector<uint8_t> data;

    // 헤더: 프레임 수
    uint32_t frame_count = static_cast<uint32_t>(frames.size());
    data.resize(sizeof(uint32_t));
    std::memcpy(data.data(), &frame_count, sizeof(uint32_t));

    // 각 프레임 직렬화
    for (const auto& frame : frames) {
        // tick, timestamp (int64_t 2개)
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

// [Order 5] ExtractPOVFrames - 특정 플레이어 POV 프레임 추출
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
