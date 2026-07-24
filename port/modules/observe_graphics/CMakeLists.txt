file(GLOB HEADERS *.h)
file(GLOB SOURCE_FILES *.cpp)

if (WIN32)
  list(APPEND SOURCE_FILES win32/emoji_win32.cpp)
  set(WIN_LIBS d2d1 dwrite Windowscodecs)
elseif (APPLE)
  list(APPEND SOURCE_FILES macos/emoji_macos.cpp)
  list(APPEND PLATFORM_FILES macos/windowless_context.mm)
elseif (UNIX)
  list(APPEND SOURCE_FILES linux/emoji_linux.cpp)
endif ()

if (OBSERVE_AMALGAMATED_BUILD)
  amalgamate_sources(AMALGAMATED ${CMAKE_CURRENT_BINARY_DIR}/observe_graphics_amalgamated "${SOURCE_FILES}")
  list(APPEND SOURCE_FILES ${AMALGAMATED})
endif ()

amalgamate_headers(${OBSERVE_INCLUDE}/observe/graphics.h "${HEADERS}")
list(APPEND HEADERS ${OBSERVE_INCLUDE}/observe/graphics.h)

add_library(ObserveGraphics OBJECT ${SOURCE_FILES} ${PLATFORM_FILES} ${HEADERS})

if (APPLE)
  find_library(METAL_LIBRARY Metal)
  find_library(METALKIT_LIBRARY MetalKit)
  set(METAL_LIBS ${METAL_LIBRARY} ${METALKIT_LIBRARY})
elseif (UNIX)
  add_definitions(-D_LARGEFILE64_SOURCE)
endif ()

set(BGFX_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
set(BGFX_BUILD_EXAMPLE_COMMON OFF CACHE INTERNAL "" FORCE)
set(BGFX_BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)
set(BGFX_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
set(BGFX_INSTALL OFF CACHE INTERNAL "" FORCE)
set(BX_AMALGAMATED ON CACHE INTERNAL "" FORCE)
set(BX_CONFIG_DEBUG OFF CACHE INTERNAL "" FORCE)
set(BGFX_CONFIG_DEBUG_ANNOTATION ${OBSERVE_ENABLE_GRAPHICS_DEBUG_LOGGING} CACHE INTERNAL "" FORCE)
set(BGFX_AMALGAMATED ON CACHE INTERNAL "" FORCE)
set(BGFX_CONFIG_MAX_FRAME_BUFFERS 1024 CACHE INTERNAL "" FORCE)
set(BGFX_CONFIG_MAX_VIEWS 256 CACHE INTERNAL "" FORCE)
set(BGFX_CONFIG_MAX_DRAW_CALLS 8192 CACHE INTERNAL "" FORCE)
set(BGFX_WITH_WAYLAND OFF CACHE INTERNAL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)

if (OBSERVE_ENABLE_GRAPHICS_DEBUG_LOGGING)
  target_compile_definitions(ObserveGraphics PUBLIC OBSERVE_GRAPHICS_DEBUG_LOGGING=1)
endif ()

set(BGFX_CONFIG_MULTITHREADED 0 CACHE INTERNAL "" FORCE)
if (NOT EMSCRIPTEN AND OBSERVE_ENABLE_BACKGROUND_GRAPHICS_THREAD)
  set(BGFX_CONFIG_MULTITHREADED 1 CACHE INTERNAL "" FORCE)
  target_compile_definitions(ObserveGraphics PUBLIC OBSERVE_BACKGROUND_GRAPHICS_THREAD=1)
endif ()

observe_fetch_dependency(bgfx
  SOURCE_DIR "${OBSERVE_BGFX_SOURCE_DIR}"
  GIT_REPOSITORY https://github.com/bkaradzic/bgfx.cmake.git
  GIT_TAG v1.129.8958-499
)

observe_fetch_dependency(freetype
  SOURCE_DIR "${OBSERVE_FREETYPE_SOURCE_DIR}"
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  GIT_TAG VER-2-14-1
)

if (MSVC)
  set(DISABLE_WARNINGS /W0)
else ()
  set(DISABLE_WARNINGS -w)
endif ()

target_compile_options(freetype PRIVATE ${DISABLE_WARNINGS})
target_compile_options(bgfx PRIVATE ${DISABLE_WARNINGS})
target_compile_options(bx PRIVATE ${DISABLE_WARNINGS})
target_compile_definitions(bx PRIVATE BX_CONFIG_EXCEPTION_HANDLING_USE_WINDOWS_SEH=0)
target_compile_options(bimg_decode PRIVATE ${DISABLE_WARNINGS})
target_compile_options(bimg_encode PRIVATE ${DISABLE_WARNINGS})
target_compile_options(bimg PRIVATE ${DISABLE_WARNINGS})

target_link_libraries(
  ObserveGraphics
  PRIVATE
  bx
  bgfx
  bx
  bimg_decode
  bimg_encode
  freetype
  ${WIN_LIBS}
  ${METAL_LIBS}
  ${FONTCONFIG_LIBRARIES}
)

include(embedded.cmake)

target_include_directories(ObserveGraphics
  PRIVATE
  ${OBSERVE_INCLUDE_PATH}
  ${freetype_SOURCE_DIR}/include
  ${BGFX_DIR}/include
  ${BGFX_DIR}/3rdparty
  ${BX_DIR}/include
  ${BIMG_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}
)

if(WIN32)
  target_compile_definitions(ObserveGraphics PRIVATE
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
    NTDDI_VERSION=0x0A000000
  )
endif()

target_include_directories(ObserveGraphics
  SYSTEM PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bgfx/tools
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bx/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bimg/include
)

set_target_properties(ObserveGraphics PROPERTIES FOLDER "observe")
set_target_properties(bgfx PROPERTIES FOLDER "observe/third_party/bgfx")
set_target_properties(bx PROPERTIES FOLDER "observe/third_party/bgfx")
set_target_properties(bimg PROPERTIES FOLDER "observe/third_party/bgfx")
set_target_properties(bimg_decode PROPERTIES FOLDER "observe/third_party/bgfx")
set_target_properties(bimg_encode PROPERTIES FOLDER "observe/third_party/bgfx")


set_target_properties(freetype PROPERTIES FOLDER "observe/third_party")

if (APPLE)
  target_compile_options(ObserveGraphics PRIVATE -fobjc-arc)
endif ()
