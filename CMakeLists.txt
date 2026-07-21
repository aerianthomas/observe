cmake_minimum_required(VERSION 3.22)

project(observe)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(ObserveWindows)
include(ObserveCompiler)
include(ObserveShaders)
include(ObserveUtilities)

observe_configure_third_party_options()

observe_compile_shader(
    NAME vs_triangle
    TYPE vertex)

observe_compile_shader(
    NAME fs_triangle
    TYPE fragment)

observe_compile_shader(
    NAME vs_imgui
    TYPE vertex
    VARYING varying_imgui.def.sc)

observe_compile_shader(
    NAME fs_imgui
    TYPE fragment
    VARYING varying_imgui.def.sc)

add_custom_target(
    shaders ALL

    DEPENDS
        ${SHADER_OUTPUTS}
)

add_subdirectory(external/SDL3)
#add_subdirectory(external/freetype)
add_subdirectory(external/bgfx.cmake)

observe_exclude_bimg_miniz()

set(APP_SOURCES
    src/app/main.cpp
    src/app/application.cpp
)

set(WINDOWING_SOURCES
    src/windowing/window.cpp
)

set(GRAPHICS_SOURCES
    src/graphics/renderer.cpp
    src/graphics/shader.cpp
    src/graphics/shader_loader.cpp
    src/graphics/vertex_layout.cpp
    src/graphics/mesh.cpp
    src/graphics/triangle.cpp
    src/graphics/camera.cpp
)

set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl3.cpp
    src/imgui/imgui_context.cpp
    src/imgui/imgui_renderer.cpp
)

set(MATH_SOURCES
    src/math/mat4.cpp
    src/math/transform.cpp
    src/math/vec2.cpp
    src/math/vec3.cpp
    src/math/vec4.cpp
)

set(OBSERVE_SOURCES
    ${APP_SOURCES}
    ${WINDOWING_SOURCES}
    ${GRAPHICS_SOURCES}
    ${IMGUI_SOURCES}
    ${MATH_SOURCES}
)

# Built together, every time: observe (windowed) and observe-console
# (stdout/stderr visible), from the same sources. See
# cmake/ObserveWindows.cmake for how src/app/main.cpp supports both without
# any preprocessor branching.
add_executable(observe ${OBSERVE_SOURCES})
observe_configure_executable(observe TRUE)

add_executable(observe-console ${OBSERVE_SOURCES})
observe_configure_executable(observe-console FALSE)