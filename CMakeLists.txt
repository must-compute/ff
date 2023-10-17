cmake_minimum_required(VERSION 3.26)
project(ff)

set(CMAKE_CXX_STANDARD 20)

find_package(Curses REQUIRED)

add_executable(ff main.cpp single_include/tui/tui.hpp)

target_link_libraries(ff PRIVATE ${CURSES_LIBRARIES})
target_include_directories(ff PRIVATE ${CURSES_INCLUDE_DIR})
