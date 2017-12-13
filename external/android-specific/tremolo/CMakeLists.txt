set(VORBISIDEC_SRC
  Tremolo/bitwise.c
  Tremolo/codebook.c
  Tremolo/dsp.c
  Tremolo/floor0.c
  Tremolo/floor1.c
  Tremolo/floor_lookup.c
  Tremolo/framing.c
  Tremolo/mapping0.c
  Tremolo/mdct.c
  Tremolo/misc.c
  Tremolo/res012.c
  Tremolo/treminfo.c
  Tremolo/vorbisfile.c
)

add_library(vorbisidec STATIC
  ${VORBISIDEC_SRC}
)

set_target_properties(vorbisidec
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (ANDROID)
add_custom_command(TARGET vorbisidec POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libvorbisidec.a"
                   "${LIB_PUBLISH_DIRECTORY}/libvorbisidec.a")
endif()