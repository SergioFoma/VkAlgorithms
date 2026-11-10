cmake_minimum_required(VERSION 3.11)

project(trees)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === download utilities ====
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  FIND_PACKAGE_ARGS NAMES GTest CONFIG
)
FetchContent_MakeAvailable(googletest)

add_executable(trees)
target_compile_options(trees PRIVATE -Wall -O2)
target_sources(trees PRIVATE main.cpp)
target_include_directories(trees PRIVATE ./)

include(GoogleTest)

add_executable(unit_tests)
target_compile_options(trees PRIVATE -Wall -O2)
target_sources(unit_tests PRIVATE unit_tests.cpp)
target_include_directories(unit_tests PRIVATE ./)
target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

enable_testing()
gtest_discover_tests(unit_tests)
