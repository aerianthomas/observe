# Minimal proof that Canvas (observe_core's 2D drawing primitives) works
# standalone, with zero ImGui involvement - contrast with
# examples/imgui_demo, which proves the optional debug-tooling layer.

add_executable(canvas_demo
    main.cpp
    canvas_demo_layer.cpp
)

# Same reasoning as imgui_demo: shaders/ only exists directly under the
# top-level build directory, and Shader::load() resolves paths relative to
# the process's current working directory, not the executable's location.
set_target_properties(canvas_demo
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

observe_set_windows_subsystem(canvas_demo FALSE)

observe_configure_compile_options(canvas_demo)

target_link_libraries(canvas_demo PRIVATE
    observe_core
)
