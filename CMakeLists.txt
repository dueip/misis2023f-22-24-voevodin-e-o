cmake_minimum_required(VERSION 3.8)

option(BUILD_WITH_CODEFORCES "Build prj.codeforces targets" on)

if (BUILD_WITH_CODEFORCES)
  add_subdirectory(prj.codeforces)
endif()

add_subdirectory(prj.cw)