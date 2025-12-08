#pragma once

#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

namespace pvpserver::anticheat {

// 전투 통계
struct CombatStats {
    // 기본 통계
    int total_shots = 0;
    int hits = 0;
    int headshots = 0;
    int kills = 0;
    int deaths = 0;

    // 반응 시간 (적 발견 → 사격, ms)
    std::vector<float> reaction_times;

    // 연속 킬
    int current_streak = 0;
    int max_kill_streak = 0;

    // 계산 메서드
    float Accuracy() const { return total_shots > 0 ? static_cast<float>(hits) / total_shots : 0.0f; }

    float HeadshotRatio() const {
        return hits > 0 ? static_cast<float>(headshots) / hits : 0.0f;
    }

    float AvgReactionTime() const;
    float StdReactionTime() const;

    // 통계 업데이트
    void RecordShot(bool hit, bool headshot = false);
    void RecordKill();
    void RecordDeath();
    void RecordReactionTime(float ms);
    void ResetStreak();
};

// 이상치 점수
struct AnomalyScore {
    float accuracy = 0.0f;      // 0.0 ~ 1.0
    float headshot = 0.0f;      // 0.0 ~ 1.0
    float reaction = 0.0f;      // 0.0 ~ 1.0
    float consistency = 0.0f;   // 0.0 ~ 1.0 (너무 일정하면 봇 의심)

    float Combined() const {
        return accuracy * 0.3f + headshot * 0.3f + reaction * 0.2f + consistency * 0.2f;
    }

    std::string GetHighestFactor() const;
};

// 의심 레벨
enum class SuspicionLevel {
    NONE = 0,
    LOW = 1,       // 주의 관찰
    MEDIUM = 2,    // 상세 모니터링
    HIGH = 3,      // 리플레이 자동 저장
    CRITICAL = 4   // 자동 밴 후보
};

// 위반 기록
struct Violation {
    std::string type;
    float severity;
    int64_t timestamp;
    std::string details;
};

// 글로벌 통계 (기준치)
struct GlobalStats {
    float avg_accuracy = 0.25f;
    float std_accuracy = 0.1f;
    float avg_headshot = 0.15f;
    float std_headshot = 0.08f;
    float avg_reaction = 250.0f;  // ms
    float std_reaction = 100.0f;
    float avg_consistency = 50.0f;  // 반응시간 표준편차
    float std_consistency = 30.0f;
};

// 이상치 탐지기
class AnomalyDetector {
public:
    // 분석
    AnomalyScore Analyze(const CombatStats& stats);

    // 글로벌 통계 업데이트
    void UpdateGlobalStats(const GlobalStats& stats);

    // Z-Score 임계값 설정
    void SetZScoreThreshold(float threshold);
    float GetZScoreThreshold() const { return zscore_threshold_; }

private:
    GlobalStats global_;
    float zscore_threshold_ = 3.0f;  // 기본 3 시그마

    // Z-Score 계산
    float CalculateZScore(float value, float mean, float stddev);

    // Z-Score를 0~1 이상치 점수로 변환
    float ZScoreToAnomaly(float zscore);
};

// 의심 시스템
class SuspicionSystem {
public:
    // 위반 기록
    void RecordViolation(const std::string& player_id, const Violation& v);

    // 이상치 점수 업데이트
    void UpdateAnomalyScore(const std::string& player_id, const AnomalyScore& score);

    // 조회
    SuspicionLevel GetLevel(const std::string& player_id) const;
    std::vector<Violation> GetHistory(const std::string& player_id) const;
    float GetTotalScore(const std::string& player_id) const;

    // 임계값 설정
    void SetLevelThresholds(float low, float medium, float high, float critical);

    // 플레이어 정리
    void RemovePlayer(const std::string& player_id);
    void ClearAll();

    // 감시 대상 조회 (특정 레벨 이상)
    std::vector<std::string> GetPlayersAtLevel(SuspicionLevel min_level) const;

private:
    struct PlayerSuspicion {
        float total_score = 0.0f;
        std::vector<Violation> violations;
        SuspicionLevel level = SuspicionLevel::NONE;
    };

    std::unordered_map<std::string, PlayerSuspicion> suspicions_;

    // 임계값
    float threshold_low_ = 0.3f;
    float threshold_medium_ = 0.5f;
    float threshold_high_ = 0.7f;
    float threshold_critical_ = 0.9f;

    SuspicionLevel CalculateLevel(float score) const;
};

}  // namespace pvpserver::anticheat
