if (TARGET imgui)
    return()
endif()

set(IMGUI_SOURCEDIR ${RC_EDITOR_ROOT_DIR}/external/imgui)
set(IMGUI_BINARYDIR ${CMAKE_BINARY_DIR}/external/imgui)

add_subdirectory(${IMGUI_SOURCEDIR} ${IMGUI_BINARYDIR})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui
    REQUIRED_VARS
    IMGUI_SOURCEDIR
)

