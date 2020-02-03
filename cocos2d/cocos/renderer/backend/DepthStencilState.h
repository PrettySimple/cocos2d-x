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
 
#pragma once

#include "Macros.h"
#include "Types.h"

#include <stdint.h>

#include <cocos/base/CCRef.h>

CC_BACKEND_BEGIN

/**
 * @addtogroup _backend
 * @{
 */

/**
 * Stencil descriptor.
 */
struct StencilDescriptor
{
    bool operator ==(const StencilDescriptor& rhs) const;
    
    StencilOperation stencilFailureOperation = StencilOperation::KEEP;
    StencilOperation depthFailureOperation = StencilOperation::KEEP;
    StencilOperation depthStencilPassOperation = StencilOperation::KEEP;
    CompareFunction stencilCompareFunction = CompareFunction::ALWAYS;
    uint8_t readMask = 0;
    uint8_t writeMask = 0;
};

/**
 * Depth and stencil descriptor.
 */
struct DepthStencilDescriptor
{
    bool operator==(const DepthStencilDescriptor& rhs) const;
    
    CompareFunction depthCompareFunction = CompareFunction::LESS;
    bool depthWriteEnabled = false;
    bool depthTestEnabled = false;
    
    bool stencilTestEnabled = false;
    StencilDescriptor backFaceStencil;
    StencilDescriptor frontFaceStencil;
    
    std::size_t hash() const;
};

/**
 * Store Depth and stencil status.
 */
class DepthStencilState : public cocos2d::Ref
{
public:
    const DepthStencilDescriptor & depthStencilInfo() const { return _depthStencilInfo; }
protected:
    /**
     * @param descriptor Specifies depth and stencil descriptor.
     */
    DepthStencilState(const DepthStencilDescriptor& descriptor);
    virtual ~DepthStencilState();
    
    DepthStencilDescriptor _depthStencilInfo; ///< depth and stencil descriptor.
    bool _isBackFrontStencilEqual = false; ///< Does front stencil status equals to back stencil's.
};

// end of _backend group
/// @}
CC_BACKEND_END

namespace std {
template <>
struct hash<cocos2d::backend::DepthStencilDescriptor> {
    size_t operator()(cocos2d::backend::DepthStencilDescriptor const &v) const {
        return v.hash();
    }
};
} // namespace std
