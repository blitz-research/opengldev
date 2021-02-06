
cmake_minimum_required(VERSION 3.5)

project(opengldev LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(opengldev main.cpp)

target_link_libraries(opengldev glfw GLESv2)

add_compile_definitions(
    GLFW_INCLUDE_ES3=1
    IMGUI_IMPL_OPENGL_LOADER_CUSTOM=<GLFW/glfw3.h>
    IMGUI_IMPL_OPENGL_ES3=1)

# ImGui lib - I tried to make this a subproject that would add the
# include paths, libs etc used by imgui but I'm not smart enough.
target_include_directories(opengldev PUBLIC imgui imgui/backends)
target_sources(opengldev PUBLIC
        imgui/imconfig.h
        imgui/imgui.cpp
        imgui/imgui.h
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_truetype.h
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_opengl3.h)
