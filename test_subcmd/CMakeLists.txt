project(test_subcmd)

add_subdirectory(subcmd)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} sub::libforkin cap)

