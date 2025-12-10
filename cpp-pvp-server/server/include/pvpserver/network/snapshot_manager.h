#pragma once

#include <array>
#include <cstdint>
#include <mutex>
#include <optional>
#include <vector>

#include "pvpserver/game/player_state.h"
#include "pvpserver/game/projectile.h"
#include "pvpserver/network/packet_types.h"

namespace pvpserver {

/**
 * @brief 게임 상태 스냅샷
 */
struct Snapshot {
    std::uint32_t sequence{0};
    std::uint64_t timestamp{0};
    std::vector<PlayerState> players;
    std::vector<ProjectileSnapshot> projectiles;
    
    /**
     * @brief 스냅샷 직렬화
     */
    std::vector<std::uint8_t> Serialize() const;
    
    /**
     * @brief 스냅샷 역직렬화
     */
    static Snapshot Deserialize(const std::vector<std::uint8_t>& data);
    
    /**
     * @brief 직렬화된 크기 예측
     */
    std::size_t EstimatedSize() const;
};

/**
 * @brief 델타 (변경분)
 */
struct Delta {
    std::uint32_t base_sequence{0};
    std::uint32_t target_sequence{0};
    std::vector<std::uint8_t> changes;
    
    /**
     * @brief 델타 직렬화
     */
    std::vector<std::uint8_t> Serialize() const;
    
    /**
     * @brief 델타 역직렬화
     */
    static Delta Deserialize(const std::vector<std::uint8_t>& data);
};

/**
 * @brief 스냅샷 매니저
 * 
 * 게임 상태 스냅샷을 관리하고 델타를 계산합니다.
 */
class SnapshotManager {
   public:
    static constexpr std::size_t BUFFER_SIZE = 64;  // 약 1초 (60 TPS)
    
    SnapshotManager() = default;
    
    /**
     * @brief 현재 게임 상태로 스냅샷 생성
     * @param players 현재 플레이어 상태
     * @param projectiles 현재 발사체 상태
     * @return 생성된 스냅샷
     */
    Snapshot CreateSnapshot(
        const std::vector<PlayerState>& players,
        const std::vector<Projectile>& projectiles
    );
    
    /**
     * @brief 스냅샷 저장
     * @param snapshot 저장할 스냅샷
     */
    void SaveSnapshot(const Snapshot& snapshot);
    
    /**
     * @brief 특정 시퀀스의 스냅샷 조회
     * @param sequence 시퀀스 번호
     * @return 스냅샷 (없으면 nullopt)
     */
    std::optional<Snapshot> GetSnapshot(std::uint32_t sequence) const;
    
    /**
     * @brief 가장 최근 스냅샷 조회
     */
    std::optional<Snapshot> GetLatestSnapshot() const;
    
    /**
     * @brief 특정 시간대의 스냅샷 조회 (보간 포함)
     * @param timestamp 타임스탬프 (밀리초)
     * @return 보간된 스냅샷
     */
    std::optional<Snapshot> GetSnapshotAt(std::uint64_t timestamp) const;
    
    /**
     * @brief 두 스냅샷 간 델타 계산
     * @param base_seq 기준 시퀀스
     * @param target_seq 대상 시퀀스
     * @return 델타 (실패 시 nullopt)
     */
    std::optional<Delta> CalculateDelta(
        std::uint32_t base_seq,
        std::uint32_t target_seq
    ) const;
    
    /**
     * @brief 델타 적용
     * @param base 기준 스냅샷
     * @param delta 적용할 델타
     * @return 결과 스냅샷
     */
    static Snapshot ApplyDelta(const Snapshot& base, const Delta& delta);
    
    /**
     * @brief 현재 시퀀스 번호
     */
    std::uint32_t CurrentSequence() const { return current_sequence_; }
    
    /**
     * @brief 버퍼에 저장된 스냅샷 수
     */
    std::size_t BufferedCount() const;
    
   private:
    // 버퍼에서 인덱스 찾기
    std::optional<std::size_t> FindIndex(std::uint32_t sequence) const;
    
    // 두 플레이어 상태 비교하여 변경 플래그 생성
    static std::uint8_t ComparePlayerStates(
        const PlayerState& base,
        const PlayerState& target
    );
    
    mutable std::mutex mutex_;
    std::array<Snapshot, BUFFER_SIZE> buffer_;
    std::uint32_t current_sequence_{0};
    std::size_t head_{0};
    std::size_t count_{0};
};

}  // namespace pvpserver
