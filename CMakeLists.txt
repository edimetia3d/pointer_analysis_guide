cmake_minimum_required(VERSION 3.27)
project(PointAnalysis)

set(CMAKE_CXX_STANDARD 17)

add_executable(CIPointAnalysis main.cpp)
add_executable(CSPointAnalysis cs_main.cc)
