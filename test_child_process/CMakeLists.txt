project(test_child_process)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall -Werror)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} sub::libforkin cap)

