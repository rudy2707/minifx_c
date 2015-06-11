cmake_minimum_required(VERSION 3.2)
project(minifx_c)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c99")

set(SOURCE_FILES main.c)
add_executable(minifx_c ${SOURCE_FILES})