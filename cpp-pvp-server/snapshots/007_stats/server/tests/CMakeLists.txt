enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(pvpserver_tests
    unit/test_combat.cpp
    unit/test_config.cpp
    unit/test_game_loop.cpp
    unit/test_game_session.cpp
    unit/test_leaderboard_store.cpp
    unit/test_match_queue.cpp
    unit/test_match_stats.cpp
    unit/test_matchmaker.cpp
    unit/test_player_profile_service.cpp
    unit/test_projectile.cpp
)

target_link_libraries(pvpserver_tests
    PRIVATE
        pvpserver_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pvpserver_tests)
