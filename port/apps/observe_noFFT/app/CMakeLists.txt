set(CMAKE_VS_JUST_MY_CODE_DEBUGGING OFF CACHE BOOL "Disable Visual Studio Just My Code /JMC instrumentation")

if (MSVC)
  add_compile_options(/JMC-)
endif ()

if (NOT TARGET observe)
  message(FATAL_ERROR "The observe_noFFT app must be added from the observe root so the 'observe' target exists.")
endif ()

set(OBSERVE_APP_DEPENDENCIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../external" CACHE PATH "Dependency checkout root for observe apps")
set(OBSERVE_APP_CLAP_SOURCE_DIR "${OBSERVE_APP_DEPENDENCIES_DIR}/clap" CACHE PATH "CLAP SDK source directory")
set(OBSERVE_APP_CLAP_HELPERS_SOURCE_DIR "${OBSERVE_APP_DEPENDENCIES_DIR}/clap-helpers" CACHE PATH "CLAP helpers source directory")
set(OBSERVE_APP_VST3SDK_SOURCE_DIR "${OBSERVE_APP_DEPENDENCIES_DIR}/vst3sdk" CACHE PATH "VST3 SDK source directory")

function(observe_app_add_dependency NAME SOURCE_DIR GIT_REPOSITORY GIT_TAG)
  if (COMMAND observe_fetch_dependency)
    observe_fetch_dependency(${NAME}
      SOURCE_DIR "${SOURCE_DIR}"
      GIT_REPOSITORY ${GIT_REPOSITORY}
      GIT_TAG ${GIT_TAG}
    )
  elseif (EXISTS "${SOURCE_DIR}/CMakeLists.txt")
    add_subdirectory("${SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/${NAME}-build")
  else ()
    message(FATAL_ERROR "${NAME} not found at '${SOURCE_DIR}'.")
  endif ()
endfunction()

# Portable subset of the Oscillators C++ implementation (scalar reference
# classes; the vectorized banks depend on Apple's Accelerate framework).
set(OSC_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/oscillators")
add_library(oscillators_cpp STATIC
  "${OSC_CPP_DIR}/Phasor.cpp"
  "${OSC_CPP_DIR}/Oscillator.cpp"
  "${OSC_CPP_DIR}/Resonator.cpp"
  "${OSC_CPP_DIR}/TrackingResonator.cpp"
)
target_include_directories(oscillators_cpp PUBLIC "${OSC_CPP_DIR}")
set_target_properties(oscillators_cpp PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(oscillators_observe_core STATIC
  resonate_view.cpp
  resonate_view.h
)
target_link_libraries(oscillators_observe_core PUBLIC observe oscillators_cpp)
set_target_properties(oscillators_observe_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# miniaudio: single-header, public-domain audio capture/playback (WASAPI on
# Windows, no link dependencies).
if (OSCILLATORS_BUILD_OBSERVE_APP)
  # miniaudio: single-header, public-domain audio capture/playback (WASAPI on
  # Windows, no link dependencies).
  observe_fetch_dependency(miniaudio
    SOURCE_DIR "${OBSERVE_MINIAUDIO_SOURCE_DIR}"
    CHECK_FILE miniaudio.h
    SOURCE_SUBDIR _observe_header_only
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
  )

  add_executable(OscillatorsObserveApp main.cpp)
  target_compile_definitions(OscillatorsObserveApp PRIVATE MINIAUDIO_IMPLEMENTATION)
  target_include_directories(OscillatorsObserveApp PRIVATE "${miniaudio_SOURCE_DIR}")
  target_link_libraries(OscillatorsObserveApp PRIVATE oscillators_observe_core)
  observe_set_artifact_output(OscillatorsObserveApp OscillatorsObserve Standalone)

  if (WIN32)
    set_target_properties(OscillatorsObserveApp PROPERTIES WIN32_EXECUTABLE YES)
  endif ()
endif ()

if (OSCILLATORS_BUILD_CLAP AND NOT EMSCRIPTEN)
  if (NOT TARGET clap)
    set(CLAP_BUILD_TESTS OFF CACHE BOOL "Should CLAP build tests and the like?" FORCE)
    observe_app_add_dependency(clap "${OBSERVE_APP_CLAP_SOURCE_DIR}" https://github.com/free-audio/clap.git main)
  endif ()

  if (NOT TARGET clap-helpers)
    set(CLAP_HELPERS_BUILD_TESTS OFF CACHE BOOL "Build tests for the CLAP Helper" FORCE)
    set(CLAP_HELPERS_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "Resolve CLAP targets with CPM if not defined" FORCE)
    observe_app_add_dependency(clap_helpers "${OBSERVE_APP_CLAP_HELPERS_SOURCE_DIR}" https://github.com/free-audio/clap-helpers.git main)
  endif ()

  add_library(oscillators_observe_plugin_impl STATIC
    clap_plugin.cpp
    clap_plugin.h
    clap_entry_impl.cpp
    clap_entry_impl.h
  )
  target_link_libraries(oscillators_observe_plugin_impl PUBLIC
    oscillators_observe_core
    clap
    clap-helpers
  )
  set_target_properties(oscillators_observe_plugin_impl PROPERTIES POSITION_INDEPENDENT_CODE ON)

  add_library(OscillatorsObserveClap MODULE plugin_entry.cpp)
  target_link_libraries(OscillatorsObserveClap PRIVATE oscillators_observe_plugin_impl)
  observe_set_artifact_output(OscillatorsObserveClap OscillatorsObserve CLAP)

  if (APPLE)
    set_target_properties(OscillatorsObserveClap PROPERTIES
      BUNDLE TRUE
      BUNDLE_EXTENSION clap
      OUTPUT_NAME "OscillatorsObserveClap"
    )
  else ()
    set_target_properties(OscillatorsObserveClap PROPERTIES SUFFIX ".clap" PREFIX "")
  endif ()
endif ()

if (OSCILLATORS_BUILD_VST3 AND NOT EMSCRIPTEN)
  if (NOT TARGET sdk)
    set(SMTG_CUSTOM_BINARY_LOCATION "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>")
    observe_app_add_dependency(vst3sdk "${OBSERVE_APP_VST3SDK_SOURCE_DIR}" https://github.com/steinbergmedia/vst3sdk.git master)
    if (MSVC AND COMMAND observe_remove_directory_compile_option)
      observe_remove_directory_compile_option("${OBSERVE_APP_VST3SDK_SOURCE_DIR}" "$<$<CONFIG:Release>:/EHa>")
    endif ()
    observe_set_vst3sdk_artifact_outputs("${OBSERVE_APP_VST3SDK_SOURCE_DIR}")
  endif ()

  list(APPEND CMAKE_MODULE_PATH "${OBSERVE_APP_VST3SDK_SOURCE_DIR}/cmake/modules")
  set(SMTG_CUSTOM_BINARY_LOCATION "${OBSERVE_ARTIFACTS_DIR}/OscillatorsObserve_artefacts/$<CONFIG>")

  smtg_add_vst3plugin(OscillatorsObserveVst3
    vst3_plugin.cpp
    vst3_plugin.h
    vst3_entry.cpp
    ${OBSERVE_APP_VST3SDK_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
    ${OBSERVE_APP_VST3SDK_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.h
  )
  observe_fix_vst3_single_config_output(OscillatorsObserveVst3)

  target_link_libraries(OscillatorsObserveVst3 PRIVATE
    oscillators_observe_core
    sdk
  )

  smtg_target_configure_version_file(OscillatorsObserveVst3)
  target_include_directories(OscillatorsObserveVst3 PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

  if (SMTG_MAC)
    smtg_target_set_bundle(OscillatorsObserveVst3
      BUNDLE_IDENTIFIER dev.oscillators.observe.spectrum
      COMPANY_NAME "Oscillators"
    )
  endif ()
endif ()
