#pragma once

#include <stddef.h>
#include <stdint.h>
#include <unordered_map>
#include <utility>

#include <Metal/MTLDepthStencil.h>
#include <Metal/MTLDevice.h>
#include <Metal/MTLLibrary.h>
#include <Metal/MTLRenderCommandEncoder.h>
#include <Metal/MTLRenderPipeline.h>
#include <Metal/MTLSampler.h>
#include <Metal/MTLVertexDescriptor.h>

#include "cocos/renderer/backend/DepthStencilState.h"
#include "cocos/renderer/backend/RenderPassDescriptor.h"

#if defined(__LP64__)
#if !defined(__SIZEOF_INT128__)
#error platform doesn't support __uint128_t
#endif
#endif

template <typename T>
static constexpr void hash_combine(size_t &seed, T const &value) noexcept
{
#if defined(__LP64__)
    seed ^= static_cast<size_t>(std::hash<T>{}(value) + static_cast<__uint128_t>(0x9e3779b97f4a7c15ULL) + (seed << 4) + (seed >> 1));
#else
    seed ^= std::hash<T>{}(value) + static_cast<size_t>(0x9e3779b9U) + (seed << 4) + (seed >> 1);
#endif
}

namespace std {
    template <>
    struct hash<cocos2d::backend::RenderPassDescriptor> {
        size_t operator()(cocos2d::backend::RenderPassDescriptor const &key) const
        {
            size_t hash = 0;
            hash_combine(hash, key.clearDepthValue);
            hash_combine(hash, key.clearStencilValue);
            for(auto &c : key.clearColorValue)
                hash_combine(hash, c);
            hash_combine(hash, key.needColorAttachment);
            hash_combine(hash, key.depthTestEnabled);
            hash_combine(hash, key.stencilTestEnabled);
            hash_combine(hash, key.needClearColor);
            hash_combine(hash, key.needClearDepth);
            hash_combine(hash, key.needClearStencil);
            hash_combine(hash, key.depthAttachmentTexture);
            hash_combine(hash, key.stencilAttachmentTexture);
            for(auto &c : key.colorAttachmentsTexture)
                hash_combine(hash, c);
            return hash;
        }
    };
} // namespace std

namespace cocos2d {
    namespace backend {
        namespace state_helper {

            // StateTracker

            template <typename StateType>
            class StateTracker {
                mutable bool _stateDirty   = true;
                StateType    _currentState = {};

              public:
                void invalidate() noexcept { _stateDirty = true; }

                void updateState(const StateType &newState) noexcept
                {
                    if (_currentState != newState) {
                        _currentState = newState;
                        _stateDirty   = true;
                    }
                }

                bool stateChanged() const noexcept
                {
                    bool ret    = _stateDirty;
                    _stateDirty = false;
                    return ret;
                }

                const StateType &getState() const
                {
                    return _currentState;
                }
            };

            // StateCache

            template <typename StateType, typename MetalType, typename StateCreator, typename CreatorHelper>
            class StateCache {
              public:
                using Map         = std::unordered_map<StateType, __strong MetalType>;
                using MapIterator = typename Map::iterator;
                
                StateCache()                   = default;
                StateCache(const StateCache &) = delete;
                StateCache &operator=(const StateCache &) = delete;

                StateCache(CreatorHelper creatorHelper,
                           bool storeInCache = true)
                    : _storeInCache(storeInCache)
                    , _creator()
                    , _creatorHelper(creatorHelper)
                    , _stateCache()
                {
                }

                void setCreatorHelper(CreatorHelper creatorHelper) noexcept
                {
                    _creatorHelper = creatorHelper;
                }

                MetalType getOrCreateState(const StateType &state) noexcept
                {
                    assert(_creatorHelper);

                    // Check if a valid state already exists in the cache.
                    auto iter = cacheFind(state);
                    if (iter != _stateCache.end()) {
                        return iter->second;
                    }
                    
                    if (!_storeInCache)
                        // Release the previsous stored state
                        clearCache(&state);
                    
                    // If we reach this point, we couldn't find one in the cache; create a new one.
                    const auto &newState = _creator(_creatorHelper, state);
                    
                    _stateCache.insert({ state, newState });

                    return newState;
                }

                MapIterator cacheFind(const StateType &state) {
                    return _stateCache.find(state);
                }
                
                void clearCache(const StateType *state = nullptr) {}
                
            protected:
                bool          _storeInCache;
                StateCreator  _creator;
                CreatorHelper _creatorHelper = nil;
                Map           _stateCache;
            };

            // DepthStencilState

            struct DepthStencilState {
                MTLCompareFunction depthCompareFunction = MTLCompareFunctionAlways;
                bool               depthWriteEnabled    = false;

                MTLCompareFunction  stencilCompareFunction = MTLCompareFunctionAlways;
                MTLStencilOperation stencilFailureFunction = MTLStencilOperationKeep;
                bool                stencilWriteEnabled    = false;
                uint8_t             stencilMask            = 0xff;
                uint8_t             stencilBitsetRead      = 0xff;
                uint8_t             stencilBitsetWrite     = 0xff;

                inline bool operator==(const DepthStencilState &rhs) const noexcept
                {
                    return (this->depthCompareFunction == rhs.depthCompareFunction &&
                            this->depthWriteEnabled == rhs.depthWriteEnabled &&
                            this->stencilCompareFunction == rhs.stencilCompareFunction &&
                            this->stencilFailureFunction == rhs.stencilFailureFunction &&
                            this->stencilWriteEnabled == rhs.stencilWriteEnabled &&
                            stencilMask == rhs.stencilMask &&
                            stencilBitsetRead == rhs.stencilBitsetRead &&
                            stencilBitsetWrite == rhs.stencilBitsetWrite);
                }

                inline bool operator!=(const DepthStencilState &rhs) const noexcept
                {
                    return !operator==(rhs);
                }

                static DepthStencilState convert(const cocos2d::backend::DepthStencilDescriptor &src);
            };

            struct DepthStateCreator {
                id<MTLDepthStencilState> operator()(id<MTLDevice> device, const DepthStencilState &state) noexcept;
            };

            using DepthStencilStateCache = StateCache<DepthStencilState, id<MTLDepthStencilState>, DepthStateCreator, id<MTLDevice>>;

            using DepthStencilStateTracker = StateTracker<DepthStencilState>;

        
            using RenderCommandEncoderCacheElement = struct {
                       MTLRenderPassDescriptor* renderPassDecriptor;
                       id<MTLRenderCommandEncoder> renderCommandEncoder;
            };
        
            struct RenderCommandEncoderCreator {
            public:
                RenderCommandEncoderCacheElement operator()(id<MTLCommandBuffer> commandBuffer,
                                                       const RenderPassDescriptor &descriptor) noexcept;
                
                std::string debugInfo;
            protected:
                friend class StateCache<RenderPassDescriptor, id<MTLRenderCommandEncoder>, RenderCommandEncoderCreator, id<MTLCommandBuffer>>;
            };
        
           
            using RenderCommandEncoderCache = StateCache<RenderPassDescriptor, RenderCommandEncoderCacheElement, RenderCommandEncoderCreator, id<MTLCommandBuffer>>;
        
        } // namespace state_helper
    }     // namespace backend
} // namespace cocos2d

namespace std {
    template <>
    struct hash<cocos2d::backend::state_helper::DepthStencilState> {
        size_t operator()(cocos2d::backend::state_helper::DepthStencilState const &key) const
        {
            size_t hash = key.depthCompareFunction;
            hash_combine(hash, key.depthWriteEnabled);
            hash_combine(hash, key.stencilCompareFunction);
            hash_combine(hash, key.stencilFailureFunction);
            hash_combine(hash, key.stencilWriteEnabled);
            hash_combine(hash, key.stencilMask);
            hash_combine(hash, key.stencilBitsetRead);
            hash_combine(hash, key.stencilBitsetWrite);
            return hash;
        }
    };
} // namespace std
