# Compiler options and standard target wiring shared across observe's
# own executable targets.

function(observe_configure_compile_options TARGET)

    target_compile_options(${TARGET} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus /Zc:preprocessor>
    )

endfunction()

# Applies everything a fully-configured observe executable target needs -
# Windows subsystem, shader dependency, compile options, include
# directories, and link libraries - beyond the plain add_executable() call
# and its source list, which stay in the top-level CMakeLists.txt. Used for
# both the observe (GUI) and observe-console (console) targets, so they
# stay in lockstep as dependencies/include paths/etc. change over time.
function(observe_configure_executable TARGET GUI)

    observe_set_windows_subsystem(${TARGET} ${GUI})

    add_dependencies(${TARGET} shaders)

    observe_configure_compile_options(${TARGET})

    target_include_directories(${TARGET}
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

    target_link_libraries(${TARGET} PRIVATE
        SDL3::SDL3-static
        #freetype
        bgfx
        bx
    )

endfunction()