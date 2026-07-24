cmake_minimum_required(VERSION 3.17)
set(CMAKE_POLICY_DEFAULT_CMP0177 NEW)

if (WIN32 AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "MSVC runtime library")
endif ()
if (WIN32)
  set(SMTG_USE_STATIC_CRT ON CACHE BOOL "Use static CRuntime on Windows for VST3 SDK targets" FORCE)
endif ()

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
option(OBSERVE_BUILD_OSCILLATORS "Build the Oscillators Observe application and plug-ins" ON)
option(OSCILLATORS_BUILD_OBSERVE_APP "Build the Oscillators Observe standalone app" ON)
option(OSCILLATORS_BUILD_CLAP "Build the Oscillators Observe CLAP plug-in" ON)
option(OSCILLATORS_BUILD_VST3 "Build the Oscillators Observe VST3 plug-in" ON)
option(OBSERVE_BUILD_VST3SDK_EXAMPLES "Build the Steinberg VST3 SDK plug-in examples" OFF)
option(OBSERVE_BUILD_VST3SDK_HOSTING_EXAMPLES "Build the Steinberg VST3 SDK hosting examples" OFF)
option(OBSERVE_ENABLE_VSTGUI "Enable VSTGUI support in the VST3 SDK" OFF)

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
  option(OBSERVE_BUILD_EXAMPLES "Build examples" OFF)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
else ()
  option(OBSERVE_BUILD_EXAMPLES "Build examples" OFF)
endif ()

set(OBSERVE_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(OBSERVE_DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external" CACHE PATH "Dependency checkout root")
set(OBSERVE_BGFX_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/bgfx.cmake" CACHE PATH "Local bgfx.cmake source directory")
set(OBSERVE_CLAP_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/clap" CACHE PATH "Local CLAP SDK source directory")
set(OBSERVE_CLAP_HELPERS_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/clap-helpers" CACHE PATH "Local CLAP helpers source directory")
set(OBSERVE_FREETYPE_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/freetype" CACHE PATH "Local FreeType source directory")
set(OBSERVE_MINIAUDIO_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/miniaudio" CACHE PATH "Local miniaudio source directory")
set(OBSERVE_VST3SDK_SOURCE_DIR "${OBSERVE_DEPENDENCIES_DIR}/vst3sdk" CACHE PATH "Local VST3 SDK source directory")
set(OBSERVE_ARTIFACTS_DIR "${CMAKE_BINARY_DIR}/artifacts" CACHE PATH "Root directory for built applications and plug-ins")

set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES ${OBSERVE_BUILD_VST3SDK_EXAMPLES} CACHE BOOL "Build VST3 SDK plug-in examples" FORCE)
set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES ${OBSERVE_BUILD_VST3SDK_HOSTING_EXAMPLES} CACHE BOOL "Build VST3 SDK hosting examples" FORCE)
set(SMTG_ENABLE_VSTGUI_SUPPORT ${OBSERVE_ENABLE_VSTGUI} CACHE BOOL "Enable VSTGUI support" FORCE)
set(SMTG_CREATE_PLUGIN_LINK OFF CACHE BOOL "Create symbolic links for VST3 plug-ins" FORCE)

include(FetchContent)

function(observe_fetch_dependency NAME)
  set(single_options SOURCE_DIR CHECK_FILE SOURCE_SUBDIR GIT_REPOSITORY GIT_TAG)
  cmake_parse_arguments(PARSE "" "${single_options}" "" ${ARGN})

  if (NOT PARSE_SOURCE_DIR)
    message(FATAL_ERROR "OBSERVE: Missing SOURCE_DIR for ${NAME}")
  endif ()
  if (NOT PARSE_CHECK_FILE)
    set(PARSE_CHECK_FILE CMakeLists.txt)
  endif ()
  if (PARSE_SOURCE_SUBDIR)
    set(_observe_subdir_args SOURCE_SUBDIR "${PARSE_SOURCE_SUBDIR}")
  endif ()

  set(_observe_previous_message_log_level "${CMAKE_MESSAGE_LOG_LEVEL}")
  set(CMAKE_MESSAGE_LOG_LEVEL STATUS)
  if (EXISTS "${PARSE_SOURCE_DIR}/${PARSE_CHECK_FILE}")
    message(STATUS "OBSERVE: Using ${NAME} from ${PARSE_SOURCE_DIR}")
    FetchContent_Declare(${NAME}
      SOURCE_DIR "${PARSE_SOURCE_DIR}"
      ${_observe_subdir_args}
      EXCLUDE_FROM_ALL
    )
  else ()
    if (EXISTS "${PARSE_SOURCE_DIR}")
      message(STATUS "OBSERVE: Ignoring incomplete ${NAME} at ${PARSE_SOURCE_DIR} (missing ${PARSE_CHECK_FILE})")
    endif ()
    message(STATUS "OBSERVE: Fetching ${NAME} from ${PARSE_GIT_REPOSITORY} (${PARSE_GIT_TAG})")
    FetchContent_Declare(${NAME}
      ${_observe_subdir_args}
      GIT_REPOSITORY ${PARSE_GIT_REPOSITORY}
      GIT_TAG ${PARSE_GIT_TAG}
      GIT_SHALLOW TRUE
      EXCLUDE_FROM_ALL
    )
  endif ()
  set(CMAKE_MESSAGE_LOG_LEVEL "${_observe_previous_message_log_level}")

  FetchContent_MakeAvailable(${NAME})

  string(TOLOWER "${NAME}" _observe_dependency_name)
  set(${_observe_dependency_name}_SOURCE_DIR "${${_observe_dependency_name}_SOURCE_DIR}" PARENT_SCOPE)
endfunction()

function(observe_remove_directory_compile_option DIRECTORY OPTION)
  get_property(OPTIONS DIRECTORY "${DIRECTORY}" PROPERTY COMPILE_OPTIONS)

  if (OPTIONS)
    list(REMOVE_ITEM OPTIONS "${OPTION}")
    set_property(DIRECTORY "${DIRECTORY}" PROPERTY COMPILE_OPTIONS "${OPTIONS}")
  endif ()

  get_property(TARGETS DIRECTORY "${DIRECTORY}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach (TARGET ${TARGETS})
    get_property(TARGET_OPTIONS TARGET "${TARGET}" PROPERTY COMPILE_OPTIONS)
    if (TARGET_OPTIONS)
      list(REMOVE_ITEM TARGET_OPTIONS "${OPTION}")
      set_property(TARGET "${TARGET}" PROPERTY COMPILE_OPTIONS "${TARGET_OPTIONS}")
    endif ()
  endforeach ()

  get_property(SUBDIRECTORIES DIRECTORY "${DIRECTORY}" PROPERTY SUBDIRECTORIES)
  foreach (SUBDIRECTORY ${SUBDIRECTORIES})
    observe_remove_directory_compile_option("${SUBDIRECTORY}" "${OPTION}")
  endforeach ()
endfunction()

function(observe_set_artifact_output TARGET PRODUCT FORMAT)
  set(_observe_output_dir "${OBSERVE_ARTIFACTS_DIR}/${PRODUCT}_artefacts/$<CONFIG>/${FORMAT}")
  set_target_properties(${TARGET} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${_observe_output_dir}"
    LIBRARY_OUTPUT_DIRECTORY "${_observe_output_dir}"
    RUNTIME_OUTPUT_DIRECTORY "${_observe_output_dir}"
  )
endfunction()

function(observe_fix_vst3_single_config_output TARGET)
  if (SMTG_WIN AND SMTG_CREATE_BUNDLE_FOR_WINDOWS AND NOT CMAKE_CONFIGURATION_TYPES)
    get_target_property(_observe_vst3_bin_dir ${TARGET} SMTG_PLUGIN_BINARY_DIR)
    get_target_property(_observe_vst3_package_name ${TARGET} SMTG_PLUGIN_PACKAGE_NAME)
    get_target_property(_observe_vst3_contents ${TARGET} SMTG_PLUGIN_PACKAGE_CONTENTS)
    get_target_property(_observe_vst3_architecture ${TARGET} SMTG_WIN_ARCHITECTURE_NAME)
    if (NOT _observe_vst3_bin_dir OR NOT _observe_vst3_package_name OR NOT _observe_vst3_contents OR NOT _observe_vst3_architecture)
      return()
    endif ()
    set_target_properties(${TARGET} PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY
        "${_observe_vst3_bin_dir}/${_observe_vst3_package_name}/${_observe_vst3_contents}/${_observe_vst3_architecture}"
    )
  endif ()
endfunction()

function(observe_set_vst3sdk_artifact_outputs DIRECTORY)
  get_property(_observe_targets DIRECTORY "${DIRECTORY}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach (_observe_target ${_observe_targets})
    get_target_property(_observe_target_type ${_observe_target} TYPE)
    if (_observe_target_type STREQUAL "EXECUTABLE")
      set_target_properties(${_observe_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>/Tools"
      )
    elseif (_observe_target_type STREQUAL "STATIC_LIBRARY" OR _observe_target_type STREQUAL "SHARED_LIBRARY")
      set_target_properties(${_observe_target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>/Libraries"
        LIBRARY_OUTPUT_DIRECTORY "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>/Libraries"
        RUNTIME_OUTPUT_DIRECTORY "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>/Libraries"
      )
    elseif (_observe_target_type STREQUAL "MODULE_LIBRARY")
      observe_fix_vst3_single_config_output(${_observe_target})
    endif ()
  endforeach ()

  get_property(_observe_subdirectories DIRECTORY "${DIRECTORY}" PROPERTY SUBDIRECTORIES)
  foreach (_observe_subdirectory ${_observe_subdirectories})
    observe_set_vst3sdk_artifact_outputs("${_observe_subdirectory}")
  endforeach ()
endfunction()

include(cmake/compile_flags.cmake)

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

add_subdirectory(modules/observe_file_embed)
add_subdirectory(modules/observe_graphics)
add_subdirectory(modules/observe_ui)
add_subdirectory(modules/observe_utils)
add_subdirectory(modules/observe_widgets)
add_subdirectory(modules/observe_windowing)
add_subdirectory(modules/observe_app)

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
  ObserveWindowing
)
if (OBSERVE_ENABLE_WIDGETS)
  target_link_libraries(observe PRIVATE ObserveWidgets)
endif ()

set_target_properties(observe PROPERTIES FOLDER "observe")

if ((OBSERVE_BUILD_VST3SDK_EXAMPLES OR OBSERVE_BUILD_VST3SDK_HOSTING_EXAMPLES) AND NOT TARGET sdk)
  set(SMTG_CUSTOM_BINARY_LOCATION "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>")
  observe_fetch_dependency(vst3sdk
    SOURCE_DIR "${OBSERVE_VST3SDK_SOURCE_DIR}"
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG master
  )
  if (MSVC)
    observe_remove_directory_compile_option("${OBSERVE_VST3SDK_SOURCE_DIR}" "$<$<CONFIG:Release>:/EHa>")
  endif ()
  observe_set_vst3sdk_artifact_outputs("${vst3sdk_SOURCE_DIR}")
  unset(SMTG_CUSTOM_BINARY_LOCATION)
endif ()

if (OBSERVE_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()

if (OBSERVE_BUILD_OSCILLATORS)
  add_subdirectory(apps/observe_noFFT)
endif ()
