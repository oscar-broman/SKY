cmake_minimum_required(VERSION 3.10)
project(SKY)

set(PLUGIN_VERSION "2.0.0")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/builds)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/lib/cmake-modules")
include_directories(${PROJECT_SOURCE_DIR}/lib)

## 
# Check specifically if being compiled using 
# MXE and MiniGW
##
message("MXE=${MXE}")

if(UNIX)
	# force 32bit compilation and hide non-exported symbols
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector -O3 -g -fPIC -w -fpermissive -m32 -fvisibility=hidden")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector -O3 -g -fPIC -w -fpermissive -m32 -fvisibility=hidden")
	set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
elseif(MXE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpsapi")
	set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -lpsapi")
	add_definitions(-D_MXE)
endif()

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(src)
