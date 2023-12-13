cmake_minimum_required(VERSION 3.8)

option(BUILD_ONLY_LIBRARY "Only build loader" on)

if (BUILD_ONLY_LIBRARY)
	add_subdirectory(prj.cw/src/)
else()
	add_subdirectory(prj.codeforces)
	add_subdirectory(prj.cw)
endif()

