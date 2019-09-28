cmake_minimum_required(VERSION 2.8)

project(RegularExpression)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")

include_directories(src/headers test/headers)

file(GLOB SOURCES "src/sources/*.cpp")
file(GLOB TESTS "test/sources/*.cpp")

add_executable(RegularExpression main.cpp ${SOURCES} ${TESTS})

SET(CMAKE_BUILD_TYPE "Debug") 
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
