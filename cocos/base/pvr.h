/******************************************************************************
 
 @File         PVRTDecompress.h
 
 @Title
 
 @Copyright    Copyright (C) 2000 - 2008 by Imagination Technologies Limited.
 
 @Platform     ANSI compatible
 
 @Description  PVRTC Texture Decompression.
 
 ******************************************************************************/

#ifndef __PVR_H__
#define __PVR_H__


int PVRTDecompressPVRTC(const void* pCompressedData,
                        int Do2bitMode,
                        int XDim,
                        int YDim,
                        unsigned char* pResultImage);


#endif //__PVR_H__
