add_library(utilities INTERFACE)

set_property(TARGET utilities PROPERTY CXX_STANDARD 20)

target_include_directories(utilities INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	$<INSTALL_INTERFACE:include>)

install(TARGETS utilities)