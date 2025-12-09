// [FILE]
// - 목적: 플레이어 프로필 서비스 (누적 통계 관리)
// - 주요 역할: ELO 레이팅 계산, 프로필 조회, 리더보드 연동
// - 관련 클론 가이드 단계: [v1.3.0] 통계 시스템, [v1.2.0] 매치메이킹
// - 권장 읽는 순서: EloRatingCalculator → RecordMatch → GetProfile
//
// [LEARN] ELO 레이팅 시스템:
//         - 기대 승률 = 1 / (1 + 10^((상대점수-내점수)/400))
//         - 새 점수 = 기존 점수 + K * (실제결과 - 기대승률)
//         - K=25: 변동폭 상수 (높을수록 변동 큼)

#include "pvpserver/stats/player_profile_service.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace pvpserver {

// [Order 1] EloRatingCalculator::Update - ELO 점수 계산
// [LEARN] 표준 ELO 공식:
//         - 기대 승률: E = 1 / (1 + 10^((상대-나)/400))
//         - 점수 변화: ΔR = K * (S - E)  (S=1:승, S=0:패)
EloRatingUpdate EloRatingCalculator::Update(int winner_rating, int loser_rating) const {
    // 기대 승률 계산
    const double expected_winner =
        1.0 / (1.0 + std::pow(10.0, (loser_rating - winner_rating) / 400.0));
    const double expected_loser =
        1.0 / (1.0 + std::pow(10.0, (winner_rating - loser_rating) / 400.0));
    
    constexpr double kFactor = 25.0;  // K 상수: 변동폭 결정
    
    // 새 레이팅 계산 (승자: S=1.0, 패자: S=0.0)
    const int winner_new =
        static_cast<int>(std::lround(winner_rating + kFactor * (1.0 - expected_winner)));
    const int loser_new =
        static_cast<int>(std::lround(loser_rating + kFactor * (0.0 - expected_loser)));
    return {winner_new, loser_new};
}

// PlayerProfile::Accuracy - 누적 정확도
double PlayerProfile::Accuracy() const noexcept {
    if (shots_fired == 0) {
        return 0.0;
    }
    return static_cast<double>(hits_landed) / static_cast<double>(shots_fired);
}

// [Order 2] PlayerProfileService 생성자
// [LEARN] 의존성 주입: 리더보드 스토어를 외부에서 주입
PlayerProfileService::PlayerProfileService(std::shared_ptr<LeaderboardStore> leaderboard_store)
    : leaderboard_(std::move(leaderboard_store)) {}

// [Order 3] RecordMatch - 매치 결과 기록
// [LEARN] 매치 종료 시 호출:
//         1. 플레이어별 통계 누적
//         2. ELO 레이팅 업데이트
//         3. 리더보드 갱신
void PlayerProfileService::RecordMatch(const MatchResult& result) {
    std::lock_guard<std::mutex> lk(mutex_);  // 멀티스레드 보호

    // 모든 플레이어 통계 누적
    for (const auto& stats : result.player_stats()) {
        auto& aggregate = aggregates_[stats.player_id()];
        aggregate.matches += 1;
        aggregate.shots_fired += stats.shots_fired();
        aggregate.hits_landed += stats.hits_landed();
        aggregate.damage_dealt += stats.damage_dealt();
        aggregate.damage_taken += stats.damage_taken();
        aggregate.kills += stats.kills();
        aggregate.deaths += stats.deaths();
    }

    // 승/패 기록
    auto& winner = aggregates_[result.winner_id()];
    auto& loser = aggregates_[result.loser_id()];
    winner.wins += 1;
    loser.losses += 1;

    // ELO 레이팅 업데이트
    const auto update = calculator_.Update(winner.rating, loser.rating);
    winner.rating = update.winner_new;
    loser.rating = update.loser_new;
    rating_updates_total_ += 2;

    // 리더보드 갱신 (있으면)
    if (leaderboard_) {
        leaderboard_->Upsert(result.winner_id(), winner.rating);
        leaderboard_->Upsert(result.loser_id(), loser.rating);
    }

    ++matches_recorded_total_;
}

// GetProfile - 플레이어 프로필 조회
std::optional<PlayerProfile> PlayerProfileService::GetProfile(const std::string& player_id) const {
    std::lock_guard<std::mutex> lk(mutex_);
    const auto it = aggregates_.find(player_id);
    if (it == aggregates_.end()) {
        return std::nullopt;  // 프로필 없음
    }
    return BuildProfileUnsafe(player_id, it->second);
}

// TopProfiles - 상위 플레이어 목록
std::vector<PlayerProfile> PlayerProfileService::TopProfiles(std::size_t limit) const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::vector<PlayerProfile> profiles;
    if (!leaderboard_) {
        // 리더보드 없으면 전체 스캔
        profiles.reserve(std::min(limit, aggregates_.size()));
        for (const auto& kv : aggregates_) {
            profiles.push_back(BuildProfileUnsafe(kv.first, kv.second));
        }
        std::sort(profiles.begin(), profiles.end(),
                  [](const PlayerProfile& lhs, const PlayerProfile& rhs) {
                      if (lhs.rating != rhs.rating) {
                          return lhs.rating > rhs.rating;
                      }
                      return lhs.player_id < rhs.player_id;
                  });
        if (profiles.size() > limit) {
            profiles.resize(limit);
        }
        return profiles;
    }

    const auto ordered = leaderboard_->TopN(limit);
    profiles.reserve(ordered.size());
    for (const auto& entry : ordered) {
        const auto it = aggregates_.find(entry.first);
        if (it != aggregates_.end()) {
            profiles.push_back(BuildProfileUnsafe(entry.first, it->second));
        }
    }
    return profiles;
}

std::string PlayerProfileService::SerializeProfile(const PlayerProfile& profile) const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"player_id\":\"" << profile.player_id << "\",";
    oss << "\"rating\":" << profile.rating << ",";
    oss << "\"matches\":" << profile.matches << ",";
    oss << "\"wins\":" << profile.wins << ",";
    oss << "\"losses\":" << profile.losses << ",";
    oss << "\"kills\":" << profile.kills << ",";
    oss << "\"deaths\":" << profile.deaths << ",";
    oss << "\"shots_fired\":" << profile.shots_fired << ",";
    oss << "\"hits_landed\":" << profile.hits_landed << ",";
    oss << "\"damage_dealt\":" << profile.damage_dealt << ",";
    oss << "\"damage_taken\":" << profile.damage_taken << ",";
    oss << "\"accuracy\":" << std::fixed << std::setprecision(4) << profile.Accuracy();
    oss << "}";
    return oss.str();
}

std::string PlayerProfileService::SerializeLeaderboard(
    const std::vector<PlayerProfile>& profiles) const {
    std::ostringstream oss;
    oss << "[";
    for (std::size_t i = 0; i < profiles.size(); ++i) {
        if (i > 0) {
            oss << ",";
        }
        oss << SerializeProfile(profiles[i]);
    }
    oss << "]";
    return oss.str();
}

std::string PlayerProfileService::MetricsSnapshot() const {
    std::lock_guard<std::mutex> lk(mutex_);
    std::ostringstream oss;
    oss << "# TYPE player_profiles_total gauge\n";
    oss << "player_profiles_total " << aggregates_.size() << "\n";
    const std::size_t leaderboard_size = leaderboard_ ? leaderboard_->Size() : aggregates_.size();
    oss << "# TYPE leaderboard_entries_total gauge\n";
    oss << "leaderboard_entries_total " << leaderboard_size << "\n";
    oss << "# TYPE matches_recorded_total counter\n";
    oss << "matches_recorded_total " << matches_recorded_total_ << "\n";
    oss << "# TYPE rating_updates_total counter\n";
    oss << "rating_updates_total " << rating_updates_total_ << "\n";
    return oss.str();
}

PlayerProfile PlayerProfileService::BuildProfileUnsafe(const std::string& player_id,
                                                       const AggregateStats& stats) const {
    PlayerProfile profile;
    profile.player_id = player_id;
    profile.rating = stats.rating;
    profile.matches = stats.matches;
    profile.wins = stats.wins;
    profile.losses = stats.losses;
    profile.kills = stats.kills;
    profile.deaths = stats.deaths;
    profile.shots_fired = stats.shots_fired;
    profile.hits_landed = stats.hits_landed;
    profile.damage_dealt = stats.damage_dealt;
    profile.damage_taken = stats.damage_taken;
    return profile;
}

}  // namespace pvpserver
