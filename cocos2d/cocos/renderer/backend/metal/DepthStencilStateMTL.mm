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
 
#include "DepthStencilStateMTL.h"
#include "MetalState.hpp"

CC_BACKEND_BEGIN

state_helper::DepthStencilStateCache &DepthStencilStateMTL::_depthStencilStateCache(id<MTLDevice> mtlDevice)
{
    static state_helper::DepthStencilStateCache __depthStencilStateCache{mtlDevice};
    return __depthStencilStateCache;
}

DepthStencilStateMTL::DepthStencilStateMTL(id<MTLDevice> mtlDevice, const DepthStencilDescriptor& descriptor)
: DepthStencilState(descriptor)
{
    assert(!descriptor.stencilTestEnabled ||
           (descriptor.frontFaceStencil == descriptor.backFaceStencil)); // The current API does not support otherwise
        
    _mtlDepthStencilState = _depthStencilStateCache(mtlDevice).getOrCreateState(state_helper::DepthStencilState::convert(descriptor));
}

DepthStencilStateMTL::~DepthStencilStateMTL()
{
}

CC_BACKEND_END
