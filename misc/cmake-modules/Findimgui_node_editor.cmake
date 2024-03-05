if (TARGET imgui_node_editor)
    return()
endif()

find_package(imgui REQUIRED)

set(_imgui_node_editor_Sources
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/crude_json.cpp
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/crude_json.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_bezier_math.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_bezier_math.inl
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_canvas.cpp
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_canvas.cpp
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_canvas.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_canvas.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_extra_math.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_extra_math.inl
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_node_editor_api.cpp
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_node_editor_internal.h
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_node_editor_internal.inl
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_node_editor.cpp
    ${RC_EDITOR_ROOT_DIR}/src/imgui_node_editor/imgui_node_editor.h
    ${RC_EDITOR_ROOT_DIR}/misc/imgui_node_editor.natvis
)

add_library(imgui_node_editor STATIC
    ${_imgui_node_editor_Sources}
)

target_include_directories(imgui_node_editor PUBLIC
    ${RC_EDITOR_ROOT_DIR}
)

target_link_libraries(imgui_node_editor PUBLIC imgui)

source_group(TREE ${RC_EDITOR_ROOT_DIR} FILES ${_imgui_node_editor_Sources})

include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)

find_package_handle_standard_args(
    imgui_node_editor
    REQUIRED_VARS
    RC_EDITOR_ROOT_DIR
)

set_property(TARGET imgui_node_editor PROPERTY FOLDER "base")