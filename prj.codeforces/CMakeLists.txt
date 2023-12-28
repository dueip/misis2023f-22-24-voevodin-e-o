cmake_minimum_required(VERSION 3.8)

project(codeforces)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(0236a 0236a.cpp)
add_executable(0118a 0118a.cpp)
add_executable(0058a 0058a.cpp)
add_executable(0455a 0455a.cpp)
add_executable(0282a 0282a.cpp)
add_executable(0110a 0110a.cpp)
add_executable(0469a 0469a.cpp)
add_executable(0004c 0004c.cpp)

add_executable(object_pool_test object_pool_test.cpp)
find_package(doctest)

target_link_libraries(object_pool_test doctest::doctest)