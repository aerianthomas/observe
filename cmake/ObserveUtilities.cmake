# Build-option overrides for vendored submodules, and workarounds for
# issues in their CMake setup. Call observe_configure_third_party_options()
# before add_subdirectory()-ing SDL3/bgfx.cmake, and
# observe_exclude_bimg_miniz() after add_subdirectory(external/bgfx.cmake).

function(observe_configure_third_party_options)

    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)

    set(BGFX_CONFIG_MULTITHREADED ON CACHE BOOL "" FORCE)
    set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BGFX_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
    set(BGFX_BUILD_TESTS OFF CACHE BOOL "" FORCE)

endfunction()

# bimg vendors its own copy of miniz (used only for EXR support via
# tinyexr) and compiles it as a standalone source file into the `bimg`
# target, which bgfx links unconditionally. SDL3 also vendors its own
# separate copy of miniz (src/video/miniz.h, pulled in by
# src/video/SDL_stb.c for its built-in surface-loading/PNG-encode
# support), and both end up in the same final link, producing
# duplicate-symbol linker warnings (LNK4006) for mz_*/tdefl_*/tinfl_*
# symbols. We don't use bimg's EXR decoding, so exclude its copy of
# miniz.c from compilation entirely, without touching the vendored
# bgfx.cmake/bimg submodule itself.
#
# DIRECTORY must point at external/bgfx.cmake/cmake/bimg, since that's the
# add_subdirectory() scope in which the bimg/bimg_decode/bimg_encode
# targets (and this file's compilation) are actually declared.
function(observe_exclude_bimg_miniz)

    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/external/bgfx.cmake/bimg/3rdparty/tinyexr/deps/miniz/miniz.c
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/bgfx.cmake/cmake/bimg
        PROPERTIES HEADER_FILE_ONLY ON
    )

endfunction()