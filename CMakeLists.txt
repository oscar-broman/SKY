###
# PROJECT SETUP
###

cmake_minimum_required(VERSION 3.20)
project(SKY)
set(PLUGIN_VERSION "2.3.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

###
# BUILD OUTPUT
###

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/components)
set(CMAKE_INSTALL_PREFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/cmake-modules")
include_directories(${PROJECT_SOURCE_DIR}/lib)

if(MSVC)
	foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
		string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
	endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()

###
# COMPILER FLAGS
###

if(UNIX)
	# force 32bit compilation and hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
	add_link_options(
		"-static-libgcc"
		"-static-libstdc++"
	)
elseif(WIN32)
	add_definitions(
		-D_CRT_SECURE_NO_WARNINGS
		-DWIN32_LEAN_AND_MEAN
	)
endif()

###
# DIRECTORIES
###

add_subdirectory(lib/omp-sdk)
add_subdirectory(src)
