set(SOURCE CCModuleTIFF.cpp)

add_library(cocostiff STATIC ${SOURCE})

target_link_libraries(cocostiff tiff cocos2d)

set_target_properties(cocostiff
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET cocostiff POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libcocostiff.a"
                   "${LIB_PUBLISH_DIRECTORY}/libcocostiff.a")
endif()