/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
 
#include <cocos/platform/CCPlatformConfig.h>
#if defined(CC_USE_GL) || defined(CC_USE_GLES)
#include "DepthStencilStateGL.h"
#include "CommandBufferGL.h"
#include <cocos/platform/CCGL.h>

#include <cocos/base/ccMacros.h>
#include <cocos/renderer/backend/opengl/UtilsGL.h>

CC_BACKEND_BEGIN

DepthStencilStateGL::DepthStencilStateGL(const DepthStencilDescriptor& descriptor)
: DepthStencilState(descriptor)
{}

void DepthStencilStateGL::apply(unsigned int stencilReferenceValueFront, unsigned int stencilReferenceValueBack, CommandBufferGL* gl) const
{
    // depth test
    gl->setDepthTest(_depthStencilInfo.depthTestEnabled);
    gl->setDepthMask(_depthStencilInfo.depthWriteEnabled);
    gl->setDepthFunc(_depthStencilInfo.depthCompareFunction);
    gl->setStencilTest(_depthStencilInfo.stencilTestEnabled);

    // stencil test
    if (_depthStencilInfo.stencilTestEnabled)
    {
        if (_isBackFrontStencilEqual)
        {
            gl->setStencilFunc(_depthStencilInfo.frontFaceStencil.stencilCompareFunction,
                    stencilReferenceValueFront,
                    _depthStencilInfo.frontFaceStencil.readMask);
            gl->setStencilOp(_depthStencilInfo.frontFaceStencil.stencilFailureOperation,
                             _depthStencilInfo.frontFaceStencil.depthFailureOperation,
                             _depthStencilInfo.frontFaceStencil.depthStencilPassOperation);
            gl->setStencilMask(_depthStencilInfo.frontFaceStencil.writeMask);
        }
        else
        {
            gl->setStencilFuncSeparate(_depthStencilInfo.backFaceStencil.stencilCompareFunction,
                    stencilReferenceValueBack,
                    _depthStencilInfo.backFaceStencil.readMask,
                    _depthStencilInfo.frontFaceStencil.stencilCompareFunction,
                    stencilReferenceValueFront,
                    _depthStencilInfo.frontFaceStencil.readMask);

            gl->setStencilOpSeparate(_depthStencilInfo.backFaceStencil.stencilFailureOperation,
                                     _depthStencilInfo.backFaceStencil.depthFailureOperation,
                                     _depthStencilInfo.backFaceStencil.depthStencilPassOperation,
                                     _depthStencilInfo.frontFaceStencil.stencilFailureOperation,
                                     _depthStencilInfo.frontFaceStencil.depthFailureOperation,
                                     _depthStencilInfo.frontFaceStencil.depthStencilPassOperation);

            gl->setStencilMaskSeparate(_depthStencilInfo.backFaceStencil.writeMask,
                    _depthStencilInfo.frontFaceStencil.writeMask);
        }
    }
    
    CHECK_GL_ERROR_DEBUG();
}

CC_BACKEND_END
#endif
