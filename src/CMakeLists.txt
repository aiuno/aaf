cmake_minimum_required(VERSION 3.27)
project(aaf C)

set(CMAKE_C_STANDARD 17)

include_directories(../include)

add_library(aaf aaf.c gui.c gui_text_input.c gui_label.c gui_button.c)
find_package(raylib CONFIG REQUIRED)
target_link_libraries(aaf PRIVATE raylib)