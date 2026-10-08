cmake_minimum_required(VERSION 3.11)

set(ALGORITHMS algorithms)

project(${ALGORITHMS})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(INCLUDE ${PROJECT_SOUR_DIR})

add_executable(${ALGORITHMS})
target_sources(${ALGORITHMS} PRIVATE main.cpp algorithms.cpp)
target_include_directories(${ALGORITHMS} PRIVATE ${INCLUDE})
