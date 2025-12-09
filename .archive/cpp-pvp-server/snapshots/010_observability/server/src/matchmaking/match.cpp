#include "pvpserver/matchmaking/match.h"

namespace pvpserver {

Match::Match(std::string match_id, std::vector<std::string> players, int average_elo,
             std::chrono::steady_clock::time_point created_at, std::string region)
    : match_id_(std::move(match_id)),
      players_(std::move(players)),
      average_elo_(average_elo),
      created_at_(created_at),
      region_(std::move(region)) {}

}  // namespace pvpserver
