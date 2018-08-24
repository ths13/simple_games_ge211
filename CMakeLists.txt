cmake_minimum_required(VERSION 3.3)
project(c_snake CXX)

add_subdirectory(3rdparty/ge211 EXCLUDE_FROM_ALL)

add_executable(c_snake c_snake.cpp)
target_link_libraries(c_snake ge211)
set_property(TARGET c_snake PROPERTY CXX_STANDARD 14)
set_property(TARGET c_snake PROPERTY CXX_STANDARD_REQUIRED On)