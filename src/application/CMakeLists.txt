project(application)

set(RC_EDITOR_APPLICATION_SOURCES "")
file(GLOB_RECURSE RC_EDITOR_APPLICATION_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${RC_EDITOR_DEF_SOURCEEXTS})

add_library(application STATIC)
target_include_directories(application PUBLIC ${RC_EDITOR_GLOBAL_INCLUDEDIRS})

find_package(imgui REQUIRED)
target_link_libraries(application PUBLIC imgui)
target_link_libraries(application PUBLIC imgui_node_editor)

set(DXSDK_DIR  ${RC_EDITOR_ROOT_DIR}/external/DXSDK)
set(DXSDK_ARCH x86)
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(DXSDK_ARCH x64)
endif()

add_library(dxerr STATIC ${DXSDK_DIR}/src/dxerr.cpp)
target_include_directories(dxerr PUBLIC "${DXSDK_DIR}/include")
set_property(TARGET dxerr PROPERTY FOLDER "external")

add_library(d3dx11 UNKNOWN IMPORTED)
set_target_properties(d3dx11 PROPERTIES
    IMPORTED_LOCATION               "${DXSDK_DIR}/lib/${DXSDK_ARCH}/d3dx11.lib"
    IMPORTED_LOCATION_DEBUG         "${DXSDK_DIR}/lib/${DXSDK_ARCH}/d3dx11d.lib"
    INTERFACE_INCLUDE_DIRECTORIES   "${DXSDK_DIR}/include"
    INTERFACE_LINK_LIBRARIES        "$<$<CONFIG:Debug>:dxerr>"
)

target_link_libraries(application PRIVATE d3d11.lib d3dcompiler.lib d3dx11)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RC_EDITOR_APPLICATION_SOURCES})
target_sources(application PRIVATE ${RC_EDITOR_APPLICATION_SOURCES}) 