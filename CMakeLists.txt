cmake_minimum_required(VERSION 3.20)
project(NetSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB SOURCES "src/*.cpp")

add_executable(NetSim main.cpp ${SOURCES})

target_include_directories(NetSim PRIVATE include)