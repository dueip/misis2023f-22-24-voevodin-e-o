add_library(loader loader.cpp loader.hpp )

set_property(TARGET loader PROPERTY CXX_STANDARD 20)
find_package(OpenCV CONFIG REQUIRED)



if ((MSVC))
	target_compile_options(loader INTERFACE "/Zc:__cplusplus")
endif()
#find_package(DCMTK CONFIG REQUIRED)
#trget_link_libraries(loader PRIVATE DCMTK::DCMTK)


add_subdirectory(Utilities)
add_subdirectory(Options)
# I link opencv here since it'll be moved a bit later?? Probably..
target_link_libraries(loader PRIVATE ${OpenCV_LIBS})
target_link_libraries(loader PRIVATE options)

target_include_directories(loader PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
	$<INSTALL_INTERFACE:include>)

install(TARGETS loader)
