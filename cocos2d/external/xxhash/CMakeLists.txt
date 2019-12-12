cmake_minimum_required(VERSION 3.6)

set(lib_name xxhash)
set(target_name ext_${lib_name})

project(${lib_name})

add_library(${target_name} STATIC
  xxhash.c
)

target_include_directories(${target_name} PUBLIC .)

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
            TARGET ext_xxhash POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            copy "${CMAKE_BINARY_DIR}/lib/libext_xxhash.a" "${LIB_PUBLISH_DIRECTORY}/libext_xxhash.a"
            COMMENT "Moving libext_xxhash.a to output directory"
    )
endif()