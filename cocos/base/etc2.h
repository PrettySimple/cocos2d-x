/**
    @file   etc2.h
    @note   etc2 cpu decode
*/

#ifndef CC_BASE_ETC2_H
#define CC_BASE_ETC2_H

#include "platform/CCGL.h"

typedef unsigned char etc2_byte;
typedef int etc2_bool;
typedef unsigned int etc2_uint32;

#ifdef __cplusplus
extern "C"
{
#endif

    int etc2_decode_image(const unsigned char* pIn, const GLenum srcFormat, unsigned int width, unsigned int height, unsigned char** pOut);

#ifdef __cplusplus
}
#endif

#endif // CC_BASE_ETC2_H