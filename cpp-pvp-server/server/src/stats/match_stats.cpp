// [FILE]
// - 목적: 매치 통계 수집 (경기 결과 기록)
// - 주요 역할: 매치 종료 시 플레이어별 통계 집계
// - 관련 클론 가이드 단계: [v1.3.0] 통계 시스템
// - 권장 읽는 순서: PlayerMatchStats → MatchStatsCollector::Collect
//
// [LEARN] 매치 통계 항목:
//         - shots_fired / hits_landed: 정확도 계산용
//         - kills / deaths: K/D 계산용
//         - damage_dealt / damage_taken: MVP 선정용

#include "pvpserver/stats/match_stats.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace pvpserver {

namespace {
// [LEARN] 임시 집계용 구조체 (익명 namespace = 파일 스코프)
struct RunningTotals {
    std::string player_id;
    std::uint32_t shots_fired{0};
    std::uint32_t hits_landed{0};
    std::uint32_t kills{0};
    std::uint32_t deaths{0};
    std::uint64_t damage_dealt{0};
    std::uint64_t damage_taken{0};
};
}  // namespace

// [Order 1] PlayerMatchStats 생성자
// [LEARN] 불변 객체 패턴: 생성 시 모든 필드 설정, 이후 변경 불가
PlayerMatchStats::PlayerMatchStats(std::string match_id, std::string player_id,
                                   std::uint32_t shots_fired, std::uint32_t hits_landed,
                                   std::uint32_t kills, std::uint32_t deaths,
                                   std::uint64_t damage_dealt, std::uint64_t damage_taken)
    : match_id_(std::move(match_id)),
      player_id_(std::move(player_id)),
      shots_fired_(shots_fired),
      hits_landed_(hits_landed),
      kills_(kills),
      deaths_(deaths),
      damage_dealt_(damage_dealt),
      damage_taken_(damage_taken) {}

// Accuracy - 정확도 계산
// [LEARN] 0으로 나누기 방지 패턴
double PlayerMatchStats::Accuracy() const noexcept {
    if (shots_fired_ == 0) {
        return 0.0;
    }
    return static_cast<double>(hits_landed_) / static_cast<double>(shots_fired_);
}

// [Order 2] MatchResult 생성자
MatchResult::MatchResult(std::string match_id, std::string winner_id, std::string loser_id,
                         std::chrono::system_clock::time_point completed_at,
                         std::vector<PlayerMatchStats> player_stats)
    : match_id_(std::move(match_id)),
      winner_id_(std::move(winner_id)),
      loser_id_(std::move(loser_id)),
      completed_at_(completed_at),
      player_stats_(std::move(player_stats)) {}

// [Order 3] Collect - 매치 종료 시 통계 수집
// [LEARN] 게임 세션 + 전투 로그에서 플레이어별 통계 집계
MatchResult MatchStatsCollector::Collect(const CombatEvent& death_event, const GameSession& session,
                                         std::chrono::system_clock::time_point completed_at) const {
    auto states = session.Snapshot();
    auto log = session.CombatLogSnapshot();

    // 플레이어별 집계 맵
    std::unordered_map<std::string, RunningTotals> totals;
    totals.reserve(states.size());
    for (const auto& state : states) {
        RunningTotals entry;
        entry.player_id = state.player_id;
        entry.shots_fired = static_cast<std::uint32_t>(state.shots_fired);
        entry.hits_landed = static_cast<std::uint32_t>(state.hits_landed);
        entry.deaths = static_cast<std::uint32_t>(state.deaths);
        totals.emplace(entry.player_id, entry);
    }

    // 엔트리 없으면 생성하는 헬퍼 람다
    const auto ensure_entry = [&totals](const std::string& player_id) -> RunningTotals& {
        auto it = totals.find(player_id);
        if (it == totals.end()) {
            RunningTotals entry;
            entry.player_id = player_id;
            it = totals.emplace(player_id, entry).first;
        }
        return it->second;
    };

    // 전투 로그에서 킬/데미지 집계
    for (const auto& event : log) {
        if (event.tick > death_event.tick) {
            continue;  // 종료 시점 이후 이벤트 무시
        }
        if (event.type == CombatEventType::Hit) {
            auto& shooter = ensure_entry(event.shooter_id);
            shooter.damage_dealt += static_cast<std::uint64_t>(event.damage);
            auto& target = ensure_entry(event.target_id);
            target.damage_taken += static_cast<std::uint64_t>(event.damage);
        } else if (event.type == CombatEventType::Death) {
            auto& shooter = ensure_entry(event.shooter_id);
            ++shooter.kills;
            auto& target = ensure_entry(event.target_id);
            if (target.deaths == 0) {
                target.deaths = 1;
            }
        }
    }

    auto& winner_totals = ensure_entry(death_event.shooter_id);
    auto& loser_totals = ensure_entry(death_event.target_id);
    if (winner_totals.kills == 0) {
        winner_totals.kills = 1;
    }
    if (loser_totals.deaths == 0) {
        loser_totals.deaths = 1;
    }

    std::ostringstream id_stream;
    id_stream << "match-" << death_event.tick << '-' << death_event.shooter_id << "-vs-"
              << death_event.target_id;
    const std::string match_id = id_stream.str();

    std::vector<PlayerMatchStats> stats;
    stats.reserve(totals.size());
    for (auto& kv : totals) {
        auto& entry = kv.second;
        stats.emplace_back(match_id, entry.player_id, entry.shots_fired, entry.hits_landed,
                           entry.kills, entry.deaths, entry.damage_dealt, entry.damage_taken);
    }
    std::sort(stats.begin(), stats.end(),
              [](const PlayerMatchStats& lhs, const PlayerMatchStats& rhs) {
                  return lhs.player_id() < rhs.player_id();
              });

    std::cout << "match complete " << match_id << " winner=" << death_event.shooter_id
              << " loser=" << death_event.target_id << std::endl;

    return MatchResult(match_id, death_event.shooter_id, death_event.target_id, completed_at,
                       std::move(stats));
}

}  // namespace pvpserver
