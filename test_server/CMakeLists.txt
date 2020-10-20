project(test_server)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} sub::libforkin)

if (NOT ${CMAKE_HOST_APPLE})
    target_link_libraries(${PROJECT_NAME} cap)
endif ()