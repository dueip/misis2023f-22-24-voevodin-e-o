cmake_minimum_required(VERSION 3.8)
project(loading_project)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file (GLOB_RECURSE sources src/*.cpp include/*.hpp)
file (GLOB_RECURSE sources_tests tests/*.cpp)

add_library(loading_project ${sources})

target_compile_options(loading_project PUBLIC "-W3")

target_include_directories(loading_project PUBLIC src/ include/)


find_package(doctest CONFIG)

if (${doctest_FOUND})
	add_executable(unit_tests ${sources_tests} ${sources})
	target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)
	set_target_properties(loading_project PROPERTIES ENABLE_EXPORTS on)
	target_link_libraries(unit_tests doctest::doctest loading_project)
	
endif()
