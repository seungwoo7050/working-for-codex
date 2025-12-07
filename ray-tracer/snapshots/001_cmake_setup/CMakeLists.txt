cmake_minimum_required(VERSION 3.20)
project(RayTracer VERSION 0.1 LANGUAGES CXX)

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
