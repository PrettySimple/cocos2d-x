#include "MetalState.hpp"
#include "cocos/renderer/backend/metal/TextureMTL.h"
#include "cocos/renderer/backend/metal/Utils.h"
#include "cocos/renderer/backend/RenderPassDescriptor.h"
#include "cocos/renderer/backend/metal/Utils.h"

using namespace cocos2d::backend::state_helper;
namespace
    {
    MTLCompareFunction toMTLCompareFunction(cocos2d::backend::CompareFunction compareFunction)
    {
        MTLCompareFunction ret = MTLCompareFunctionNever;
        switch (compareFunction) {
            case cocos2d::backend::CompareFunction::NEVER:
                ret = MTLCompareFunctionNever;
                break;
            case cocos2d::backend::CompareFunction::LESS:
                ret = MTLCompareFunctionLess;
                break;
            case cocos2d::backend::CompareFunction::LESS_EQUAL:
                ret = MTLCompareFunctionLessEqual;
                break;
            case cocos2d::backend::CompareFunction::GREATER:
                ret = MTLCompareFunctionGreater;
                break;
            case cocos2d::backend::CompareFunction::GREATER_EQUAL:
                ret = MTLCompareFunctionGreaterEqual;
                break;
            case cocos2d::backend::CompareFunction::EQUAL:
                ret = MTLCompareFunctionEqual;
                break;
            case cocos2d::backend::CompareFunction::NOT_EQUAL:
                ret = MTLCompareFunctionNotEqual;
                break;
            case cocos2d::backend::CompareFunction::ALWAYS:
                ret = MTLCompareFunctionAlways;
                break;
            default:
                assert(false);
                break;
        }
        return ret;
    }
    
    MTLStencilOperation toMTLStencilOperation(cocos2d::backend::StencilOperation operation)
    {
        MTLStencilOperation ret = MTLStencilOperationKeep;
        switch (operation) {
            case cocos2d::backend::StencilOperation::KEEP:
                ret = MTLStencilOperationKeep;
                break;
            case cocos2d::backend::StencilOperation::ZERO:
                ret = MTLStencilOperationZero;
                break;
            case cocos2d::backend::StencilOperation::REPLACE:
                ret = MTLStencilOperationReplace;
                break;
            case cocos2d::backend::StencilOperation::INVERT:
                ret = MTLStencilOperationInvert;
                break;
            case cocos2d::backend::StencilOperation::INCREMENT_WRAP:
                ret = MTLStencilOperationIncrementWrap;
                break;
            case cocos2d::backend::StencilOperation::DECREMENT_WRAP:
                ret = MTLStencilOperationDecrementWrap;
                break;
            default:
                assert(false);
                break;
        }
        return ret;
    }
    
    MTLRenderPassDescriptor* toMTLRenderPassDescriptor(const cocos2d::backend::RenderPassDescriptor& descriptor)
    {
        MTLRenderPassDescriptor* mtlDescritpor = [MTLRenderPassDescriptor renderPassDescriptor];
        
        // Set color attachments.
        if (descriptor.needColorAttachment)
        {
            bool hasCustomColorAttachment = false;
            for (int i = 0; i < MAX_COLOR_ATTCHMENT; ++i)
            {
                if (! descriptor.colorAttachmentsTexture[i])
                    continue;
                
                mtlDescritpor.colorAttachments[i].texture = static_cast<cocos2d::backend::TextureMTL*>(descriptor.colorAttachmentsTexture[i])->getMTLTexture();
                if (descriptor.needClearColor)
                {
                    mtlDescritpor.colorAttachments[i].loadAction = MTLLoadActionClear;
                    mtlDescritpor.colorAttachments[i].clearColor = MTLClearColorMake(descriptor.clearColorValue[0],
                                                                                     descriptor.clearColorValue[1],
                                                                                     descriptor.clearColorValue[2],
                                                                                     descriptor.clearColorValue[3]);
                }
                else
                    mtlDescritpor.colorAttachments[i].loadAction = MTLLoadActionLoad;
                
                hasCustomColorAttachment = true;
            }
            
            if (!hasCustomColorAttachment)
            {
                mtlDescritpor.colorAttachments[0].texture = cocos2d::backend::DeviceMTL::getCurrentDrawable().texture;
                if (descriptor.needClearColor)
                {
                    mtlDescritpor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    mtlDescritpor.colorAttachments[0].clearColor = MTLClearColorMake(descriptor.clearColorValue[0],
                                                                                     descriptor.clearColorValue[1],
                                                                                     descriptor.clearColorValue[2],
                                                                                     descriptor.clearColorValue[3]);
                }
                else
                    mtlDescritpor.colorAttachments[0].loadAction = MTLLoadActionLoad;
            }
            
            mtlDescritpor.colorAttachments[0].storeAction = MTLStoreActionStore;
        }
        
        if(descriptor.needDepthStencilAttachment())
        {
            // Set depth attachment
            {
                if (descriptor.depthAttachmentTexture)
                    mtlDescritpor.depthAttachment.texture = static_cast<cocos2d::backend::TextureMTL*>(descriptor.depthAttachmentTexture)->getMTLTexture();
                else
                    mtlDescritpor.depthAttachment.texture = cocos2d::backend::Utils::getDefaultDepthStencilTexture();
                
                if (descriptor.needClearDepth)
                {
                    mtlDescritpor.depthAttachment.loadAction = MTLLoadActionClear;
                    mtlDescritpor.depthAttachment.clearDepth = descriptor.clearDepthValue;
                }
                else
                    mtlDescritpor.depthAttachment.loadAction = MTLLoadActionLoad;
                
                mtlDescritpor.depthAttachment.storeAction = MTLStoreActionStore;
            }
            
            // Set stencil attachment
            {
                if (descriptor.stencilAttachmentTexture)
                    mtlDescritpor.stencilAttachment.texture = static_cast<cocos2d::backend::TextureMTL*>(descriptor.stencilAttachmentTexture)->getMTLTexture();
                else
                    mtlDescritpor.stencilAttachment.texture = cocos2d::backend::Utils::getDefaultDepthStencilTexture();
                
                if (descriptor.needClearStencil)
                {
                    mtlDescritpor.stencilAttachment.loadAction = MTLLoadActionClear;
                    mtlDescritpor.stencilAttachment.clearStencil = descriptor.clearStencilValue;
                }
                else
                    mtlDescritpor.stencilAttachment.loadAction = MTLLoadActionLoad;
                
                mtlDescritpor.stencilAttachment.storeAction = MTLStoreActionStore;
            }
        }
        
        return mtlDescritpor;
    }
    }

id<MTLDepthStencilState> DepthStateCreator::operator()(id<MTLDevice> device, const DepthStencilState& state) noexcept {
    MTLDepthStencilDescriptor* depthStencilDescriptor = [MTLDepthStencilDescriptor new];

    depthStencilDescriptor.depthCompareFunction = state.depthCompareFunction;
    depthStencilDescriptor.depthWriteEnabled = state.depthWriteEnabled;

    if (state.stencilWriteEnabled || (!state.stencilWriteEnabled && state.stencilMask != 0xff)) {
        MTLStencilDescriptor* stencilDescriptor = nil;
        stencilDescriptor = [MTLStencilDescriptor new];
        stencilDescriptor.stencilCompareFunction = state.stencilCompareFunction;
        stencilDescriptor.stencilFailureOperation = state.stencilFailureFunction;
        stencilDescriptor.depthFailureOperation = MTLStencilOperationKeep;
        if (state.stencilWriteEnabled) {
            stencilDescriptor.depthStencilPassOperation = MTLStencilOperationReplace;
        } else {
            stencilDescriptor.depthStencilPassOperation = MTLStencilOperationKeep;
        }

        stencilDescriptor.writeMask = state.stencilBitsetWrite;
        stencilDescriptor.readMask = state.stencilBitsetRead;

        depthStencilDescriptor.frontFaceStencil = stencilDescriptor;
        depthStencilDescriptor.backFaceStencil = stencilDescriptor;
    }

    return [device newDepthStencilStateWithDescriptor:depthStencilDescriptor];
}

#define VERBOSE_RENDERCOMMANDENCODER

RenderCommandEncoderCacheElement RenderCommandEncoderCreator::operator()(id<MTLCommandBuffer> commandBuffer,
                                                                    const RenderPassDescriptor &descriptor) noexcept
{
    auto mtlDescriptor = toMTLRenderPassDescriptor(descriptor);
    auto renderCommandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:mtlDescriptor];
    [renderCommandEncoder retain];
    [mtlDescriptor retain];
#ifdef VERBOSE_RENDERCOMMANDENCODER
    renderCommandEncoder.label = [NSString stringWithUTF8String:debugInfo.c_str()];
#endif
    return { mtlDescriptor, renderCommandEncoder };
}

template <>
RenderCommandEncoderCache::MapIterator RenderCommandEncoderCache::cacheFind(const RenderPassDescriptor &state)
{
    return std::find_if(_stateCache.begin(), _stateCache.end(),
                        [&state](auto &e) {
        return state.equalExceptClear(e.first);
    });
}

template <>
void RenderCommandEncoderCache::clearCache(const RenderPassDescriptor *descriptor)
{
#ifdef VERBOSE_RENDERCOMMANDENCODER
    if (descriptor) {
        _creator.debugInfo = "";
        if (_stateCache.size() == 0)
            _creator.debugInfo = "New RC";
        else {
            _creator.debugInfo = "Switch RC due to: " + _stateCache.begin()->first.diff(*descriptor);
            if (_stateCache.size() > 1)
                _creator.debugInfo += " More than 1 rendercommanderencoder, NOT POSSIBLE!!!! ";
        }
        if (descriptor->needClearColor)
            _creator.debugInfo += "| Clear Color";
        if (descriptor->needClearDepth)
            _creator.debugInfo += "| Clear Depth";
        if (descriptor->needClearStencil)
            _creator.debugInfo += "| Clear Stencil";
    }
#endif
    
    for(auto &renderCommandEncoder : _stateCache) {
        [renderCommandEncoder.second.renderCommandEncoder endEncoding];
        [renderCommandEncoder.second.renderCommandEncoder release];
        [renderCommandEncoder.second.renderPassDecriptor release];
    }
    _stateCache.clear();
}


DepthStencilState DepthStencilState::convert(const cocos2d::backend::DepthStencilDescriptor &src)
{
    return DepthStencilState{
        src.depthTestEnabled ? toMTLCompareFunction(src.depthCompareFunction) : MTLCompareFunctionAlways,
        src.depthWriteEnabled,
        src.stencilTestEnabled ? toMTLCompareFunction(src.frontFaceStencil.stencilCompareFunction) : MTLCompareFunctionAlways,
        src.stencilTestEnabled ? toMTLStencilOperation(src.frontFaceStencil.stencilFailureOperation) : MTLStencilOperationKeep,
        toMTLStencilOperation(src.frontFaceStencil.depthStencilPassOperation) == MTLStencilOperationReplace,
        src.stencilTestEnabled ? static_cast<uint8_t>(0x0F) : static_cast<uint8_t>(0xFF),
        src.frontFaceStencil.readMask,
        src.frontFaceStencil.writeMask
    };
}
