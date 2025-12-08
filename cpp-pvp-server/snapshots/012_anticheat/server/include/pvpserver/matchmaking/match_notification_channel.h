#pragma once

#include <mutex>
#include <optional>
#include <queue>
#include <vector>

#include "pvpserver/matchmaking/match.h"

namespace pvpserver {

class MatchNotificationChannel {
   public:
    void Publish(const Match& match);
    std::optional<Match> Poll();
    std::vector<Match> Drain();

   private:
    std::mutex mutex_;
    std::queue<Match> queue_;
};

}  // namespace pvpserver
