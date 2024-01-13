#TODO: maybe unite local tests with unit tests later?
# For now it'll work, but I really don't want to push 1gb of data to github or anywhere else, 
# espeically if it's just mock data.
# IDEA: maybe make each test create files worth 1 gb and then delete them?
# Though that might really make tests long...
# Caching data after 1st test maybe?

project(local_tests)

find_package(doctest CONFIG REQUIRED)

add_definitions(-DCMAKE_LOCAL_TEST_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/\")

add_executable(local_tests local_test.cpp)

target_link_libraries(local_tests doctest::doctest loader)