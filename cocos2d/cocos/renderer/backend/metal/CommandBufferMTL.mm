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
 
#include "CommandBufferMTL.h"
#include "BufferMTL.h"
#include "DeviceMTL.h"
#include "RenderPipelineMTL.h"
#include "TextureMTL.h"
#include "Utils.h"
#include "../Macros.h"
#include "BufferManager.h"
#include "DepthStencilStateMTL.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
#include "cocos/base/CCDirector.h"
#endif

CC_BACKEND_BEGIN

namespace
{
    MTLWinding toMTLWinding(Winding winding)
    {
        if (Winding::CLOCK_WISE == winding)
            return MTLWindingClockwise;
        else
            return MTLWindingCounterClockwise;
    }

    MTLPrimitiveType toMTLPrimitive(PrimitiveType primitiveType)
    {
        MTLPrimitiveType ret = MTLPrimitiveTypeTriangle;
        switch (primitiveType)
        {
            case PrimitiveType::POINT:
                ret = MTLPrimitiveTypePoint;
                break;
            case PrimitiveType::LINE:
                ret = MTLPrimitiveTypeLine;
                break;
            case PrimitiveType::LINE_STRIP:
                ret = MTLPrimitiveTypeLineStrip;
                break;
            case PrimitiveType::TRIANGLE:
                ret = MTLPrimitiveTypeTriangle;
                break;
            case PrimitiveType::TRIANGLE_STRIP:
                ret = MTLPrimitiveTypeTriangleStrip;
                break;
            default:
                break;
        }
        return ret;
    }
    
    MTLIndexType toMTLIndexType(IndexFormat indexFormat)
    {
        if (IndexFormat::U_SHORT == indexFormat)
            return MTLIndexTypeUInt16;
        else
            return MTLIndexTypeUInt32;
    }
    
    MTLCullMode toMTLCullMode(CullMode mode)
    {
        switch (mode) {
            case CullMode::NONE:
                return MTLCullModeNone;
            case CullMode::FRONT:
                return MTLCullModeFront;
            case CullMode::BACK:
                return MTLCullModeBack;
        }
    }
    
    id<MTLTexture> getMTLTexture(TextureBackend* texture)
    {
        switch (texture->getTextureType())
        {
            case TextureType::TEXTURE_2D:
                return static_cast<TextureMTL*>(texture)->getMTLTexture();
            case TextureType::TEXTURE_CUBE:
                return static_cast<TextureCubeMTL*>(texture)->getMTLTexture();
            default:
                assert(false);
                return nil;
        }
    }
    
    id<MTLSamplerState> getMTLSamplerState(TextureBackend* texture)
    {
        switch (texture->getTextureType())
        {
            case TextureType::TEXTURE_2D:
                return static_cast<TextureMTL*>(texture)->getMTLSamplerState();
            case TextureType::TEXTURE_CUBE:
                return static_cast<TextureCubeMTL*>(texture)->getMTLSamplerState();
            default:
                assert(false);
                return nil;
        }
    }
}

CommandBufferMTL::CommandBufferMTL(DeviceMTL* deviceMTL)
: _mtlCommandQueue(deviceMTL->getMTLCommandQueue())
, _frameBoundarySemaphore(dispatch_semaphore_create(MAX_INFLIGHT_BUFFER))
{
}

CommandBufferMTL::~CommandBufferMTL()
{
    dispatch_semaphore_signal(_frameBoundarySemaphore);
}

void CommandBufferMTL::beginFrame()
{
    _autoReleasePool = [[NSAutoreleasePool alloc] init];
    dispatch_semaphore_wait(_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    _mtlCommandBuffer = [_mtlCommandQueue commandBuffer];
    [_mtlCommandBuffer enqueue];
    [_mtlCommandBuffer retain];

    BufferManager::beginFrame();
}

void CommandBufferMTL::beginRenderPass(const RenderPassDescriptor& descriptor)
{
    auto renderPassDescriptorCopy = descriptor;
    if (_clearNextCommand) {
        renderPassDescriptorCopy.needClearColor = true;
        renderPassDescriptorCopy.clearColorValue = {
            _clearNextCommandColorV.r,
            _clearNextCommandColorV.g,
            _clearNextCommandColorV.b,
            _clearNextCommandColorV.a
        };
        renderPassDescriptorCopy.needClearDepth= true;
        renderPassDescriptorCopy.clearDepthValue = _clearNextCommandDepthV;
        renderPassDescriptorCopy.needClearStencil = true;
        renderPassDescriptorCopy.clearStencilValue = _clearNextCommandStencilV;

        _clearNextCommand = false;
    }

    auto rc = _mtlRenderCommandEncoderCache(_mtlCommandBuffer).getOrCreateState(renderPassDescriptorCopy);
    _mtlRenderEncoder = rc.renderCommandEncoder;
    
    auto mtlDescriptor = rc.renderPassDecriptor;
    _renderTargetWidth = (unsigned int)mtlDescriptor.colorAttachments[0].texture.width;
    _renderTargetHeight = (unsigned int)mtlDescriptor.colorAttachments[0].texture.height;
    if (mtlDescriptor.stencilAttachment &&
        mtlDescriptor.stencilAttachment.texture) {
        _renderTargetWidth = std::min(_renderTargetWidth,
                                      (unsigned int)mtlDescriptor.stencilAttachment.texture.width);
        _renderTargetHeight = std::min(_renderTargetHeight,
                                       (unsigned int)mtlDescriptor.stencilAttachment.texture.height);
    }

    //    [_mtlRenderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
}

void CommandBufferMTL::setRenderPipeline(RenderPipeline* renderPipeline)
{
    CC_SAFE_RETAIN(renderPipeline);
    CC_SAFE_RELEASE(_renderPipelineMTL);
    _renderPipelineMTL = static_cast<RenderPipelineMTL*>(renderPipeline);
    if (_renderPipelineMTL->getMTLRenderPipelineState())
        [_mtlRenderEncoder setRenderPipelineState:_renderPipelineMTL->getMTLRenderPipelineState()];
}

void CommandBufferMTL::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    MTLViewport viewport;
    viewport.originX = x;
    viewport.originY = (int)(_renderTargetHeight - y - h);
    viewport.width = w;
    viewport.height = h;
    viewport.znear = -1;
    viewport.zfar = 1;
    [_mtlRenderEncoder setViewport:viewport];
}

void CommandBufferMTL::setCullMode(CullMode mode)
{
    [_mtlRenderEncoder setCullMode:toMTLCullMode(mode)];
}

void CommandBufferMTL::setWinding(Winding winding)
{
    [_mtlRenderEncoder setFrontFacingWinding:toMTLWinding(winding)];
}

void CommandBufferMTL::setVertexBuffer(Buffer* buffer)
{
    // Vertex buffer is bound in index 0.
    [_mtlRenderEncoder setVertexBuffer:static_cast<BufferMTL*>(buffer)->getMTLBuffer()
                                offset:0
                               atIndex:0];
}

void CommandBufferMTL::setProgramState(ProgramState* programState)
{
    CC_SAFE_RETAIN(programState);
    CC_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferMTL::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (!buffer)
        return;
    
    _mtlIndexBuffer = static_cast<BufferMTL*>(buffer)->getMTLBuffer();
    [_mtlIndexBuffer retain];
}

void CommandBufferMTL::drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)
{
    prepareDrawing();
    [_mtlRenderEncoder drawPrimitives:toMTLPrimitive(primitiveType)
                          vertexStart:start
                          vertexCount:count];
}

void CommandBufferMTL::drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)
{
    if (count <= 0)
        return;
    prepareDrawing();
    [_mtlRenderEncoder drawIndexedPrimitives:toMTLPrimitive(primitiveType)
                                  indexCount:count
                                   indexType:toMTLIndexType(indexType)
                                 indexBuffer:_mtlIndexBuffer
                           indexBufferOffset:offset];
    
}

void CommandBufferMTL::endRenderPass()
{
    afterDraw();
}

void CommandBufferMTL::captureScreen(std::function<void(const unsigned char*, int, int)> callback)
{
    [_mtlCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBufferMTL) {
        Utils::getTextureBytes(0, 0, static_cast<int>(_drawableTexture.width), static_cast<int>(_drawableTexture.height), _drawableTexture, callback);
        Device::getInstance()->setFrameBufferOnly(true);
    }];
}

void CommandBufferMTL::endFrame()
{
    _mtlRenderCommandEncoderCache(_mtlCommandBuffer).clearCache();
    _mtlRenderEncoder = nil;
    
    [_mtlCommandBuffer presentDrawable:DeviceMTL::getCurrentDrawable()];
    _drawableTexture = DeviceMTL::getCurrentDrawable().texture;
    [_mtlCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
        // GPU work is complete
        // Signal the semaphore to start the CPU work
        dispatch_semaphore_signal(_frameBoundarySemaphore);
    }];

    [_mtlCommandBuffer commit];
    [_mtlCommandBuffer release];
    DeviceMTL::resetCurrentDrawable();
    [_autoReleasePool drain];
}

void CommandBufferMTL::afterDraw()
{
    if (_mtlIndexBuffer)
    {
        [_mtlIndexBuffer release];
        _mtlIndexBuffer = nullptr;
    }
    
    CC_SAFE_RELEASE_NULL(_programState);
}

state_helper::DepthStencilStateTracker &CommandBufferMTL::_mtlDepthStencilStateTracker()
{
    static state_helper::DepthStencilStateTracker __mtlDepthStencilStateTracker;
    return __mtlDepthStencilStateTracker;
}

state_helper::RenderCommandEncoderCache &CommandBufferMTL::_mtlRenderCommandEncoderCache(id<MTLCommandBuffer> mtlCommandBuffer)
{
    static state_helper::RenderCommandEncoderCache __mtlRenderCommandEncoderCache{mtlCommandBuffer, false};
    // Update the command buffer
    __mtlRenderCommandEncoderCache.setCreatorHelper(mtlCommandBuffer);
    return __mtlRenderCommandEncoderCache;
}

void CommandBufferMTL::setDepthStencilState(DepthStencilState* depthStencilState)
{
    if (depthStencilState) {
        _mtlDepthStencilState = static_cast<DepthStencilStateMTL*>(depthStencilState)->getMTLDepthStencilState();
        _mtlDepthStencilStateTracker().updateState(state_helper::DepthStencilState::convert(depthStencilState->depthStencilInfo()));
    } else {
        _mtlDepthStencilState = nil;
        _mtlDepthStencilStateTracker().invalidate();
    }
    
}

void CommandBufferMTL::prepareDrawing() const
{
    setUniformBuffer();
    setTextures();
    
    if (_mtlDepthStencilState)
    {
        if (_mtlDepthStencilStateTracker().stateChanged()) {
            [_mtlRenderEncoder setDepthStencilState:_mtlDepthStencilState];
            [_mtlRenderEncoder setStencilFrontReferenceValue:_stencilReferenceValueFront
                                          backReferenceValue:_stencilReferenceValueBack];
        }
    }
}

void CommandBufferMTL::setTextures() const
{
    if (_programState)
    {
        doSetTextures(true);
        doSetTextures(false);
    }
}

void CommandBufferMTL::doSetTextures(bool isVertex) const
{
    const auto& bindTextureInfos = (isVertex) ? _programState->getVertexTextureInfos() : _programState->getFragmentTextureInfos();

    for(const auto& iter : bindTextureInfos)
    {
        //FIXME: should support texture array.
        int i = 0;
        auto location = iter.first;
        const auto& textures = iter.second.textures;
        
        if (isVertex)
        {
            [_mtlRenderEncoder setVertexTexture:getMTLTexture(textures[i])
                                    atIndex:location];
            [_mtlRenderEncoder setVertexSamplerState:getMTLSamplerState(textures[i])
                                         atIndex:location];
        }
        else
        {
            [_mtlRenderEncoder setFragmentTexture:getMTLTexture(textures[i])
                                      atIndex:location];
            [_mtlRenderEncoder setFragmentSamplerState:getMTLSamplerState(textures[i])
                                           atIndex:location];
        }
        
        ++i;
    }
}

void CommandBufferMTL::setUniformBuffer() const
{
    if (_programState)
    {
        auto &callbackUniforms = _programState->getCallbackUniforms();
        
        for(auto &cb : callbackUniforms)
        {
            cb.second(_programState, cb.first);
        }
        
        // Uniform buffer is bound to index 1.
        std::size_t bufferSize = 0;
        char* vertexBuffer = nullptr;
        _programState->getVertexUniformBuffer(&vertexBuffer, bufferSize);
        if (bufferSize > 0 && vertexBuffer)
        {
            [_mtlRenderEncoder setVertexBytes:vertexBuffer
                                       length:bufferSize 
                                       atIndex:1];
        }
        
        char* fragmentBuffer = nullptr;
        _programState->getFragmentUniformBuffer(&fragmentBuffer, bufferSize);
        if (bufferSize > 0 && fragmentBuffer)
        {
            [_mtlRenderEncoder setFragmentBytes:fragmentBuffer
                                         length:bufferSize
                                        atIndex:1];
        }
    }
}

void CommandBufferMTL::setLineWidth(float lineWidth)
{
}

void CommandBufferMTL::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    MTLScissorRect scissorRect;
    
    if(isEnabled)
    {
        if (x < 0.0f) {
            scissorRect.x = 0.0f;
            width += x;
        } else
            scissorRect.x = static_cast<unsigned long>(x);
        if (_renderTargetHeight - height - y < 0.0f) {
            scissorRect.y = 0.0f;
            height += _renderTargetHeight - height - y;
        } else
            scissorRect.y = static_cast<unsigned long>(_renderTargetHeight - height - y);
        scissorRect.width = static_cast<unsigned long>(std::max(0.0f, width));
        scissorRect.height = static_cast<unsigned long>(std::max(0.0f, height));
        // Ensure that we are smaller than render pass size
        scissorRect.width  = static_cast<unsigned long>(std::max(0.0f, std::min( 1.0f* scissorRect.width,  1.0f* _renderTargetWidth  - 1.0f * scissorRect.x)));
        if (scissorRect.width == 0)
            scissorRect.x = std::min(scissorRect.x, static_cast<unsigned long>(_renderTargetWidth));
        scissorRect.height = static_cast<unsigned long>(std::max(0.0f, std::min( 1.0f* scissorRect.height, 1.0f* _renderTargetHeight - 1.0f * scissorRect.y)));
        if (scissorRect.height == 0)
            scissorRect.y = std::min(scissorRect.y, static_cast<unsigned long>(_renderTargetHeight));
    }
    else
    {
        scissorRect.x = 0;
        scissorRect.y = 0;
        scissorRect.width = _renderTargetWidth;
        scissorRect.height = _renderTargetHeight;
    }
    
    [_mtlRenderEncoder setScissorRect:scissorRect]  ;    
}


CC_BACKEND_END
