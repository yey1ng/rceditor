project(imgui VERSION 1.76)

set(_imgui_Sources
    imconfig.h
    imgui.cpp
    imgui.h
    imgui.natvis
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_widgets.cpp
    imgui_tables.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    backend/imgui_impl_win32.h
    backend/imgui_impl_win32.cpp
    backend/imgui_impl_dx11.h
    backend/imgui_impl_dx11.cpp
    backend/imgui_impl_vulkan.h
    backend/imgui_impl_vulkan.cpp
    backend/imgui_impl_glfw.h
    backend/imgui_impl_glfw.cpp
    LICENSE.txt
)

add_library(${PROJECT_NAME} STATIC ${_imgui_Sources})

target_include_directories(${PROJECT_NAME} 
PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RC_EDITOR_ROOT_DIR}/external/
    )
#target_compile_definitions(${PROJECT_NAME} PUBLIC "ImDrawIdx=unsigned int")
target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${_imgui_Sources})

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "external")

