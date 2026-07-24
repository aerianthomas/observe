file(GLOB EXAMPLES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */)

file(GLOB_RECURSE EXAMPLE_SHADERS shaders/[vf]s_*.sc)
add_compile_definitions(SHADERS_FOLDER=\"${CMAKE_CURRENT_SOURCE_DIR}/shaders\")

observe_embed_shaders(EmbeddedShaderResources "example_shaders.h" "resources::shaders" "${EXAMPLE_SHADERS}")
file(GLOB_RECURSE FONT_TTF_FILES fonts/*.ttf)
file(GLOB_RECURSE ICON_FILES icons/*.svg)
file(GLOB_RECURSE IMAGE_FILES images/*.png)

add_embedded_resources(EmbeddedFontResources "example_fonts.h" "resources::fonts" "${FONT_TTF_FILES}")
add_embedded_resources(EmbeddedIconResources "example_icons.h" "resources::icons" "${ICON_FILES}")
add_embedded_resources(EmbeddedImageResources "example_images.h" "resources::images" "${IMAGE_FILES}")

if (WIN32)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../modules/observe_graphics/bin/win32/shaderc.exe" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif (APPLE)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../modules/observe_graphics/bin/macos/shaderc" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else (UNIX)
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../modules/observe_graphics/bin/linux/shaderc" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

foreach (EXAMPLE ${EXAMPLES})
  file(GLOB SOURCE_FILES ${EXAMPLE}/*.cpp)
  file(GLOB HEADER_FILES ${EXAMPLE}/*.h)

  if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE}" AND SOURCE_FILES AND NOT EXAMPLE STREQUAL "ClapPlugin" AND NOT EXAMPLE STREQUAL "Vst3Plugin")
    add_executable(Example${EXAMPLE} main.cpp ${SOURCE_FILES} ${HEADER_FILES})

    if (WIN32)
      set_target_properties(Example${EXAMPLE} PROPERTIES WIN32_EXECUTABLE YES)
    elseif (APPLE)
      set(EXAMPLE_NAME Example${EXAMPLE})
      configure_file(Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE}/ExampleInfo.plist)
      set_target_properties(Example${EXAMPLE} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE}/ExampleInfo.plist
      )
    elseif (EMSCRIPTEN)
      target_link_options(Example${EXAMPLE}
        PRIVATE
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten_template.html
        -sGL_ENABLE_GET_PROC_ADDRESS
        -sALLOW_MEMORY_GROWTH
        --bind
        "-sEXPORTED_FUNCTIONS=['_main', '_pasteCallback']"
        "-sEXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'UTF8ToString']"
      )

      set_target_properties(Example${EXAMPLE} PROPERTIES
        SUFFIX ".html"
        OUTPUT_NAME "index"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/builds/${EXAMPLE}"
      )
    endif ()

    target_link_libraries(Example${EXAMPLE} PRIVATE
      observe
      EmbeddedShaderResources
      EmbeddedFontResources
      EmbeddedIconResources
      EmbeddedImageResources
    )
    observe_set_artifact_output(Example${EXAMPLE} ObserveExamples Standalone)
  endif ()
endforeach ()

if (NOT EMSCRIPTEN)
  set(CLAP_BUILD_TESTS OFF CACHE BOOL "Should CLAP build tests and the like?" FORCE)
  observe_fetch_dependency(clap
    SOURCE_DIR "${OBSERVE_CLAP_SOURCE_DIR}"
    GIT_REPOSITORY https://github.com/free-audio/clap.git
    GIT_TAG main
  )

  set(CLAP_HELPERS_BUILD_TESTS OFF CACHE BOOL "Build tests for the CLAP Helper" FORCE)
  set(CLAP_HELPERS_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "Resolve CLAP targets with CPM if not defined" FORCE)
  observe_fetch_dependency(clap_helpers
    SOURCE_DIR "${OBSERVE_CLAP_HELPERS_SOURCE_DIR}"
    GIT_REPOSITORY https://github.com/free-audio/clap-helpers.git
    GIT_TAG main
  )

  file(GLOB SOURCE_FILES ClapPlugin/*.cpp)
  file(GLOB HEADER_FILES ClapPlugin/*.h)
  add_library(ExampleClapPlugin MODULE ${SOURCE_FILES} ${HEADER_FILES})
  if (APPLE)
    set(EXAMPLE_NAME ExampleClapPlugin)
    configure_file(Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/ClapPlugin/ExampleInfo.plist)
    set_target_properties(ExampleClapPlugin PROPERTIES
      BUNDLE TRUE
      BUNDLE_EXTENSION clap
      OUTPUT_NAME "ExampleClapPlugin"
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/ClapPlugin/ExampleInfo.plist"
    )
  else ()
    set_target_properties(ExampleClapPlugin PROPERTIES SUFFIX ".clap" PREFIX "")
  endif ()

  target_link_libraries(ExampleClapPlugin PRIVATE
    observe
    EmbeddedShaderResources
    EmbeddedFontResources
    EmbeddedIconResources
    EmbeddedImageResources
    clap
    clap-helpers
  )
  observe_set_artifact_output(ExampleClapPlugin ObserveExamples CLAP)
endif ()

if (NOT EMSCRIPTEN)

  set(SMTG_CUSTOM_BINARY_LOCATION "${OBSERVE_ARTIFACTS_DIR}/VST3SDKExamples_artefacts/$<CONFIG>")
  observe_fetch_dependency(vst3sdk
    SOURCE_DIR "${OBSERVE_VST3SDK_SOURCE_DIR}"
    GIT_REPOSITORY https://github.com/steinbergmedia/vst3sdk.git
    GIT_TAG master
  )
  if (MSVC)
    observe_remove_directory_compile_option("${OBSERVE_VST3SDK_SOURCE_DIR}" "$<$<CONFIG:Release>:/EHa>")
  endif ()
  observe_set_vst3sdk_artifact_outputs("${OBSERVE_VST3SDK_SOURCE_DIR}")

  # Add VST3 CMake modules (required for smtg_* helpers)
  list(APPEND CMAKE_MODULE_PATH
    "${OBSERVE_VST3SDK_SOURCE_DIR}/cmake/modules"
  )

  #include(SMTG_AddVST3Library)  #not needed?

  # ------------------------------------------------------------------
  # Plugin sources
  # ------------------------------------------------------------------
  file(GLOB VST3_SOURCE_FILES Vst3Plugin/*.cpp)
  file(GLOB VST3_HEADER_FILES Vst3Plugin/*.h)

  # Optional helper source (kept from original design)
  list(APPEND VST3_SOURCE_FILES
    ${OBSERVE_VST3SDK_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
  )

  list(APPEND VST3_HEADER_FILES
    ${OBSERVE_VST3SDK_SOURCE_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.h
  )

  # ------------------------------------------------------------------
  # Plugin target
  # ------------------------------------------------------------------
  set(SMTG_CUSTOM_BINARY_LOCATION "${OBSERVE_ARTIFACTS_DIR}/ObserveExamples_artefacts/$<CONFIG>")
  smtg_add_vst3plugin(
    ExampleVst3Plugin
    ${VST3_SOURCE_FILES}
    ${VST3_HEADER_FILES}
  )
  observe_fix_vst3_single_config_output(ExampleVst3Plugin)

  target_link_libraries(ExampleVst3Plugin PRIVATE
    observe
    EmbeddedShaderResources
    EmbeddedFontResources
    EmbeddedIconResources
    EmbeddedImageResources
    sdk
  )

  # Version file generation
  smtg_target_configure_version_file(ExampleVst3Plugin)

  # Ensure generated headers are visible
  target_include_directories(ExampleVst3Plugin PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
  )

  # ------------------------------------------------------------------
  # macOS bundle settings
  # ------------------------------------------------------------------
  if (SMTG_MAC)
    smtg_target_set_bundle(ExampleVst3Plugin
      BUNDLE_IDENTIFIER dev.observe.example.vst3plugin
      COMPANY_NAME "Observe"
    )

    smtg_target_set_debug_executable(ExampleVst3Plugin
      "/Applications/Studio One 7.app"
      "--pluginfolders=$(BUILT_PRODUCTS_DIR)"
    )
  endif ()

endif ()
