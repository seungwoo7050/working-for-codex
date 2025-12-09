#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <csignal>
#include <iostream>
#include <memory>

#include "pvpserver/core/config.h"
#include "pvpserver/core/game_loop.h"
#include "pvpserver/game/game_session.h"
#include "pvpserver/network/websocket_server.h"
#include "pvpserver/storage/postgres_storage.h"

int main() {
    using namespace pvpserver;

    const auto config = GameConfig::FromEnv();
    std::cout << "PvP Server v1.0.0 starting on port " << config.port() << std::endl;

    GameSession session(config.tick_rate());
    GameLoop loop(config.tick_rate());
    PostgresStorage storage(config.database_dsn());
    if (!storage.Connect()) {
        std::cerr << "Failed to connect to Postgres at startup; continuing in degraded mode."
                  << std::endl;
    }

    boost::asio::io_context io_context;
    auto server = std::make_shared<WebSocketServer>(io_context, config.port(), session, loop);
    server->SetLifecycleHandlers(
        [&](const std::string& player_id) {
            if (!storage.RecordSessionEvent(player_id, "start")) {
                std::cerr << "Failed to record session start for " << player_id << std::endl;
            }
        },
        [&](const std::string& player_id) {
            if (!storage.RecordSessionEvent(player_id, "end")) {
                std::cerr << "Failed to record session end for " << player_id << std::endl;
            }
        });

    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](const boost::system::error_code& /*ec*/, int /*signal*/) {
        std::cout << "Signal received. Shutting down." << std::endl;
        server->Stop();
        loop.Stop();
        io_context.stop();
    });

    server->Start();
    loop.Start();

    io_context.run();

    loop.Stop();
    loop.Join();

    std::cout << "PvP Server stopped" << std::endl;
    return 0;
}
