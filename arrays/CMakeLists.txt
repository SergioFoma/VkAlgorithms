cmake_minimum_required(VERSION 3.11)

set(ALGORITHMS algorithms)

project(${ALGORITHMS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(INCLUDE ${PROJECT_SOUR_DIR})

# === algorithms ===
add_executable(${ALGORITHMS})
target_sources(${ALGORITHMS} PRIVATE main.cpp)
target_include_directories(${ALGORITHMS} PRIVATE ${INCLUDE})

# === tests ====
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/v1.18.x.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

set(ALGORITHM_TESTS algorithm_tests)
add_executable(${ALGORITHM_TESTS})
target_sources(${ALGORITHM_TESTS} PRIVATE tests.cpp)
target_include_directories(${ALGORITHM_TESTS} PRIVATE ${INCLUDE})
target_link_libraries(${ALGORITHM_TESTS} PRIVATE GTest::gtest_main)

gtest_discover_tests(${ALGORITHM_TESTS})

