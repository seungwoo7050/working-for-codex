#include "pvpserver/anticheat/anomaly_detector.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace pvpserver::anticheat {

// CombatStats 구현
float CombatStats::AvgReactionTime() const {
    if (reaction_times.empty()) return 0.0f;
    float sum = std::accumulate(reaction_times.begin(), reaction_times.end(), 0.0f);
    return sum / static_cast<float>(reaction_times.size());
}

float CombatStats::StdReactionTime() const {
    if (reaction_times.size() < 2) return 0.0f;
    float mean = AvgReactionTime();
    float sum_sq = 0.0f;
    for (float t : reaction_times) {
        sum_sq += (t - mean) * (t - mean);
    }
    return std::sqrt(sum_sq / static_cast<float>(reaction_times.size() - 1));
}

void CombatStats::RecordShot(bool hit, bool headshot) {
    total_shots++;
    if (hit) {
        hits++;
        if (headshot) {
            headshots++;
        }
    }
}

void CombatStats::RecordKill() {
    kills++;
    current_streak++;
    if (current_streak > max_kill_streak) {
        max_kill_streak = current_streak;
    }
}

void CombatStats::RecordDeath() {
    deaths++;
    ResetStreak();
}

void CombatStats::RecordReactionTime(float ms) {
    reaction_times.push_back(ms);
    // 최근 100개만 유지
    if (reaction_times.size() > 100) {
        reaction_times.erase(reaction_times.begin());
    }
}

void CombatStats::ResetStreak() { current_streak = 0; }

// AnomalyScore 구현
std::string AnomalyScore::GetHighestFactor() const {
    if (accuracy >= headshot && accuracy >= reaction && accuracy >= consistency) return "accuracy";
    if (headshot >= reaction && headshot >= consistency) return "headshot";
    if (reaction >= consistency) return "reaction";
    return "consistency";
}

// AnomalyDetector 구현
AnomalyScore AnomalyDetector::Analyze(const CombatStats& stats) {
    AnomalyScore score;

    // 최소 샘플 필요
    if (stats.total_shots < 10) {
        return score;  // 0점 반환
    }

    // 정확도 분석
    float accuracy_zscore = CalculateZScore(stats.Accuracy(), global_.avg_accuracy, global_.std_accuracy);
    score.accuracy = ZScoreToAnomaly(accuracy_zscore);

    // 헤드샷 비율 분석
    float headshot_zscore = CalculateZScore(stats.HeadshotRatio(), global_.avg_headshot, global_.std_headshot);
    score.headshot = ZScoreToAnomaly(headshot_zscore);

    // 반응 시간 분석 (낮을수록 의심)
    if (!stats.reaction_times.empty()) {
        float avg_reaction = stats.AvgReactionTime();
        // 반응 시간은 낮을수록 이상하므로 음수 z-score
        float reaction_zscore = CalculateZScore(global_.avg_reaction - avg_reaction + global_.avg_reaction,
                                                global_.avg_reaction, global_.std_reaction);
        score.reaction = ZScoreToAnomaly(reaction_zscore);
    }

    // 일관성 분석 (너무 일정하면 봇 의심)
    if (stats.reaction_times.size() >= 5) {
        float std_reaction = stats.StdReactionTime();
        // 표준편차가 너무 낮으면 의심
        float consistency_zscore = CalculateZScore(global_.avg_consistency - std_reaction + global_.avg_consistency,
                                                   global_.avg_consistency, global_.std_consistency);
        score.consistency = ZScoreToAnomaly(consistency_zscore);
    }

    return score;
}

void AnomalyDetector::UpdateGlobalStats(const GlobalStats& stats) { global_ = stats; }

void AnomalyDetector::SetZScoreThreshold(float threshold) { zscore_threshold_ = threshold; }

float AnomalyDetector::CalculateZScore(float value, float mean, float stddev) {
    if (stddev < 0.0001f) return 0.0f;
    return (value - mean) / stddev;
}

float AnomalyDetector::ZScoreToAnomaly(float zscore) {
    // Z-score를 0~1로 변환
    // |z| >= threshold -> 1.0
    // |z| = 0 -> 0.0
    float abs_z = std::abs(zscore);
    if (abs_z >= zscore_threshold_) return 1.0f;
    return abs_z / zscore_threshold_;
}

// SuspicionSystem 구현
void SuspicionSystem::RecordViolation(const std::string& player_id, const Violation& v) {
    auto& suspicion = suspicions_[player_id];
    suspicion.violations.push_back(v);
    suspicion.total_score += v.severity;
    suspicion.level = CalculateLevel(suspicion.total_score);
}

void SuspicionSystem::UpdateAnomalyScore(const std::string& player_id, const AnomalyScore& score) {
    auto& suspicion = suspicions_[player_id];
    float combined = score.Combined();
    suspicion.total_score = std::max(suspicion.total_score, combined);
    suspicion.level = CalculateLevel(suspicion.total_score);
}

SuspicionLevel SuspicionSystem::GetLevel(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.level : SuspicionLevel::NONE;
}

std::vector<Violation> SuspicionSystem::GetHistory(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.violations : std::vector<Violation>{};
}

float SuspicionSystem::GetTotalScore(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.total_score : 0.0f;
}

void SuspicionSystem::SetLevelThresholds(float low, float medium, float high, float critical) {
    threshold_low_ = low;
    threshold_medium_ = medium;
    threshold_high_ = high;
    threshold_critical_ = critical;
}

void SuspicionSystem::RemovePlayer(const std::string& player_id) { suspicions_.erase(player_id); }

void SuspicionSystem::ClearAll() { suspicions_.clear(); }

std::vector<std::string> SuspicionSystem::GetPlayersAtLevel(SuspicionLevel min_level) const {
    std::vector<std::string> result;
    for (const auto& [id, suspicion] : suspicions_) {
        if (static_cast<int>(suspicion.level) >= static_cast<int>(min_level)) {
            result.push_back(id);
        }
    }
    return result;
}

SuspicionLevel SuspicionSystem::CalculateLevel(float score) const {
    if (score >= threshold_critical_) return SuspicionLevel::CRITICAL;
    if (score >= threshold_high_) return SuspicionLevel::HIGH;
    if (score >= threshold_medium_) return SuspicionLevel::MEDIUM;
    if (score >= threshold_low_) return SuspicionLevel::LOW;
    return SuspicionLevel::NONE;
}

}  // namespace pvpserver::anticheat
