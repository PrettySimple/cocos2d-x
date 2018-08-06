/******************************************************************************

 @File         PVRTDecompress.h

 @Title

 @Copyright    Copyright (C) 2000 - 2008 by Imagination Technologies Limited.

 @Platform     ANSI compatible

 @Description  PVRTC Texture Decompression.

 ******************************************************************************/

#ifndef CC_BASE_PVR_H
#define CC_BASE_PVR_H

int PVRTDecompressPVRTC(const void* pCompressedData, int Do2bitMode, int XDim, int YDim, unsigned char* pResultImage);

#endif // CC_BASE_PVR_H
