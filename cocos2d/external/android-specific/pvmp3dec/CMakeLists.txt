set(PVMP3DEC_SRC
  src/pvmp3_normalize.cpp
  src/pvmp3_alias_reduction.cpp
  src/pvmp3_crc.cpp
  src/pvmp3_decode_header.cpp
  src/pvmp3_decode_huff_cw.cpp
  src/pvmp3_getbits.cpp
  src/pvmp3_dequantize_sample.cpp
  src/pvmp3_framedecoder.cpp
  src/pvmp3_get_main_data_size.cpp
  src/pvmp3_get_side_info.cpp
  src/pvmp3_get_scale_factors.cpp
  src/pvmp3_mpeg2_get_scale_data.cpp
  src/pvmp3_mpeg2_get_scale_factors.cpp
  src/pvmp3_mpeg2_stereo_proc.cpp
  src/pvmp3_huffman_decoding.cpp
  src/pvmp3_huffman_parsing.cpp
  src/pvmp3_tables.cpp
  src/pvmp3_imdct_synth.cpp
  src/pvmp3_mdct_6.cpp
  src/pvmp3_dct_6.cpp
  src/pvmp3_poly_phase_synthesis.cpp
  src/pvmp3_equalizer.cpp
  src/pvmp3_seek_synch.cpp
  src/pvmp3_stereo_proc.cpp
  src/pvmp3_reorder.cpp
)

if(${ANDROID_ABI} STREQUAL "arm")
set(ARCH_SRC
  src/asm/pvmp3_polyphase_filter_window_gcc.s
  src/asm/pvmp3_mdct_18_gcc.s
  src/asm/pvmp3_dct_9_gcc.s
  src/asm/pvmp3_dct_16_gcc.s
  )
else()
set(ARCH_SRC
  src/pvmp3_polyphase_filter_window.cpp
  src/pvmp3_mdct_18.cpp
  src/pvmp3_dct_9.cpp
  src/pvmp3_dct_16.cpp
  )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(pvmp3dec STATIC
  ${PVMP3DEC_SRC}
  ${ARCH_SRC}
)

set_target_properties(pvmp3dec
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D\"OSCL_UNUSED_ARG(x)=(void)(x)\"")

if (ANDROID)
add_custom_command(TARGET pvmp3dec POST_BUILD
                   COMMAND "${CMAKE_COMMAND}" -E
                   copy "${CMAKE_BINARY_DIR}/lib/libpvmp3dec.a"
                   "${LIB_PUBLISH_DIRECTORY}/libpvmp3dec.a")
endif()