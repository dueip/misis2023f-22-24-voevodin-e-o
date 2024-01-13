cmake_minimum_required(VERSION 3.8)
project(loading_project)

option(BUILD_WITH_TESTS "Build tests" on)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

add_subdirectory(tests)
