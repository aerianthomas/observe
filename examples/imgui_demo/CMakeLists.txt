# Optional debug/tooling layer for observe, built on ImGui. Nothing here
# is linked into observe_core or the observe/observe-console executables -
# this is a separate example target that links observe_core from the
# outside, exactly like any other application built on the framework
# would.

observe_compile_shader(
    NAME vs_imgui
    TYPE vertex
    VARYING varying_imgui.def.sc)

observe_compile_shader(
    NAME fs_imgui
    TYPE fragment
    VARYING varying_imgui.def.sc)

add_custom_target(
    imgui_demo_shaders ALL

    DEPENDS
        ${SHADER_OUTPUTS}
)

add_executable(imgui_demo
    main.cpp
    imgui_layer.cpp
    imgui_context.cpp
    imgui_renderer.cpp

    ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_demo.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_sdl3.cpp
)

add_dependencies(imgui_demo imgui_demo_shaders)

# Shader::load() resolves paths like "shaders/vs_imgui.bin" relative to the
# process's current working directory, not the executable's own location.
# observe/observe-console sit directly in the top-level build directory by
# default, right next to shaders/, so this "just works" for them. Without
# this, imgui_demo would default to examples/imgui_demo/ instead (mirroring
# its source path), two directories away from shaders/ - shader loading
# would silently fail, Application::initialize() would return false, and
# the app would exit immediately with no window and no error message.
set_target_properties(imgui_demo
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

observe_set_windows_subsystem(imgui_demo FALSE)

observe_configure_compile_options(imgui_demo)

target_include_directories(imgui_demo
    PRIVATE
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bx/include
)

target_link_libraries(imgui_demo PRIVATE
    observe_core
)
