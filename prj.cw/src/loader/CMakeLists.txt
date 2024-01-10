add_library(loader loader.cpp loader.hpp )

find_package(OpenCV CONFIG REQUIRED)

add_subdirectory(Utilities)
add_subdirectory(Options)
# I link opencv here since it'll be moved a bit later?? Probably..
target_link_libraries(loader PRIVATE ${OpenCV_LIBS})
target_link_libraries(loader PRIVATE options)

target_include_directories(loader PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	$<INSTALL_INTERFACE:include>)

install(TARGETS loader)
