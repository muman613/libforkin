cmake_minimum_required(VERSION 3.12)
project(forkin)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Werror)

add_executable(forkin main.cpp fork_utils.cpp fork_utils.h)