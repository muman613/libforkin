cmake_minimum_required (VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(forkin)

add_subdirectory(libforkin_utils)
add_subdirectory(test_child_process)
