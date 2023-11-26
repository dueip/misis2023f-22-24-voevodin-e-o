add_library(loader loader.cpp loader.hpp)

target_include_directories(loader PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	$<INSTALL_INTERFACE:include>)

install(TARGETS loader)