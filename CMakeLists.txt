cmake_minimum_required(VERSION 3.26)
project(ff)

set(CMAKE_CXX_STANDARD 20)

find_package(Curses REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_executable(ff main.cpp)

add_executable(ff_test test.cpp)

target_link_libraries(ff PRIVATE ${CURSES_LIBRARIES})
target_include_directories(ff PRIVATE ${CURSES_INCLUDE_DIR})
target_link_libraries(ff_test PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME TestCpp COMMAND ff_test)

