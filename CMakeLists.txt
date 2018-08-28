cmake_minimum_required(VERSION 3.3)
project(simple_games_ge211 CXX)

add_subdirectory(3rdparty/ge211 EXCLUDE_FROM_ALL)

add_executable(snake snake.cpp)
target_link_libraries(snake ge211)
set_property(TARGET snake PROPERTY CXX_STANDARD 14)
set_property(TARGET snake PROPERTY CXX_STANDARD_REQUIRED On)