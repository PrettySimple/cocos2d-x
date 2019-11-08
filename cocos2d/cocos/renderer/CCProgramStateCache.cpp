//
//  ProgramStateCache.cpp
//  cocos2d
//
//  Created by Christophe Genova on 08/11/2019.
//

#include "CCProgramStateCache.h"

#include <xxhash.h>
#include <cocos/base/CCDirector.h>
#include <cocos/renderer/CCTexture2D.h>
#include <cocos/renderer/CCRenderer.h>
#include <cocos/renderer/backend/ProgramState.h>
#include <cocos/renderer/CCCallbackCommand.h>


using namespace cocos2d;

ProgramStateCache &ProgramStateCache::_getInstance()
{
    static ProgramStateCache _cache;
    return _cache;
}

ProgramStateCache::MaterialId ProgramStateCache::computeMaterialId(cocos2d::backend::Program *       programs,
                                                                   cocos2d::backend::TextureBackend *texture,
                                                                   cocos2d::BlendFunc                blendType)
{
    struct
    {
        void *               texture;
        void *               program;
        backend::BlendFactor src;
        backend::BlendFactor dst;
    } hashMe;
    
    // NOTE: Initialize hashMe struct to make the value of padding bytes be filled with zero.
    // It's important since XXH32 below will also consider the padding bytes which probably
    // are set to random values by different compilers.
    memset(&hashMe, 0, sizeof(hashMe));
    
    hashMe.texture = texture;
    hashMe.src     = blendType.src;
    hashMe.dst     = blendType.dst;
    hashMe.program = programs;
    return XXH32((const void *)&hashMe, sizeof(hashMe), 0);
}

backend::ProgramState *ProgramStateCache::getOrCreateProgramState(backend::ProgramType type,
                                                                    cocos2d::Texture2D * texture,
                                                                    cocos2d::BlendFunc   blendType)
{
    return _getInstance()._getOrCreateProgramState(new cocos2d::backend::ProgramState(type), texture, blendType);
}

backend::ProgramState *ProgramStateCache::getOrCreateProgramState(const std::string &vertexSh,
                                                                    const std::string &fragSh,
                                                                    cocos2d::Texture2D * texture,
                                                                    cocos2d::BlendFunc   blendType)
{
    return _getInstance()._getOrCreateProgramState(new cocos2d::backend::ProgramState(vertexSh, fragSh), texture, blendType);
}

backend::ProgramState *ProgramStateCache::_getOrCreateProgramState(cocos2d::backend::ProgramState *templateProgram,
                                                                     cocos2d::Texture2D *            texture,
                                                                     cocos2d::BlendFunc              blendType)
{
    auto materialID         = computeMaterialId(templateProgram->getProgram(), texture->getBackendTexture(), blendType);
    
    auto cachedProgramState = _programStateCache.find(materialID);
    if (cachedProgramState != _programStateCache.end()) {
        delete templateProgram;
        return cachedProgramState->second;
    }
    
    _programStateCache[materialID] = templateProgram;
    return templateProgram;
}

void ProgramStateCache::clear()
{
    _getInstance()._programStateCache.clear();
}

void ProgramStateCache::setUpStandardAttributeLayout(cocos2d::backend::ProgramState *programState)
{
    auto vertexLayout = programState->getVertexLayout();
    //a_position
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               programState->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3,
                               0,
                               false);
    
    //a_texCoord
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               programState->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2,
                               offsetof(V3F_C4B_T2F, texCoords),
                               false);
    
    //a_color
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR,
                               programState->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::UBYTE4,
                               offsetof(V3F_C4B_T2F, colors),
                               true);
    
    vertexLayout->setLayout(sizeof(V3F_C4B_T2F));
}

void ProgramStateCache::addStandardUniformRenderCommand(CallbackCommand *               setUniformCmd,
                                                        cocos2d::Renderer *             renderer,
                                                        cocos2d::backend::ProgramState *programState,
                                                        float                           globalOrder,
                                                        cocos2d::Texture2D *            texture,
                                                        bool                            isUsingMVP)
{
    assert(setUniformCmd);
    setUniformCmd->init(globalOrder);
    setUniformCmd->func = [programState, texture, isUsingMVP]() {
        const auto& projectionMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
        const auto& modelMat = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        auto m = isUsingMVP ? projectionMat * modelMat : projectionMat;
        programState->setUniform(programState->getUniformLocation(backend::Uniform::MVP_MATRIX),
                                 m.m,
                                 sizeof(m.m));
        programState->setTexture(programState->getUniformLocation(backend::Uniform::TEXTURE), 0, texture->getBackendTexture());
    };
    
    renderer->addCommand(setUniformCmd);
}
