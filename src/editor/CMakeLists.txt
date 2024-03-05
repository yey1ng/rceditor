# set(NAME editor)
# project(editor)

# set(RC_EDITOR_EDITOR_SOURCES "")
# file(GLOB_RECURSE RC_EDITOR_EDITOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${RC_EDITOR_DEF_SOURCEEXTS})

# source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RC_EDITOR_EDITOR_SOURCES})

# file(GLOB RC_EDITOR_RESOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")
# file(GLOB RC_EDITOR_EDITOR_RESOURCES CONFIGURE_DEPENDS "${RC_EDITOR_ROOT_DIR}/src/data/*")

# set(PLATFORM_TYPE WIN32)
# set(APPLICATION_ICON ${RC_EDITOR_ROOT_DIR}/src/application/support/Icon.ico)
# file(TO_NATIVE_PATH "${APPLICATION_ICON}" APPLICATION_ICON)
# string(REPLACE "\\" "\\\\" APPLICATION_ICON "${APPLICATION_ICON}")
# configure_file(
#     ${RC_EDITOR_ROOT_DIR}/src/application/support/Resource.rc.in
#     ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
# )

# source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${RC_EDITOR_RESOURCES})
# source_group(TREE "${RC_EDITOR_ROOT_DIR}/src" FILES ${RC_EDITOR_EDITOR_RESOURCES})

# list(APPEND RC_EDITOR_RESOURCES
#     ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
#     ${RC_EDITOR_EDITOR_RESOURCES}
# )
# source_group("resources" FILES ${RC_EDITOR_EDITOR_RESOURCES}/Resource.rc)

# add_executable(${NAME} ${PLATFORM_TYPE} ${RC_EDITOR_EDITOR_SOURCES} ${RC_EDITOR_RESOURCES} ${RC_EDITOR_EDITOR_RESOURCES})

# find_package(imgui REQUIRED)
# find_package(imgui_node_editor REQUIRED)

# target_include_directories(${NAME} PRIVATE ${RC_EDITOR_GLOBAL_INCLUDEDIRS})

# target_link_directories(${NAME} PRIVATE ${STATIC_LIB_DIR})
# target_link_libraries(${NAME} PRIVATE imgui glfw3 imgui_node_editor application)

# set(RC_EDITOR_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# # set_target_properties(${NAME} PROPERTIES
# #     FOLDER "example"
# #     RUNTIME_OUTPUT_DIRECTORY                "${RC_EDITOR_BIN_DIR}"
# #     RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${RC_EDITOR_BIN_DIR}"
# #     RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${RC_EDITOR_BIN_DIR}"
# #     RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${RC_EDITOR_BIN_DIR}"
# #     RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${RC_EDITOR_BIN_DIR}"
# #     DEBUG_POSTFIX                           _d
# #     RELWITHDEBINGO_POSTFIX                  _rd
# #     MINSIZEREL_POSTFIX                      _r
# #     VS_DEBUGGER_WORKING_DIRECTORY           ${RC_EDITOR_BIN_DIR}
# # )

# add_custom_command(
#     TARGET ${NAME}
#     PRE_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${RC_EDITOR_BIN_DIR}/data
# )


# message("CMAKE_CURRENT_BINARY_DIR \"" ${RC_EDITOR_RESOURCES} "\"")
# message("CMAKE_CURRENT_BINARY_DIR \"" ${CMAKE_CURRENT_SOURCE_DIR} "\"")
# message("CMAKE_CURRENT_BINARY_DIR \"" ${RC_EDITOR_EDITOR_RESOURCES} "\"")

# set(RESOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
# foreach(RESOURCE ROOT "${RC_EDITOR_ROOT_DIR}/src/data" ${RC_EDITOR_RESOURCES} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${RC_EDITOR_EDITOR_RESOURCES})
#     if (RESOURCE STREQUAL ROOT)
#         set(RESOURCE_ROOT FALSE)
#         continue()
#     elseif(NOT RESOURCE_ROOT)
#         set(RESOURCE_ROOT ${RESOURCE})
#         continue()
#     endif()

#     if ("${RESOURCE}" MATCHES "\.DS_Store$")
#         list(REMOVE_ITEM RC_EDITOR_EDITOR_RESOURCES ${RESOURCE})
#         list(REMOVE_ITEM RC_EDITOR_SOURCES ${RESOURCE})
#         continue()
#     endif()

#     file(RELATIVE_PATH REL_RESOURCE ${RESOURCE_ROOT} ${RESOURCE})

#     # add_custom_command(
#     #     TARGET ${NAME}
#     #     PRE_BUILD
#     #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${RESOURCE} ${RC_EDITOR_BIN_DIR}/data/${REL_RESOURCE}
#     # )

#     message("CMAKE_CURRENT_BINARY_DIR \"" ${RESOURCE} "\"")
#     message("CMAKE_CURRENT_BINARY_DIR \"" ${RC_EDITOR_BIN_DIR}/data/${REL_RESOURCE} "\"")
# endforeach()

cmake_minimum_required(VERSION 3.12)

set(NAME editor)

project(${NAME})

set(RC_EDITOR_EDITOR_SOURCE "")
file(GLOB_RECURSE RC_EDITOR_EDITOR_SOURCE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${RC_EDITOR_DEF_SOURCEEXTS})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RC_EDITOR_EDITOR_SOURCE})
file(GLOB RC_EDITOR_COMMON_RESOURCES CONFIGURE_DEPENDS "${RC_EDITOR_ROOT_DIR}/src/data/*")
file(GLOB RC_EDITOR_RESOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/*")

set(PLATFORM_TYPE WIN32)

set(APPLICATION_ICON ${RC_EDITOR_ROOT_DIR}/src/application/support/Icon.ico)
file(TO_NATIVE_PATH "${APPLICATION_ICON}" APPLICATION_ICON)
string(REPLACE "\\" "\\\\" APPLICATION_ICON "${APPLICATION_ICON}")
configure_file(
    ${RC_EDITOR_ROOT_DIR}/src/application/support/Resource.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
)
source_group(TREE "${RC_EDITOR_ROOT_DIR}/src" FILES ${RC_EDITOR_COMMON_RESOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${RC_EDITOR_RESOURCES})
list(APPEND RC_EDITOR_RESOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc
    ${RC_EDITOR_COMMON_RESOURCES}
)
source_group("resources" FILES ${CMAKE_CURRENT_BINARY_DIR}/Resource.rc)

add_executable(${NAME} ${PLATFORM_TYPE} ${RC_EDITOR_EDITOR_SOURCE} ${RC_EDITOR_RESOURCES} ${RC_EDITOR_COMMON_RESOURCES})

find_package(imgui REQUIRED)
find_package(imgui_node_editor REQUIRED)

# target_include_directories(${NAME} PRIVATE 
#     ${RC_EDITOR_ROOT_DIR}/src/
#     ${RC_EDITOR_ROOT_DIR}/src/rps/include/
#     ${RC_EDITOR_ROOT_DIR}/external/)

target_include_directories(${NAME} PRIVATE ${RC_EDITOR_GLOBAL_INCLUDEDIRS})

target_link_directories(${NAME} PRIVATE ${STATIC_LIB_DIR})
target_link_libraries(${NAME} PRIVATE imgui glfw3 imgui_node_editor application)

set(_ExampleBinDir ${CMAKE_BINARY_DIR}/bin)

# set_target_properties(${NAME} PROPERTIES
#     FOLDER "example"
#     RUNTIME_OUTPUT_DIRECTORY                "${_ExampleBinDir}"
#     RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${_ExampleBinDir}"
#     RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${_ExampleBinDir}"
#     RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${_ExampleBinDir}"
#     RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${_ExampleBinDir}"
#     DEBUG_POSTFIX                           _d
#     RELWITHDEBINGO_POSTFIX                  _rd
#     MINSIZEREL_POSTFIX                      _r
#     VS_DEBUGGER_WORKING_DIRECTORY           ${_ExampleBinDir}
#     MACOSX_BUNDLE_INFO_PLIST                "${RC_EDITOR_ROOT_DIR}/src/application/support/Info.plist.in"
#     MACOSX_BUNDLE_BUNDLE_NAME               "${PACKAGE_NAME}"
#     MACOSX_BUNDLE_GUI_IDENTIFIER            "com.sandbox.collisions"
#     MACOSX_BUNDLE_LONG_VERSION_STRING       "${PACKAGE_VERSION}"
#     MACOSX_BUNDLE_SHORT_VERSION_STRING      "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}"
#     MACOSX_BUNDLE_ICON_FILE                 Icon.icns
# )

add_custom_command(
    TARGET ${NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ARGS ${_ExampleBinDir}/data
)

set(_ResourceRoot ${CMAKE_CURRENT_SOURCE_DIR})
foreach(_Resource ROOT "${RC_EDITOR_ROOT_DIR}/src/data" ${RC_EDITOR_COMMON_RESOURCES} ROOT "${CMAKE_CURRENT_SOURCE_DIR}/data" ${RC_EDITOR_RESOURCES})
    if (_Resource STREQUAL ROOT)
        set(_ResourceRoot FALSE)
        continue()
    elseif(NOT _ResourceRoot)
        set(_ResourceRoot ${_Resource})
        continue()
    endif()

    if ("${_Resource}" MATCHES "\.DS_Store$")
        list(REMOVE_ITEM RC_EDITOR_RESOURCES ${_Resource})
        list(REMOVE_ITEM RC_EDITOR_COMMON_RESOURCES ${_Resource})
        continue()
    endif()

    file(RELATIVE_PATH _RelResource ${_ResourceRoot} ${_Resource})

    add_custom_command(
        TARGET ${NAME}
        PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ARGS ${_Resource} ${_ExampleBinDir}/data/${_RelResource}
    )
endforeach()