cmake_minimum_required(VERSION 3.24)
project(LI_SAT_solver)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O3)

add_executable(LI_SAT_solver main.cpp)
