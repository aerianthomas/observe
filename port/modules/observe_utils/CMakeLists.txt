if (WIN32)
  file(GLOB PLATFORM_HEADER_FILES win32/*.h)
  file(GLOB PLATFORM_SOURCE_FILES win32/*.cpp)
else ()
  file(GLOB PLATFORM_HEADER_FILES posix/*.h)
  file(GLOB PLATFORM_SOURCE_FILES posix/*.cpp)
endif ()

file(GLOB HEADERS *.h ${PLATFORM_HEADER_FILES})
file(GLOB SOURCE_FILES *.cpp ${PLATFORM_SOURCE_FILES})

if (OBSERVE_AMALGAMATED_BUILD)
  amalgamate_sources(AMALGAMATED ${CMAKE_CURRENT_BINARY_DIR}/observe_utils_amalgamated "${SOURCE_FILES}")
  list(APPEND SOURCE_FILES ${AMALGAMATED})
endif ()

amalgamate_headers(${OBSERVE_INCLUDE}/observe/utils.h "${HEADERS}")
list(APPEND HEADERS ${OBSERVE_INCLUDE}/observe/utils.h)

add_library(ObserveUtils OBJECT ${SOURCE_FILES} ${HEADERS})
target_include_directories(ObserveUtils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(ObserveUtilsDefinitions INTERFACE)
set_target_properties(ObserveUtils PROPERTIES FOLDER "observe")