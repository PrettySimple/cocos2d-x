cmake_minimum_required(VERSION 3.6)

set(lib_name unzip)
set(target_name ext_${lib_name})

project(${lib_name})

set(${target_name}_src
  ioapi.cpp
  unzip.cpp
  ioapi_mem.cpp
)

add_library(${target_name} STATIC
  ${${target_name}_src}
)

target_include_directories(${target_name} PUBLIC .)

if(MACOSX OR ANDROID OR WINDOWS)
  get_target_property(zlib_header ext_zlib INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories(${target_name} PRIVATE ${zlib_header}
)
endif()

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
)

if(ANDROID)
  get_filename_component(LIB_PUBLISH_DIRECTORY
          "${CMAKE_SOURCE_DIR}/../../../build/built_libs/android/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}"
          ABSOLUTE
          )

  file(MAKE_DIRECTORY ${LIB_PUBLISH_DIRECTORY})


  add_custom_command(
          TARGET ext_unzip POST_BUILD
          COMMAND "${CMAKE_COMMAND}" -E
          copy "${CMAKE_BINARY_DIR}/lib/libext_unzip.a" "${LIB_PUBLISH_DIRECTORY}/libext_unzip.a"
          COMMENT "Moving libext_unzip.a to output directory"
  )
endif()