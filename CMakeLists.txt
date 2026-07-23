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
    NAME vs_ui
    TYPE vertex
    VARYING varying_ui.def.sc)

observe_compile_shader(
    NAME fs_ui
    TYPE fragment
    VARYING varying_ui.def.sc)

add_custom_target(
    shaders ALL

    DEPENDS
        ${SHADER_OUTPUTS}
)

add_subdirectory(external/SDL3)
#add_subdirectory(external/freetype)
add_subdirectory(external/bgfx.cmake)

observe_exclude_bimg_miniz()

set(OBSERVE_LIB_SOURCES
    src/app/application.cpp

    src/windowing/window.cpp

    src/graphics/renderer.cpp
    src/graphics/shader.cpp
    src/graphics/shader_loader.cpp
    src/graphics/vertex_layout.cpp
    src/graphics/mesh.cpp
    src/graphics/triangle.cpp
    src/graphics/color.cpp
    src/graphics/rect.cpp
    src/graphics/camera.cpp

    src/ui/canvas.cpp

    src/math/mat4.cpp
    src/math/transform.cpp
    src/math/vec2.cpp
    src/math/vec3.cpp
    src/math/vec4.cpp
)

# observe_core is the actual reusable framework: Window, Renderer,
# Application, math, graphics primitives. It has no ImGui dependency
# anywhere - see examples/imgui_demo for an optional debug/tooling layer
# that links this library from the outside instead.
add_library(observe_core STATIC ${OBSERVE_LIB_SOURCES})
observe_configure_library(observe_core)

# observe and observe-console are both thin wrappers around observe_core:
# same main.cpp (which defines both WinMain() and main(), see
# cmake/ObserveWindows.cmake), differing only in which Windows subsystem
# they link as.
add_executable(observe src/app/main.cpp)
observe_configure_executable(observe TRUE)

add_executable(observe-console src/app/main.cpp)
observe_configure_executable(observe-console FALSE)

add_subdirectory(examples/imgui_demo)
add_subdirectory(examples/canvas_demo)
