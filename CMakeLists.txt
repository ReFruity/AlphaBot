cmake_minimum_required(VERSION 3.3)
project(breakthrough)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(breakthrough ${SOURCE_FILES})

set_target_properties(breakthrough PROPERTIES OUTPUT_NAME AlphaBot)