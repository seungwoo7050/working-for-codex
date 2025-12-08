#include <gtest/gtest.h>

#include "pvpserver/anticheat/anomaly_detector.h"

using namespace pvpserver::anticheat;

class AnomalyDetectorTest : public ::testing::Test {
protected:
    AnomalyDetector detector_;
    SuspicionSystem suspicion_;
};

// CombatStats 테스트
TEST_F(AnomalyDetectorTest, CombatStatsAccuracy) {
    CombatStats stats;
    stats.RecordShot(true);
    stats.RecordShot(true);
    stats.RecordShot(false);
    stats.RecordShot(false);

    EXPECT_FLOAT_EQ(stats.Accuracy(), 0.5f);
}

TEST_F(AnomalyDetectorTest, CombatStatsHeadshotRatio) {
    CombatStats stats;
    stats.RecordShot(true, true);   // 히트 + 헤드샷
    stats.RecordShot(true, false);  // 히트
    stats.RecordShot(true, false);  // 히트
    stats.RecordShot(true, true);   // 히트 + 헤드샷

    EXPECT_FLOAT_EQ(stats.HeadshotRatio(), 0.5f);
}

TEST_F(AnomalyDetectorTest, CombatStatsKillStreak) {
    CombatStats stats;
    stats.RecordKill();
    stats.RecordKill();
    stats.RecordKill();
    EXPECT_EQ(stats.current_streak, 3);
    EXPECT_EQ(stats.max_kill_streak, 3);

    stats.RecordDeath();
    EXPECT_EQ(stats.current_streak, 0);
    EXPECT_EQ(stats.max_kill_streak, 3);

    stats.RecordKill();
    EXPECT_EQ(stats.current_streak, 1);
}

TEST_F(AnomalyDetectorTest, CombatStatsReactionTime) {
    CombatStats stats;
    stats.RecordReactionTime(100.0f);
    stats.RecordReactionTime(200.0f);
    stats.RecordReactionTime(300.0f);

    EXPECT_FLOAT_EQ(stats.AvgReactionTime(), 200.0f);
    EXPECT_GT(stats.StdReactionTime(), 0.0f);
}

TEST_F(AnomalyDetectorTest, CombatStatsEmpty) {
    CombatStats stats;

    EXPECT_FLOAT_EQ(stats.Accuracy(), 0.0f);
    EXPECT_FLOAT_EQ(stats.HeadshotRatio(), 0.0f);
    EXPECT_FLOAT_EQ(stats.AvgReactionTime(), 0.0f);
    EXPECT_FLOAT_EQ(stats.StdReactionTime(), 0.0f);
}

// AnomalyDetector 테스트
TEST_F(AnomalyDetectorTest, AnalyzeNormalPlayer) {
    CombatStats stats;
    stats.total_shots = 100;
    stats.hits = 25;       // 25% 정확도 (평균)
    stats.headshots = 4;   // 16% 헤드샷 (평균 근처)

    for (int i = 0; i < 20; ++i) {
        stats.RecordReactionTime(250.0f + (i % 10 - 5) * 10.0f);  // 200~300ms 변동
    }

    AnomalyScore score = detector_.Analyze(stats);

    // 정상 플레이어는 낮은 점수
    EXPECT_LT(score.Combined(), 0.3f);
}

TEST_F(AnomalyDetectorTest, AnalyzeAimbot) {
    CombatStats stats;
    stats.total_shots = 100;
    stats.hits = 95;       // 95% 정확도 (매우 높음)
    stats.headshots = 80;  // 84% 헤드샷 (매우 높음)

    for (int i = 0; i < 20; ++i) {
        stats.RecordReactionTime(50.0f);  // 일정한 50ms
    }

    AnomalyScore score = detector_.Analyze(stats);

    // 에임봇 의심 플레이어는 높은 점수
    EXPECT_GT(score.accuracy, 0.8f);
    EXPECT_GT(score.headshot, 0.8f);
}

TEST_F(AnomalyDetectorTest, AnalyzeInsufficientData) {
    CombatStats stats;
    stats.total_shots = 5;  // 최소 10발 필요

    AnomalyScore score = detector_.Analyze(stats);

    EXPECT_FLOAT_EQ(score.Combined(), 0.0f);
}

TEST_F(AnomalyDetectorTest, ZScoreThresholdSetting) {
    detector_.SetZScoreThreshold(2.0f);
    EXPECT_FLOAT_EQ(detector_.GetZScoreThreshold(), 2.0f);
}

// SuspicionSystem 테스트
TEST_F(AnomalyDetectorTest, SuspicionRecordViolation) {
    Violation v;
    v.type = "speedhack";
    v.severity = 0.5f;
    v.timestamp = 1000;

    suspicion_.RecordViolation("player1", v);

    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::MEDIUM);
    EXPECT_FLOAT_EQ(suspicion_.GetTotalScore("player1"), 0.5f);
}

TEST_F(AnomalyDetectorTest, SuspicionLevelProgression) {
    suspicion_.SetLevelThresholds(0.3f, 0.5f, 0.7f, 0.9f);

    Violation v;
    v.type = "test";
    v.severity = 0.2f;

    suspicion_.RecordViolation("player1", v);
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::NONE);

    v.severity = 0.2f;
    suspicion_.RecordViolation("player1", v);
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::LOW);

    v.severity = 0.2f;
    suspicion_.RecordViolation("player1", v);
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::MEDIUM);

    v.severity = 0.2f;
    suspicion_.RecordViolation("player1", v);
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::HIGH);

    v.severity = 0.2f;
    suspicion_.RecordViolation("player1", v);
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::CRITICAL);
}

TEST_F(AnomalyDetectorTest, SuspicionHistory) {
    Violation v1{"speedhack", 0.3f, 1000, "details1"};
    Violation v2{"aimbot", 0.4f, 2000, "details2"};

    suspicion_.RecordViolation("player1", v1);
    suspicion_.RecordViolation("player1", v2);

    auto history = suspicion_.GetHistory("player1");
    EXPECT_EQ(history.size(), 2);
    EXPECT_EQ(history[0].type, "speedhack");
    EXPECT_EQ(history[1].type, "aimbot");
}

TEST_F(AnomalyDetectorTest, SuspicionUpdateAnomalyScore) {
    AnomalyScore score;
    score.accuracy = 0.8f;
    score.headshot = 0.7f;
    score.reaction = 0.6f;
    score.consistency = 0.5f;

    suspicion_.UpdateAnomalyScore("player1", score);

    float expected = 0.8f * 0.3f + 0.7f * 0.3f + 0.6f * 0.2f + 0.5f * 0.2f;
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::MEDIUM);
}

TEST_F(AnomalyDetectorTest, SuspicionGetPlayersAtLevel) {
    suspicion_.SetLevelThresholds(0.3f, 0.5f, 0.7f, 0.9f);

    Violation high_v{"cheat", 0.8f, 1000, ""};
    Violation low_v{"minor", 0.2f, 1000, ""};

    suspicion_.RecordViolation("cheater", high_v);
    suspicion_.RecordViolation("normal", low_v);

    auto high_suspicion = suspicion_.GetPlayersAtLevel(SuspicionLevel::HIGH);
    EXPECT_EQ(high_suspicion.size(), 1);
    EXPECT_EQ(high_suspicion[0], "cheater");
}

TEST_F(AnomalyDetectorTest, SuspicionClearAndRemove) {
    Violation v{"test", 0.5f, 1000, ""};
    suspicion_.RecordViolation("player1", v);
    suspicion_.RecordViolation("player2", v);

    suspicion_.RemovePlayer("player1");
    EXPECT_EQ(suspicion_.GetLevel("player1"), SuspicionLevel::NONE);
    EXPECT_NE(suspicion_.GetLevel("player2"), SuspicionLevel::NONE);

    suspicion_.ClearAll();
    EXPECT_EQ(suspicion_.GetLevel("player2"), SuspicionLevel::NONE);
}

TEST_F(AnomalyDetectorTest, AnomalyScoreHighestFactor) {
    AnomalyScore score;
    score.accuracy = 0.9f;
    score.headshot = 0.5f;
    score.reaction = 0.3f;
    score.consistency = 0.2f;

    EXPECT_EQ(score.GetHighestFactor(), "accuracy");

    score.headshot = 0.95f;
    EXPECT_EQ(score.GetHighestFactor(), "headshot");
}
