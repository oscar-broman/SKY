include(AMXConfig)
include(AddSAMPPlugin)

set(SAMPSDK_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMPSDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
find_package(SAMPSDK REQUIRED)
include_directories(${SAMPSDK_DIR})
include_directories(${SAMPSDK_DIR}/amx)

if(MXE)
	find_library(PSAPI Psapi)
endif()

file(GLOB PLUGIN_SOURCE_FILES
	${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_samp_plugin(SKY
	${SAMPSDK_DIR}/amxplugin.cpp
	${SAMPSDK_DIR}/amxplugin2.cpp
	${SAMPSDK_DIR}/amx/getch.c
	${PROJECT_SOURCE_DIR}/lib/raknet/BitStream.cpp
	${PLUGIN_SOURCE_FILES}
	${PROJECT_SOURCE_DIR}/SKY.def
)

target_link_libraries(SKY subhook)

if(MXE)
	target_link_libraries(SKY -lpsapi)
endif()

set_property(TARGET SKY PROPERTY CXX_STANDARD 11)
set_property(TARGET SKY PROPERTY CXX_STANDARD_REQUIRED ON)