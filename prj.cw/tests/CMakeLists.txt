project(unit_tests)

find_package(doctest CONFIG REQUIRED)

add_definitions(-DCMAKE_TEST_PATH=\"${CMAKE_SOURCE_DIR}/test_sources/\")

add_executable(unit_tests main_test.cpp)

target_link_libraries(unit_tests doctest::doctest loader)

add_executable(program_test program_test.cpp)
target_link_libraries(program_test PRIVATE cxxopts::cxxopts loader)
install(TARGETS program_test DESTINATION bin)


	#add_executable(unit_tests ${sources_tests} ${sources})
#	target_compile_definitions(unit_tests PUBLIC UNIT_TESTS)
	# set_target_properties(loading_project PROPERTIES ENABLE_EXPORTS on)

