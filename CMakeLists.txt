cmake_minimum_required(VERSION 2.8)

project(RegularExpression)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra")

aux_source_directory(. DIR_SRCS)

add_executable(RegularExpression ${DIR_SRCS})
