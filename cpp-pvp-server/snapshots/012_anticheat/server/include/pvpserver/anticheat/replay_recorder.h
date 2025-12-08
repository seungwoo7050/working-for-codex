#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "pvpserver/anticheat/hit_validator.h"  // Vec3, PlayerState

namespace pvpserver::anticheat {

// 게임 이벤트 타입
enum class GameEventType {
    SHOT_FIRED,
    HIT_REGISTERED,
    PLAYER_KILLED,
    PLAYER_SPAWNED,
    ITEM_USED,
    MATCH_START,
    MATCH_END
};

// 게임 이벤트
struct GameEvent {
    GameEventType type;
    int64_t timestamp;
    std::string player_id;
    std::string target_id;  // 해당되는 경우
    Vec3 position;
    std::string extra_data;  // JSON 형식 추가 데이터
};

// 투사체 상태
struct ProjectileState {
    std::string projectile_id;
    std::string owner_id;
    Vec3 position;
    Vec3 velocity;
    bool is_active;
};

// 리플레이 프레임
struct ReplayFrame {
    int64_t tick;
    int64_t timestamp;
    std::vector<PlayerState> players;
    std::vector<ProjectileState> projectiles;
    std::vector<GameEvent> events;
};

// 리플레이 메타데이터
struct ReplayMetadata {
    std::string match_id;
    int64_t recorded_at;
    int duration_seconds;
    std::vector<std::string> players;
    size_t file_size;
    std::string game_mode;
    std::string map_name;
};

// 리플레이 레코더
class ReplayRecorder {
public:
    ReplayRecorder();
    ~ReplayRecorder();

    // 녹화 제어
    void StartRecording(const std::string& match_id);
    void RecordFrame(const ReplayFrame& frame);
    void StopRecording();

    // 상태 조회
    bool IsRecording() const { return recording_; }
    const std::string& GetMatchId() const { return match_id_; }
    size_t GetFrameCount() const { return frames_.size(); }

    // 데이터 추출
    std::vector<uint8_t> GetCompressedReplay();
    std::vector<uint8_t> GetPlayerPOV(const std::string& player_id);

    // 원시 프레임 접근
    const std::vector<ReplayFrame>& GetFrames() const { return frames_; }

    // 메타데이터 생성
    ReplayMetadata BuildMetadata() const;

private:
    std::string match_id_;
    std::vector<ReplayFrame> frames_;
    bool recording_ = false;
    int64_t start_time_ = 0;

    // 압축
    std::vector<uint8_t> Compress(const std::vector<ReplayFrame>& frames);

    // 특정 플레이어 시점 추출
    std::vector<ReplayFrame> ExtractPOVFrames(const std::string& player_id);
};

// 리플레이 저장소
class ReplayStorage {
public:
    virtual ~ReplayStorage() = default;

    // CRUD
    virtual void SaveReplay(const std::string& match_id, const std::vector<uint8_t>& data) = 0;
    virtual std::optional<std::vector<uint8_t>> LoadReplay(const std::string& match_id) = 0;
    virtual void DeleteReplay(const std::string& match_id) = 0;

    // 메타데이터
    virtual void SaveMetadata(const std::string& match_id, const ReplayMetadata& metadata) = 0;
    virtual std::optional<ReplayMetadata> GetMetadata(const std::string& match_id) = 0;
    virtual std::vector<ReplayMetadata> GetReplaysByPlayer(const std::string& player_id) = 0;

    // 정리
    virtual void CleanupOldReplays(int64_t older_than_timestamp) = 0;
};

// 인메모리 리플레이 저장소 (테스트용)
class InMemoryReplayStorage : public ReplayStorage {
public:
    void SaveReplay(const std::string& match_id, const std::vector<uint8_t>& data) override;
    std::optional<std::vector<uint8_t>> LoadReplay(const std::string& match_id) override;
    void DeleteReplay(const std::string& match_id) override;

    void SaveMetadata(const std::string& match_id, const ReplayMetadata& metadata) override;
    std::optional<ReplayMetadata> GetMetadata(const std::string& match_id) override;
    std::vector<ReplayMetadata> GetReplaysByPlayer(const std::string& player_id) override;

    void CleanupOldReplays(int64_t older_than_timestamp) override;

    // 테스트용
    size_t GetReplayCount() const { return replays_.size(); }

private:
    std::unordered_map<std::string, std::vector<uint8_t>> replays_;
    std::unordered_map<std::string, ReplayMetadata> metadata_;
};

}  // namespace pvpserver::anticheat
