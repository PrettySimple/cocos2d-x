set(UNZIP_SRC
  ioapi.cpp
  unzip.cpp
  ioapi_mem.cpp
)

add_library(unzip STATIC
  ${UNZIP_SRC}
)

set_target_properties(unzip
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET unzip POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libunzip.a"
                   "${LIB_PUBLISH_DIRECTORY}/libunzip.a")
endif()