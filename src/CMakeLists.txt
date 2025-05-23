cmake_minimum_required(VERSION 3.27)
project(aaf C)

set(CMAKE_C_STANDARD 17)

add_library(aaf aaf.c)
find_package(raylib CONFIG REQUIRED)
target_link_libraries(aaf PRIVATE raylib)