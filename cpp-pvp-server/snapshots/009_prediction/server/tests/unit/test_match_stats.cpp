#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <stdexcept>

#include "pvpserver/game/game_session.h"
#include "pvpserver/stats/match_stats.h"

namespace {
using pvpserver::MatchResult;
using pvpserver::MatchStatsCollector;
using pvpserver::PlayerMatchStats;
}  // namespace

TEST(MatchStatsCollectorTest, ProducesAccurateStatsFromCombatLog) {
    pvpserver::GameSession session(60.0);
    session.UpsertPlayer("attacker");
    session.UpsertPlayer("defender");

    pvpserver::MovementInput position;
    position.sequence = 1;
    position.right = true;
    position.mouse_x = 1.0;
    session.ApplyInput("defender", position, 0.08);

    auto fire = [&](std::uint64_t sequence) {
        pvpserver::MovementInput input;
        input.sequence = sequence;
        input.mouse_x = 1.0;
        input.fire = true;
        session.ApplyInput("attacker", input, 1.0 / 60.0);
    };

    std::uint64_t tick = 0;
    for (int shot = 0; shot < 5; ++shot) {
        fire(static_cast<std::uint64_t>(shot + 2));
        for (int i = 0; i < 10; ++i) {
            session.Tick(++tick, 1.0 / 60.0);
        }
    }

    const auto deaths = session.ConsumeDeathEvents();
    ASSERT_EQ(1u, deaths.size());
    const auto completion_time = std::chrono::system_clock::now();

    MatchStatsCollector collector;
    MatchResult result = collector.Collect(deaths.front(), session, completion_time);

    EXPECT_EQ("attacker", result.winner_id());
    EXPECT_EQ("defender", result.loser_id());
    EXPECT_EQ(completion_time, result.completed_at());
    EXPECT_FALSE(result.match_id().empty());

    const auto& stats = result.player_stats();
    ASSERT_EQ(2u, stats.size());

    auto find_stats = [&](const std::string& id) -> const PlayerMatchStats& {
        const auto it =
            std::find_if(stats.begin(), stats.end(),
                         [&](const PlayerMatchStats& entry) { return entry.player_id() == id; });
        if (it == stats.end()) {
            throw std::runtime_error("player stats not found");
        }
        return *it;
    };

    const auto& attacker_stats = find_stats("attacker");
    EXPECT_GE(attacker_stats.shots_fired(), 5u);
    EXPECT_EQ(attacker_stats.hits_landed(), 5u);
    EXPECT_EQ(attacker_stats.damage_dealt(), 100u);
    EXPECT_EQ(attacker_stats.kills(), 1u);
    EXPECT_EQ(attacker_stats.deaths(), 0u);
    EXPECT_DOUBLE_EQ(1.0, attacker_stats.Accuracy());

    const auto& defender_stats = find_stats("defender");
    EXPECT_EQ(defender_stats.damage_taken(), 100u);
    EXPECT_EQ(defender_stats.deaths(), 1u);
    EXPECT_EQ(defender_stats.kills(), 0u);
    EXPECT_EQ(defender_stats.shots_fired(), 0u);
    EXPECT_DOUBLE_EQ(0.0, defender_stats.Accuracy());
}
