add_library(pvpserver_performance_tests INTERFACE)

add_executable(udp_load_test
    udp_load_test.cpp
)

target_include_directories(udp_load_test
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(udp_load_test
    PRIVATE
        pvpserver_lib
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME udp_load_test COMMAND udp_load_test)
