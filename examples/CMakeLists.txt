cmake_minimum_required(VERSION 3.27)
project(aaf_examples C)

set(CMAKE_C_STANDARD 17)

add_executable(window window.c)
target_include_directories(window PRIVATE ../include)
target_link_libraries(window PRIVATE aaf)

# Copy assets to the build directory
file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})