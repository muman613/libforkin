# Set the project name
project (libforkin_utils)

# Add a library with the above sources
add_library(${PROJECT_NAME} src/fork_utils.cpp src/process_utils.cpp src/server_utils.cpp include/client_utils.h src/client_utils.cpp include/popen2.h src/popen2.cpp)
add_library(sub::libforkin ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)