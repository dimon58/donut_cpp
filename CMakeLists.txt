cmake_minimum_required(VERSION 3.23)
project(donut_cpp)

set(CMAKE_CXX_STANDARD 23)

add_executable(donut_cpp main.cpp terminal.hpp draw.hpp fps.hpp donut.hpp)
