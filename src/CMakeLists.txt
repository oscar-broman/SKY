include(AMXConfig)
include(AddSAMPPlugin)

set(SAMPSDK_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMPSDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SAMP_SDK_ROOT "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
find_package(SAMPSDK REQUIRED)
include_directories(${SAMPSDK_DIR})
include_directories(${SAMPSDK_DIR}/amx)
include_directories(${PROJECT_SOURCE_DIR}/lib/omp-sdk/include)

add_subdirectory(Shared)
include_directories(Shared)

add_samp_plugin(SKY
	${SAMPSDK_DIR}/amxplugin.cpp
	${SAMPSDK_DIR}/amxplugin2.cpp
	${SAMPSDK_DIR}/amx/getch.c
	main.cpp
	network.cpp
	player.cpp
	scripting.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE OMP-SDK)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "./")
set(CPACK_PACKAGE_VERSION ${PLUGIN_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
set(CPACK_PACKAGE_FILE_NAME "sky-opm-${PLUGIN_VERSION}")
install(FILES ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.inc DESTINATION ".")
install(TARGETS ${PROJECT_NAME} DESTINATION "./")

if(WIN32)
        set(CPACK_GENERATOR ZIP)
else()
        set(CPACK_GENERATOR TGZ)
endif()

include(CPack)