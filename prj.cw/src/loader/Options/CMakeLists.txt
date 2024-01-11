add_library(options Options.cpp Options.hpp)


target_link_libraries(options utilities)
target_include_directories(options PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	$<INSTALL_INTERFACE:include>)

install(TARGETS options)