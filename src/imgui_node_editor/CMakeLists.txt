project(imgui_node_editor)

set(RC_EDITOR_IMGUI_NODE_EDITOR_SOURCES "")
file(GLOB_RECURSE RC_EDITOR_IMGUI_NODE_EDITOR_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${RC_EDITOR_DEF_SOURCEEXTS})

add_library(imgui_node_editor STATIC)
target_include_directories(imgui_node_editor PRIVATE ${RC_EDITOR_GLOBAL_INCLUDEDIRS})

find_package(imgui REQUIRED)
target_link_libraries(imgui_node_editor PUBLIC base)
target_link_libraries(imgui_node_editor PUBLIC imgui)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${RC_EDITOR_IMGUI_NODE_EDITOR_SOURCES})

target_sources(imgui_node_editor PRIVATE ${RC_EDITOR_IMGUI_NODE_EDITOR_SOURCES})