enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(pvpserver_tests
    unit/test_config.cpp
    unit/test_game_loop.cpp
    unit/test_game_session.cpp
)

target_link_libraries(pvpserver_tests
    PRIVATE
        pvpserver_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(pvpserver_tests)
