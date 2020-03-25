cmake_minimum_required(VERSION 2.8)
project(dmcom C)

set(DMCOM_MAJOR_VERSION 1)
set(DMCOM_MINOR_VERSION 0)
set(DMCOM_PATCH_VERSION 0)

set(DMCOM_VERSION ${DMCOM_MAJOR_VERSION}.${DMCOM_MINOR_VERSION}.${DMCOM_PATCH_VERSION})
set(DMCOM_PLATFORM ${CMAKE_SYSTEM_NAME})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include/dmcom CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR lib/cmake/dmcom CACHE PATH "Installation directory for CMake file")
set(INSTALL_DOC_DIR share/dmcom CACHE PATH "Installation directory for document")
set(INSTALL_CONFIG_DIR etc/dmcom CACHE PATH "Installation directory for config file")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -O2 -Wall -fexceptions -fstack-protector -Werror=implicit-function-declaration -Werror=format-security -D_FORTIFY_SOURCE=2")

find_package(cmocka REQUIRED)

file(GLOB dmcom_SRC
	"dmcom/src/dmtimer.c"
)
list(SORT dmcom_SRC)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/dmcom
	${CMAKE_CURRENT_SOURCE_DIR}/dmcom/src
)

add_library(dmcom SHARED ${dmcom_SRC})
set_target_properties(dmcom PROPERTIES VERSION ${DMCOM_VERSION} SOVERSION ${DMCOM_MAJOR_VERSION})
target_link_libraries(dmcom PRIVATE) # ${CMAKE_THREAD_LIBS_INIT})

#add_executable(dmcom_utest ${dmcom_utest_SRC})
#target_link_libraries(dmcom_utest ${CMOCKA_LIBRARIES})

#install(
#	TARGETS dmcom dmcom 
#	EXPORT dmcomTargets
#	LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT bin 
#	ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT bin 
#)

foreach(p LIB BIN INCLUDE CMAKE)
	set(var INSTALL_${p}_DIR)
	if(NOT IS_ABSOLUTE "${${var}}")
		set(ABSOLUTE_${var} "${CMAKE_INSTALL_PREFIX}/${${var}}") # Add all targets to the build-tree export set
	endif()
endforeach()

# Create the dmcomConfig.cmake and dmcomConfigVersion.cmake files
file(RELATIVE_PATH REL_INCLUDE_DIR "${ABSOLUTE_INSTALL_CMAKE_DIR}" "${ABSOLUTE_INSTALL_INCLUDE_DIR}/..")
# ... for the install tree
configure_file(dmcomConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dmcomConfig.cmake" @ONLY)
configure_file(dmcomConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/dmcomConfigVersion.cmake" @ONLY)

# Install the dmcomConfig.cmake and dmcomConfigVersion.cmake
install(
	FILES
	"${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/dmcomConfig.cmake"
	"${PROJECT_BINARY_DIR}/dmcomConfigVersion.cmake"
	DESTINATION "${INSTALL_CMAKE_DIR}"
	COMPONENT dev
)

# Install the export set for use with the install-tree
#install(
#	EXPORT dmcomTargets
#	DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev
#)

add_custom_target(uninstall
	COMMAND xargs rm < ${CMAKE_BINARY_DIR}/install_manifest.txt
)

#Add test
enable_testing()
add_custom_target(build_tests)
#add_dependencies(build_tests dmcom)
set(CMAKE_CTEST_COMMAND ctest -V)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
add_dependencies(check build_tests)
add_subdirectory(utest EXCLUDE_FROM_ALL)
