cmake_minimum_required(VERSION 3.17)
set(CMAKE_POLICY_DEFAULT_CMP0177 NEW)

if (APPLE AND NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS deployment version")
endif ()

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif ()

project(observe VERSION 0.1.0)

if (APPLE)
  enable_language(OBJC)
  enable_language(OBJCXX)
endif ()

option(OBSERVE_AMALGAMATED_BUILD "Compile sources together" ON)
option(OBSERVE_ENABLE_WIDGETS "Add widgets library" ON)
option(OBSERVE_ENABLE_BACKGROUND_GRAPHICS_THREAD "Offloads graphics rendering to a background thread" OFF)
option(OBSERVE_ENABLE_GRAPHICS_DEBUG_LOGGING "Shows graphics debug log in console in debug mode" OFF)
option(OBSERVE_ADDRESS_SANITIZER "Enable AddressSanitizer" OFF)

if (OBSERVE_ADDRESS_SANITIZER)
  if (MSVC)
    add_compile_options(/fsanitize=address /Zi)
    add_link_options(/fsanitize=address)
  else ()
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
  endif ()
endif ()

set(OBSERVE_APPLICATION_NAME ${CMAKE_PROJECT_NAME} CACHE STRING "Application name used for default window title")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  option(OBSERVE_BUILD_EXAMPLES "Build examples" ON)
  option(OBSERVE_BUILD_TESTS "Build tests" ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else ()
  option(OBSERVE_BUILD_EXAMPLES "Build examples" ON)
  option(OBSERVE_BUILD_TESTS "Build tests" ON)
endif ()

set(OBSERVE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
include(../cmake/testing_framework.cmake)

set(OBSERVE_INCLUDE ${CMAKE_CURRENT_BINARY_DIR}/include)

function(amalgamate_headers AMALGAMATED_HEADER_NAME FILE_HEADERS)
  set(FILE_CONTENTS "// Auto generated amalgamated header.\n#pragma once\n")
  foreach (FILE ${FILE_HEADERS})
    set(FILE_CONTENTS "${FILE_CONTENTS}#include \"${FILE}\"\n")
  endforeach ()
  file(CONFIGURE OUTPUT "${AMALGAMATED_HEADER_NAME}" CONTENT "${FILE_CONTENTS}")
endfunction()

function(amalgamate_sources GENERATED_FILES AMALGAMATED_SOURCE_BASE FILE_SOURCES)
  set(IDX 0)
  set(PART 0)
  set(GENERATED "")

  foreach (FILE ${FILE_SOURCES})
    set_source_files_properties("${FILE}" PROPERTIES HEADER_FILE_ONLY TRUE)
    if (IDX EQUAL 0)
      if (PART GREATER 0)
        file(CONFIGURE OUTPUT "${AMALGAMATED_SOURCE_BASE}${PART}.cpp" CONTENT "${FILE_CONTENTS}")
      endif ()

      math(EXPR PART "${PART} + 1")
      list(APPEND GENERATED "${AMALGAMATED_SOURCE_BASE}${PART}.cpp")
      set(FILE_CONTENTS "// Auto generated amalgamated source.\n")
    endif ()
    set(FILE_CONTENTS "${FILE_CONTENTS}#include \"${FILE}\"\n")

    math(EXPR IDX "${IDX} + 1")
    if (IDX GREATER 6)
      set(IDX 0)
    endif ()
  endforeach ()

  set(${GENERATED_FILES} "${GENERATED}" PARENT_SCOPE)
  file(CONFIGURE OUTPUT "${AMALGAMATED_SOURCE_BASE}${PART}.cpp" CONTENT "${FILE_CONTENTS}")
endfunction()

add_subdirectory(observe_file_embed)
add_subdirectory(observe_graphics)
add_subdirectory(observe_ui)
add_subdirectory(observe_utils)
add_subdirectory(observe_widgets)
add_subdirectory(observe_windowing)
add_subdirectory(observe_app)

set(DUMMY_FILE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp)
if (NOT EXISTS ${DUMMY_FILE})
  file(WRITE ${DUMMY_FILE} "")
endif ()

add_library(observe STATIC
  ${DUMMY_FILE}
)

target_include_directories(observe SYSTEM PUBLIC ${OBSERVE_INCLUDE_PATH} ${OBSERVE_INCLUDE})
target_link_libraries(observe PRIVATE
  ObserveApp
  ObserveGraphics
  ObserveUtils
  ObserveUi
  ObserveWidgets
  ObserveWindowing
)

set_target_properties(observe PROPERTIES FOLDER "observe")