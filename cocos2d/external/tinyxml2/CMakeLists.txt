set(TINYXML2_SRC
  tinyxml2.cpp
)

add_library(tinyxml2 STATIC
  ${TINYXML2_SRC}
)

set_target_properties(tinyxml2
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET tinyxml2 POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libtinyxml2.a"
                   "${LIB_PUBLISH_DIRECTORY}/libtinyxml2.a")
endif()
