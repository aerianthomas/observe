cmake_minimum_required(VERSION 3.22)

project(observe)

set(CMAKE_CXX_STANDARD 20)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

set(BGFX_CONFIG_MULTITHREADED ON CACHE BOOL "" FORCE)
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(BGFX_BUILD_TESTS OFF CACHE BOOL "" FORCE)

set(SHADERC
    ${CMAKE_SOURCE_DIR}/tools/shaderc.exe)

set(SHADER_SOURCE_DIR
    ${CMAKE_SOURCE_DIR}/assets/shaders)

set(SHADER_OUTPUT_DIR
    ${CMAKE_BINARY_DIR}/shaders)

file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SHADER_OUTPUTS)

set(SHADER_PLATFORM windows)
set(SHADER_PROFILE s_5_0)

set(SHADER_VARYING_ARGS "")

if(SHADER_VARYING)
    list(APPEND SHADER_VARYING_ARGS
        --varyingdef
        ${SHADER_SOURCE_DIR}/${SHADER_VARYING}
    )
endif()

function(observe_compile_shader)

    cmake_parse_arguments(
        SHADER
        ""
        "NAME;TYPE;VARYING"
        ""
        ${ARGN}
    )

    message(STATUS "Shader ${SHADER_NAME} varying = ${SHADER_VARYING}")

    set(SHADER_VARYING_ARGS)

    if(SHADER_VARYING)
        list(APPEND SHADER_VARYING_ARGS
            --varyingdef
            ${SHADER_SOURCE_DIR}/${SHADER_VARYING}
        )
    endif()

    message(STATUS "Shader ${SHADER_NAME} varying args = ${SHADER_VARYING_ARGS}")

    add_custom_command(
        OUTPUT
            ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin

        COMMAND
            ${SHADERC}
            -f ${SHADER_SOURCE_DIR}/${SHADER_NAME}.sc
            -o ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin
            --type ${SHADER_TYPE}
            --platform ${SHADER_PLATFORM}
            -p ${SHADER_PROFILE}
            -i ${CMAKE_SOURCE_DIR}/external/bgfx.cmake/bgfx/src
            ${SHADER_VARYING_ARGS}

        DEPENDS
            ${SHADER_SOURCE_DIR}/${SHADER_NAME}.sc

        VERBATIM
    )

        set(
        SHADER_OUTPUTS
        ${SHADER_OUTPUTS}
        ${SHADER_OUTPUT_DIR}/${SHADER_NAME}.bin
        PARENT_SCOPE
    )

endfunction()

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
add_subdirectory(external/freetype)
add_subdirectory(external/bgfx.cmake)


set(APP_SOURCES
    src/app/main.cpp
    src/app/application.cpp
    src/imgui_context.cpp
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

    src/imgui/imgui_renderer.cpp
)

set(MATH_SOURCES
    src/math/mat4.cpp
    src/math/transform.cpp
    src/math/vec2.cpp
    src/math/vec3.cpp
    src/math/vec4.cpp
)

add_executable(observe
    ${APP_SOURCES}
    ${WINDOWING_SOURCES}
    ${GRAPHICS_SOURCES}
    ${IMGUI_SOURCES}
    ${MATH_SOURCES}
)

add_dependencies(observe shaders)

target_compile_options(observe PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /Zc:preprocessor>
)

target_include_directories(observe
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_CURRENT_SOURCE_DIR}/external/SDL3/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/bgfx.cmake/bx/include
)

target_link_libraries(observe PRIVATE
    SDL3::SDL3-static
    freetype
    bgfx
    bx
)