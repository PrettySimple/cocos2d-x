set(SOURCE CCModuleWEBP.cpp)

add_library(cocoswebp STATIC ${SOURCE})

target_link_libraries(cocoswebp webp cocos2d)

set_target_properties(cocoswebp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET cocoswebp POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libcocoswebp.a"
                   "${LIB_PUBLISH_DIRECTORY}/libcocoswebp.a")
endif()