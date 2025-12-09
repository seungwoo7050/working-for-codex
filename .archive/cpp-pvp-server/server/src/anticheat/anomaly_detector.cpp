// [FILE]
// - 목적: 통계적 이상 감지 (에임봇/봇 감지)
// - 주요 역할: Z-Score 기반 통계 분석으로 비정상적 플레이 패턴 탐지
// - 관련 클론 가이드 단계: [CG-02.10] 안티치트 시스템
// - 권장 읽는 순서: CombatStats → AnomalyDetector::Analyze → SuspicionSystem
//
// [LEARN] 에임봇은 "너무 정확함"으로 드러남.
//         - 정확도가 통계적 평균보다 비정상적으로 높음
//         - 헤드샷 비율이 인간 한계 초과
//         - 반응 시간이 비현실적으로 빠름
//         - 일관성이 너무 높음 (봇 = 기계적 동작)
//
// Z-Score: (값 - 평균) / 표준편차
//          |z| > 3이면 약 0.3% 확률 = 매우 비정상

#include "pvpserver/anticheat/anomaly_detector.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace pvpserver::anticheat {

// [Order 1] CombatStats - 전투 통계 수집
// - 샷, 히트, 헤드샷, 킬/데스, 반응 시간 기록
// [LEARN] 충분한 샘플이 있어야 통계적 판단 가능 (최소 10샷 이상)
float CombatStats::AvgReactionTime() const {
    if (reaction_times.empty()) return 0.0f;
    // std::accumulate: 컨테이너 합계 계산 (C의 for 루프 대체)
    float sum = std::accumulate(reaction_times.begin(), reaction_times.end(), 0.0f);
    return sum / static_cast<float>(reaction_times.size());
}

// 반응 시간 표준편차 계산
// [LEARN] 표준편차가 너무 낮으면 봇 의심 (인간은 자연스러운 변동 있음)
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
    // 슬라이딩 윈도우: 최근 100개만 유지 (메모리 제한)
    if (reaction_times.size() > 100) {
        reaction_times.erase(reaction_times.begin());
    }
}

void CombatStats::ResetStreak() { current_streak = 0; }

// AnomalyScore - 이상 요인 중 가장 높은 것 반환
std::string AnomalyScore::GetHighestFactor() const {
    if (accuracy >= headshot && accuracy >= reaction && accuracy >= consistency) return "accuracy";
    if (headshot >= reaction && headshot >= consistency) return "headshot";
    if (reaction >= consistency) return "reaction";
    return "consistency";
}

// [Order 2] AnomalyDetector::Analyze - 통계적 이상 분석
// [LEARN] 각 지표를 Z-Score로 변환 → 이상 점수로 정규화
AnomalyScore AnomalyDetector::Analyze(const CombatStats& stats) {
    AnomalyScore score;

    // 최소 샘플 필요 (너무 적으면 통계적 의미 없음)
    if (stats.total_shots < 10) {
        return score;  // 0점 반환
    }

    // [지표 1] 정확도 분석
    float accuracy_zscore = CalculateZScore(stats.Accuracy(), global_.avg_accuracy, global_.std_accuracy);
    score.accuracy = ZScoreToAnomaly(accuracy_zscore);

    // [지표 2] 헤드샷 비율 분석
    float headshot_zscore = CalculateZScore(stats.HeadshotRatio(), global_.avg_headshot, global_.std_headshot);
    score.headshot = ZScoreToAnomaly(headshot_zscore);

    // [지표 3] 반응 시간 분석 (낮을수록 의심)
    if (!stats.reaction_times.empty()) {
        float avg_reaction = stats.AvgReactionTime();
        // 반응 시간은 낮을수록 이상하므로 역방향 z-score
        float reaction_zscore = CalculateZScore(global_.avg_reaction - avg_reaction + global_.avg_reaction,
                                                global_.avg_reaction, global_.std_reaction);
        score.reaction = ZScoreToAnomaly(reaction_zscore);
    }

    // [지표 4] 일관성 분석 (너무 일정하면 봇 의심)
    if (stats.reaction_times.size() >= 5) {
        float std_reaction = stats.StdReactionTime();
        // 표준편차가 너무 낮으면 = 기계적 = 봇
        float consistency_zscore = CalculateZScore(global_.avg_consistency - std_reaction + global_.avg_consistency,
                                                   global_.avg_consistency, global_.std_consistency);
        score.consistency = ZScoreToAnomaly(consistency_zscore);
    }

    return score;
}

void AnomalyDetector::UpdateGlobalStats(const GlobalStats& stats) { global_ = stats; }

void AnomalyDetector::SetZScoreThreshold(float threshold) { zscore_threshold_ = threshold; }

// [Order 3] CalculateZScore - Z-Score 계산
// [LEARN] Z = (X - μ) / σ
//         - |Z| > 2: 약 5% 확률 (의심)
//         - |Z| > 3: 약 0.3% 확률 (강력 의심)
float AnomalyDetector::CalculateZScore(float value, float mean, float stddev) {
    if (stddev < 0.0001f) return 0.0f;  // 0으로 나누기 방지
    return (value - mean) / stddev;
}

// ZScoreToAnomaly - Z-Score를 0~1 이상 점수로 변환
float AnomalyDetector::ZScoreToAnomaly(float zscore) {
    // Z-score를 0~1로 정규화
    // |z| >= threshold -> 1.0 (최대 의심)
    // |z| = 0 -> 0.0 (정상)
    float abs_z = std::abs(zscore);
    if (abs_z >= zscore_threshold_) return 1.0f;
    return abs_z / zscore_threshold_;
}

// [Order 4] SuspicionSystem - 의심 레벨 관리
// [LEARN] 위반 누적 → 의심 레벨 상승 → 밴 대상 선정
void SuspicionSystem::RecordViolation(const std::string& player_id, const Violation& v) {
    auto& suspicion = suspicions_[player_id];
    suspicion.violations.push_back(v);  // 이력 저장
    suspicion.total_score += v.severity;  // 심각도 누적
    suspicion.level = CalculateLevel(suspicion.total_score);  // 레벨 재계산
}

// 이상 점수로 의심 레벨 업데이트
void SuspicionSystem::UpdateAnomalyScore(const std::string& player_id, const AnomalyScore& score) {
    auto& suspicion = suspicions_[player_id];
    float combined = score.Combined();  // 모든 요인 합산
    suspicion.total_score = std::max(suspicion.total_score, combined);
    suspicion.level = CalculateLevel(suspicion.total_score);
}

// 플레이어 의심 레벨 조회
SuspicionLevel SuspicionSystem::GetLevel(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.level : SuspicionLevel::NONE;
}

// 위반 이력 조회 (관리자 대시보드용)
std::vector<Violation> SuspicionSystem::GetHistory(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.violations : std::vector<Violation>{};
}

float SuspicionSystem::GetTotalScore(const std::string& player_id) const {
    auto it = suspicions_.find(player_id);
    return it != suspicions_.end() ? it->second.total_score : 0.0f;
}

// 임계값 설정 (게임별로 튜닝 필요)
void SuspicionSystem::SetLevelThresholds(float low, float medium, float high, float critical) {
    threshold_low_ = low;
    threshold_medium_ = medium;
    threshold_high_ = high;
    threshold_critical_ = critical;
}

void SuspicionSystem::RemovePlayer(const std::string& player_id) { suspicions_.erase(player_id); }

void SuspicionSystem::ClearAll() { suspicions_.clear(); }

// 특정 레벨 이상 플레이어 목록 (관리자용)
std::vector<std::string> SuspicionSystem::GetPlayersAtLevel(SuspicionLevel min_level) const {
    std::vector<std::string> result;
    for (const auto& [id, suspicion] : suspicions_) {
        if (static_cast<int>(suspicion.level) >= static_cast<int>(min_level)) {
            result.push_back(id);
        }
    }
    return result;
}

// [Order 5] CalculateLevel - 점수 → 레벨 변환
// [LEARN] 임계값 기반 레벨링
//         - NONE: 정상
//         - LOW: 모니터링
//         - MEDIUM: 경고
//         - HIGH: 조사 필요
//         - CRITICAL: 즉시 밴
SuspicionLevel SuspicionSystem::CalculateLevel(float score) const {
    if (score >= threshold_critical_) return SuspicionLevel::CRITICAL;
    if (score >= threshold_high_) return SuspicionLevel::HIGH;
    if (score >= threshold_medium_) return SuspicionLevel::MEDIUM;
    if (score >= threshold_low_) return SuspicionLevel::LOW;
    return SuspicionLevel::NONE;
}

// ========================================================================
// [Reader Notes] 통계적 이상 감지 (Anomaly Detection)
// ========================================================================
// 1. Z-Score 기반 분석
//    - Z = (X - μ) / σ  (값 - 평균) / 표준편차
//    - |Z| > 2: 상위/하위 2.5% = 의심
//    - |Z| > 3: 상위/하위 0.15% = 강력 의심
//
// 2. 탐지 지표
//    - 정확도: 너무 높으면 에임봇
//    - 헤드샷 비율: 인간 한계 초과
//    - 반응 시간: 너무 빠르면 비인간적
//    - 일관성: 표준편차 낮으면 봇
//
// 3. 글로벌 통계 (GlobalStats)
//    - 전체 플레이어 평균/표준편차
//    - 주기적으로 업데이트 (일간/주간)
//    - 게임 밸런스 변경 시 재계산 필요
//
// 4. SuspicionLevel 단계
//    - NONE: 정상 플레이어
//    - LOW: 모니터링 대상
//    - MEDIUM: 경고 발송
//    - HIGH: 인간 검토 필요
//    - CRITICAL: 자동 밴 또는 격리
//
// 5. C++ 활용
//    - std::accumulate: 합계 계산
//    - std::unordered_map: 플레이어별 상태 관리
//    - auto& [id, suspicion]: 구조화된 바인딩 (C++17)
//
// [CG-02.10] 안티치트 - 통계적 이상 탐지로 에임봇/봇 감지
// ========================================================================

}  // namespace pvpserver::anticheat
