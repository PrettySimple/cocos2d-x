cmake_minimum_required(VERSION 3.6)

set(lib_name tremolo)
set(target_name ext_${lib_name})

project(${lib_name})

set(${target_name}_src
    ./Tremolo/bitwise.c
    ./Tremolo/codebook.c
    ./Tremolo/dsp.c
    ./Tremolo/floor0.c
    ./Tremolo/floor1.c
    ./Tremolo/floor_lookup.c
    ./Tremolo/framing.c
    ./Tremolo/mapping0.c
    ./Tremolo/mdct.c
    ./Tremolo/misc.c
    ./Tremolo/res012.c
    ./Tremolo/treminfo.c
    ./Tremolo/vorbisfile.c
)

add_library(${target_name} STATIC
    ${${target_name}_src}
)

target_compile_definitions(${target_name} PRIVATE ONLY_C)

target_include_directories(${target_name} PUBLIC .)
set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    FOLDER "External"
    LINKER_LANGUAGE C
)

if(ANDROID)
    get_filename_component(LIB_PUBLISH_DIRECTORY
            "${CMAKE_SOURCE_DIR}/../../../build/built_libs/android/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}"
            ABSOLUTE
            )

    file(MAKE_DIRECTORY ${LIB_PUBLISH_DIRECTORY})


    add_custom_command(
            TARGET ext_tremolo POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            copy "${CMAKE_BINARY_DIR}/lib/libext_tremolo.a" "${LIB_PUBLISH_DIRECTORY}/libext_tremolo.a"
            COMMENT "Moving libext_tremolo.a to output directory"
    )
endif()