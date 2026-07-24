file(GLOB HEADERS *.h)
file(GLOB SOURCE_FILES *.cpp)
if (OBSERVE_AMALGAMATED_BUILD)
  amalgamate_sources(AMALGAMATED ${CMAKE_CURRENT_BINARY_DIR}/observe_app_amalgamated "${SOURCE_FILES}")
  list(APPEND SOURCE_FILES ${AMALGAMATED})
endif ()

amalgamate_headers(${OBSERVE_INCLUDE}/observe/app.h "${HEADERS}")
list(APPEND HEADERS ${OBSERVE_INCLUDE}/observe/app.h)

add_library(ObserveApp OBJECT ${SOURCE_FILES} ${HEADERS})
target_include_directories(ObserveApp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OBSERVE_INCLUDE_PATH})
set_target_properties(ObserveApp PROPERTIES FOLDER "observe")