set(XXHASH
  xxhash.c
)

add_library(xxhash STATIC
  ${XXHASH}
)

set_target_properties(xxhash
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET xxhash POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libxxhash.a"
                   "${LIB_PUBLISH_DIRECTORY}/libxxhash.a")
endif()