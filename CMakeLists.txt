cmake_minimum_required(VERSION 3.20)

project(rceditor)

# Define RC_EDITOR_ROOT_DIR pointing to project root directory
get_filename_component(RC_EDITOR_ROOT_DIR ${CMAKE_SOURCE_DIR}/ ABSOLUTE CACHE)

# Enable solution folders in Visual Studio and Folders in Xcode
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Point CMake where to look for module files.
list(APPEND CMAKE_MODULE_PATH ${RC_EDITOR_ROOT_DIR}/misc/cmake-modules)

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set(HOST_SYSTEM_NAME "windows")
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" )
    message("use Windows")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64")
    set(SYSTEM_PROCESSOR "x86_64")
    message("use x86_64")
endif()

set(BUILD_TYPE "debug" CACHE STRING "BuildType")
set_property(CACHE BUILD_TYPE PROPERTY STRINGS "debug;release")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/)
set(CMAKE_LIBRARY_ARCHITECTURE     ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO_DATA ${CMAKE_BINARY_DIR}/bin/data/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/)
set(CMAKE_INSTALL_PREFIX ${RC_EDITOR_ROOT_DIR}/install/ CACHE PATH "Installation prefix")
set(STATIC_LIB_DIR
    ${RC_EDITOR_ROOT_DIR}/lib/${BUILD_TYPE}/${HOST_SYSTEM_NAME}/${SYSTEM_PROCESSOR}/
)
message("link lib dir: " ${STATIC_LIB_DIR})

set(RC_EDITOR_GLOBAL_INCLUDEDIRS
    "${CMAKE_BINARY_DIR}"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}/src/"
    "${CMAKE_SOURCE_DIR}/external/"
    "${CMAKE_SOURCE_DIR}/external/imgui"
    "${CMAKE_SOURCE_DIR}/src/imgui_node_editor"
)

# file(CREATE_LINK
#     ${RC_EDITOR_ROOT_DIR}/data/ ${CMAKE_BINARY_DIR}/bin/data/ SYMBOLIC
# )

message("create symbol link from \"" ${RC_EDITOR_ROOT_DIR}/data/ "\" to \"" ${CMAKE_BINARY_DIR}/bin/data/ "\"")

set(RC_EDITOR_DEF_SOURCEEXTS "*.cpp" "*.h" "*.hpp" "*.c")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/base)
add_subdirectory(src/application)
add_subdirectory(src/imgui_node_editor)
add_subdirectory(src/editor)

