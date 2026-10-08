cmake_minimum_required(VERSION 3.11)

set(ALGORITHMS algorithms)

project(${ALGORITHMS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
set(INCLUDE ${PROJECT_SOUR_DIR})

# == download libs ===
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/v1.18.x.zip
)
FetchContent_Declare(
  googlebenchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.9.1.zip
)
FetchContent_MakeAvailable(googletest googlebenchmark)

# === algorithms ===
add_executable(${ALGORITHMS})
target_sources(${ALGORITHMS} PRIVATE main.cpp)
target_include_directories(${ALGORITHMS} PRIVATE ${INCLUDE})

# === tests ====
enable_testing()

include(GoogleTest)

set(ALGORITHM_TESTS algorithm_tests)
add_executable(${ALGORITHM_TESTS})
target_sources(${ALGORITHM_TESTS} PRIVATE tests.cpp)
target_include_directories(${ALGORITHM_TESTS} PRIVATE ${INCLUDE})
target_link_libraries(${ALGORITHM_TESTS} PRIVATE GTest::gtest_main)

gtest_discover_tests(${ALGORITHM_TESTS})

# === benchmark ===

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

set(BENCHMARK_TARGET benchmark_target)
add_executable(${BENCHMARK_TARGET})

target_sources(${BENCHMARK_TARGET} PRIVATE benchmark.cpp)
target_include_directories(${BENCHMARK_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${BENCHMARK_TARGET} PRIVATE
  benchmark::benchmark
  benchmark::benchmark_main
)


