# Compiler options and standard target wiring shared across observe's
# own targets.

function(observe_configure_compile_options TARGET)

    target_compile_options(${TARGET} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /Zc:preprocessor>
    )

endfunction()

# Applies everything the observe_core library itself needs - shader
# dependency, compile options, include directories, and link libraries.
# SDL3/bgfx/bx are linked PUBLIC (rather than PRIVATE) deliberately:
# observe's own public headers return/store bgfx types by value (e.g.
# Shader::handle() returns bgfx::ProgramHandle), so anything linking
# observe_core needs those headers too, not just observe_core's own
# compilation.
function(observe_configure_library TARGET)

    add_dependencies(${TARGET} shaders)

    observe_configure_compile_options(${TARGET})

    target_include_directories(${TARGET}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include

        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
            ${CMAKE_CURRENT_SOURCE_DIR}/external/freetype/include
            ${CMAKE_CURRENT_SOURCE_DIR}/external/bgfx.cmake/bx/include
    )

    target_link_libraries(${TARGET} PUBLIC
        SDL3::SDL3-static
        #freetype
        bgfx
        bx
    )

endfunction()

# Applies what a thin observe executable target needs beyond linking
# observe_core (which brings its include directories and dependencies
# along transitively) - just the Windows subsystem and compile options.
function(observe_configure_executable TARGET GUI)

    observe_set_windows_subsystem(${TARGET} ${GUI})

    observe_configure_compile_options(${TARGET})

    target_link_libraries(${TARGET} PRIVATE observe_core)

endfunction()
