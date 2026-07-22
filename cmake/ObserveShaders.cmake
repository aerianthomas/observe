# Compiles .sc shader sources into bgfx .bin shaders via the prebuilt
# shaderc tool committed under tools/. Call observe_compile_shader() once
# per shader; each call appends its output path to SHADER_OUTPUTS, which
# the caller wires up as a custom target's DEPENDS.

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

# NOTE: this block is a no-op left over from an earlier version of
# this file, from before observe_compile_shader() existed below.
# SHADER_VARYING is never set at this (top-level module) scope, so
# the if() below never runs and SHADER_VARYING_ARGS stays empty here.
# The real, active version of this logic lives inside the function,
# where SHADER_VARYING comes from cmake_parse_arguments() on each
# observe_compile_shader() call. Kept as-is (harmless) rather than
# removed, to keep this file's history a pure move with no behavior
# change; safe to delete whenever this file is touched again.
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