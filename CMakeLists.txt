cmake_minimum_required(VERSION 3.19)
project(BMMOProtocol)

set(CMAKE_CXX_STANDARD 20)

include_directories(includes/common)
add_executable(BMMOProtocol main.cpp includes/BMMOProtocol.hpp)