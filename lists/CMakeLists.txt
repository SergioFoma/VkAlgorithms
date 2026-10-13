cmake_minimum_required(VERSION 3.11)

set(LIST list)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall -fsanitize=address")

set(INCLUDE ${CMAKE_CURRENT_SOURCE_DIR})

# === download libs ===
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/v1.18.x.zip
)

FetchContent_MakeAvailable(googletest)

# === list ===
add_executable(${LIST})
target_sources(${LIST} PRIVATE main.cpp algorithms.cpp)
target_include_directories(${LIST} PRIVATE ${INCLUDE})

# == tests ====
enable_testing()
include(GoogleTest)

# === list tests ====
set(LIST_TESTS list_tests)
add_executable(${LIST_TESTS})
target_sources(${LIST_TESTS} PRIVATE list_tests.cpp)
target_include_directories(${LIST_TESTS} PRIVATE ${INCLUDE})
target_link_libraries(${LIST_TESTS} PRIVATE GTest::gtest_main)

gtest_discover_tests(${LIST_TESTS})

# === algorithm tests ===
set(ALGORITHM_TESTS algorithm_tests)
add_executable(${ALGORITHM_TESTS})
target_sources(${ALGORITHM_TESTS} PRIVATE algorithms.cpp algorithm_tests.cpp)
target_include_directories(${ALGORITHM_TESTS} PRIVATE ${INCLUDE})
target_link_libraries(${ALGORITHM_TESTS} PRIVATE GTest::gtest_main)

gtest_discover_tests(${ALGORITHM_TESTS})

