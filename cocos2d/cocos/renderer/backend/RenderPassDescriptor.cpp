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
 
#include "RenderPassDescriptor.h"

CC_BACKEND_BEGIN

RenderPassDescriptor& RenderPassDescriptor::operator=(const RenderPassDescriptor& descriptor)
{
    clearDepthValue = descriptor.clearDepthValue;
    clearStencilValue = descriptor.clearStencilValue;
    clearColorValue = descriptor.clearColorValue;
    needColorAttachment = descriptor.needColorAttachment;
    depthTestEnabled = descriptor.depthTestEnabled;
    stencilTestEnabled = descriptor.stencilTestEnabled;
    needClearColor = descriptor.needClearColor;
    needClearDepth = descriptor.needClearDepth;
    needClearStencil = descriptor.needClearStencil;
    depthAttachmentTexture = descriptor.depthAttachmentTexture;
    stencilAttachmentTexture = descriptor.stencilAttachmentTexture;
    colorAttachmentsTexture[0] = descriptor.colorAttachmentsTexture[0];
    
    return *this;
}

bool RenderPassDescriptor::operator==(const RenderPassDescriptor& descriptor) const
{
    if( clearDepthValue == descriptor.clearDepthValue &&
        clearStencilValue == descriptor.clearStencilValue &&
        clearColorValue == descriptor.clearColorValue &&
        needColorAttachment == descriptor.needColorAttachment &&
        depthTestEnabled == descriptor.depthTestEnabled &&
        stencilTestEnabled == descriptor.stencilTestEnabled &&
        needClearColor == descriptor.needClearColor &&
        needClearDepth == descriptor.needClearDepth &&
        needClearStencil == descriptor.needClearStencil &&
        depthAttachmentTexture == descriptor.depthAttachmentTexture &&
        stencilAttachmentTexture == descriptor.stencilAttachmentTexture &&
        colorAttachmentsTexture[0] == descriptor.colorAttachmentsTexture[0])
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool RenderPassDescriptor::equalExceptClear(const RenderPassDescriptor& descriptor) const
{
    if( needColorAttachment == descriptor.needColorAttachment &&
        depthTestEnabled == descriptor.depthTestEnabled &&
        stencilTestEnabled == descriptor.stencilTestEnabled &&
        depthAttachmentTexture == descriptor.depthAttachmentTexture &&
        stencilAttachmentTexture == descriptor.stencilAttachmentTexture &&
        colorAttachmentsTexture[0] == descriptor.colorAttachmentsTexture[0])
    {
        return true;
    }
    else
    {
        return false;
    }
}


std::string RenderPassDescriptor::diff(const RenderPassDescriptor& descriptor) const
{
    std::string ret{""};
    
    for(auto &d : {
        std::make_tuple("depthTestEnabled",
                        depthTestEnabled != descriptor.depthTestEnabled,
                        std::to_string(depthTestEnabled),
                        std::to_string(descriptor.depthTestEnabled)),
        std::make_tuple("stencilTestEnabled",
                        stencilTestEnabled != descriptor.stencilTestEnabled,
                        std::to_string(stencilTestEnabled),
                        std::to_string(descriptor.stencilTestEnabled)),
        std::make_tuple("depthAttachmentTexture",
                        depthAttachmentTexture != descriptor.depthAttachmentTexture,
                        std::to_string(reinterpret_cast<unsigned long>(depthAttachmentTexture)),
                        std::to_string(reinterpret_cast<unsigned long>(descriptor.depthAttachmentTexture))),
        std::make_tuple("stencilAttachmentTexture",
                        stencilAttachmentTexture != descriptor.stencilAttachmentTexture,
                        std::to_string(reinterpret_cast<unsigned long>(stencilAttachmentTexture)),
                        std::to_string(reinterpret_cast<unsigned long>(descriptor.stencilAttachmentTexture))),
        std::make_tuple("colorAttachmentsTexture[0]",
                        colorAttachmentsTexture[0] != descriptor.colorAttachmentsTexture[0],
                        std::to_string(reinterpret_cast<unsigned long>(colorAttachmentsTexture[0])),
                        std::to_string(reinterpret_cast<unsigned long>(descriptor.colorAttachmentsTexture[0])))
    })
        if (std::get<1>(d))
            ret += std::string{std::get<0>(d)} + " (" + std::get<2>(d) + " -> " + std::get<3>(d) + "), ";
    
    auto first = true;
    for(auto &d : {
        std::make_tuple("clearDepthValue",
                        clearDepthValue != descriptor.clearDepthValue,
                        std::to_string(clearDepthValue),
                        std::to_string(descriptor.clearDepthValue)),
        std::make_tuple("clearStencilValue",
                        clearStencilValue != descriptor.clearStencilValue,
                        std::to_string(clearStencilValue),
                        std::to_string(descriptor.clearStencilValue)),
        std::make_tuple("clearColorValue R",
                        clearColorValue.at(0) != descriptor.clearColorValue.at(0),
                        std::to_string(clearColorValue.at(0)),
                        std::to_string(descriptor.clearColorValue.at(0))),
        std::make_tuple("clearColorValue G",
                        clearColorValue.at(1) != descriptor.clearColorValue.at(1),
                        std::to_string(clearColorValue.at(1)),
                        std::to_string(descriptor.clearColorValue.at(1))),
        std::make_tuple("clearColorValue B",
                        clearColorValue.at(2) != descriptor.clearColorValue.at(2),
                        std::to_string(clearColorValue.at(2)),
                        std::to_string(descriptor.clearColorValue.at(2))),
        std::make_tuple("clearColorValue A",
                        clearColorValue.at(3) != descriptor.clearColorValue.at(3),
                        std::to_string(clearColorValue.at(3)),
                        std::to_string(descriptor.clearColorValue.at(3))),
        std::make_tuple("needClearColor",
                       needClearColor != descriptor.needClearColor,
                       std::to_string(needClearColor),
                       std::to_string(descriptor.needClearColor)),
       std::make_tuple("needClearDepth",
                       needClearDepth != descriptor.needClearDepth,
                       std::to_string(needClearDepth),
                       std::to_string(descriptor.needClearDepth)),
       std::make_tuple("needClearStencil",
                       needClearStencil != descriptor.needClearStencil,
                       std::to_string(needClearStencil),
                       std::to_string(descriptor.needClearStencil)),
    })
        if (std::get<1>(d)) {
            if (first) {
                ret += " FYI: ";
                first = false;
            }
            ret += std::string{std::get<0>(d)} + " (" + std::get<2>(d) + " -> " + std::get<3>(d) + "), ";
        }
          
    return ret;
}

CC_BACKEND_END
