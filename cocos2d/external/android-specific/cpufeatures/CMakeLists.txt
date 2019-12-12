cmake_minimum_required(VERSION 3.6)

set(lib_name cpufeatures)
set(target_name ext_${lib_name})

project(lib_name)

# CMake from android SDK set ANDROID_NDK instead of CMAKE_ANDROID_NDK
if(ANDROID_NDK)
    add_library(${target_name} ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    target_include_directories(${target_name} PUBLIC ${ANDROID_NDK}/sources/android/cpufeatures)
else()
    add_library(${target_name} ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    target_include_directories(${target_name} PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures)
endif()

target_link_libraries(${target_name} dl)

if(ANDROID)
    get_filename_component(LIB_PUBLISH_DIRECTORY
            "${CMAKE_SOURCE_DIR}/../../../build/built_libs/android/${CMAKE_BUILD_TYPE}/${ANDROID_ABI}"
            ABSOLUTE
            )

    file(MAKE_DIRECTORY ${LIB_PUBLISH_DIRECTORY})


    add_custom_command(
            TARGET ext_cpufeatures POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            copy "${CMAKE_BINARY_DIR}/engine/external/android-specific/cpufeatures/libext_cpufeatures.a" "${LIB_PUBLISH_DIRECTORY}/libext_cpufeatures.a"
            COMMENT "Moving libext_cpufeatures.a to output directory"
    )
endif()