add_library(pvpserver_lib
    core/config.cpp
    core/game_loop.cpp
    game/combat.cpp
    game/game_session.cpp
    game/projectile.cpp
    matchmaking/match.cpp
    matchmaking/match_notification_channel.cpp
    matchmaking/match_queue.cpp
    matchmaking/match_request.cpp
    matchmaking/matchmaker.cpp
    network/websocket_server.cpp
    storage/postgres_storage.cpp
)

target_link_libraries(pvpserver_lib
    PUBLIC
        Boost::system
        libpq::pq
)

add_executable(pvpserver
    main.cpp
)

target_link_libraries(pvpserver
    PRIVATE
        pvpserver_lib
        Boost::system
        libpq::pq
)
