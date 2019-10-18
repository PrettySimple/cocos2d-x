set(SOURCE CCModuleJPEG.cpp)

add_library(cocosjpeg STATIC ${SOURCE})

target_link_libraries(cocosjpeg jpeg cocos2d)

set_target_properties(cocosjpeg
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET cocosjpeg POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libcocosjpeg.a"
                   "${LIB_PUBLISH_DIRECTORY}/libcocosjpeg.a")
endif()