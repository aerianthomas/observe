if (OBSERVE_ENABLE_WIDGETS)
  file(GLOB HEADERS *.h)
  file(GLOB SOURCE_FILES *.cpp)
  if (OBSERVE_AMALGAMATED_BUILD)
    amalgamate_sources(AMALGAMATED ${CMAKE_CURRENT_BINARY_DIR}/observe_widgets_amalgamated "${SOURCE_FILES}")
    list(APPEND SOURCE_FILES ${AMALGAMATED})
  endif ()

  amalgamate_headers(${OBSERVE_INCLUDE}/observe/widgets.h "${HEADERS}")
  list(APPEND HEADERS ${OBSERVE_INCLUDE}/observe/widgets.h)

  add_library(ObserveWidgets OBJECT ${SOURCE_FILES} ${HEADERS})

  target_include_directories(ObserveWidgets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OBSERVE_INCLUDE_PATH})
  target_link_libraries(ObserveWidgets PRIVATE ObserveGraphicsEmbeds)
  set_target_properties(ObserveWidgets PROPERTIES FOLDER "observe")
endif ()
