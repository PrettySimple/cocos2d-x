/******************************************************************************
 
 @File         PVRTDecompress.h
 
 @Title
 
 @Copyright    Copyright (C) 2000 - 2008 by Imagination Technologies Limited.
 
 @Platform     ANSI compatible
 
 @Description  PVRTC Texture Decompression.
 
 ******************************************************************************/

#pragma once

int PVRTDecompressPVRTC(const void * const pCompressedData,const int XDim,const int YDim,void *pDestData,const bool Do2bitMode);

