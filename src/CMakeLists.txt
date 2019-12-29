include(AMXConfig)
include(AddSAMPPlugin)

set(SAMPSDK_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMPSDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
find_package(SAMPSDK REQUIRED)
include_directories(${SAMPSDK_DIR})
include_directories(${SAMPSDK_DIR}/amx)

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

install(TARGETS SKY DESTINATION "./")

set(CPACK_PACKAGE_VERSION ${PLUGIN_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

if(WIN32)
	set(CPACK_GENERATOR ZIP)
	set(CPACK_PACKAGE_FILE_NAME "release-windows")
else()
	set(CPACK_GENERATOR TGZ)
	set(CPACK_PACKAGE_FILE_NAME "release-linux")
endif()

include(CPack)