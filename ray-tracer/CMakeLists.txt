cmake_minimum_required(VERSION 3.20)
project(RayTracer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
add_compile_options(-Wall -Wextra -Wpedantic)

# Main executable
add_executable(raytracer
    src/main.cpp
)

target_include_directories(raytracer PRIVATE src)

# Enable testing
enable_testing()

# Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(unit_tests
    tests/unit/test_vec3.cpp
    tests/unit/test_ray.cpp
    tests/unit/test_sphere.cpp
    tests/unit/test_materials.cpp
    tests/unit/test_bvh.cpp
    tests/unit/test_textures.cpp
    tests/unit/test_geometry.cpp
    tests/unit/test_pdf.cpp
    tests/unit/test_onb.cpp
)

target_include_directories(unit_tests PRIVATE src)
target_link_libraries(unit_tests GTest::gtest_main)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(unit_tests)

# Benchmark executable
add_executable(benchmark_bvh
    tests/benchmark/benchmark_bvh.cpp
)

target_include_directories(benchmark_bvh PRIVATE src)
