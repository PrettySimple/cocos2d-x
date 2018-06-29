/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.

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

#include "renderer/CCMeshCommand.h"

#include "2d/CCLight.h"
#include "base/CCConfiguration.h"
#include "base/CCDirector.h"
#include "base/CCEventCustom.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventType.h"
#include "base/ccMacros.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCMaterial.h"
#include "renderer/CCPass.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCTechnique.h"
#include "renderer/CCTexture2D.h"
#include "renderer/CCTextureAtlas.h"
#include "renderer/ccGLStateCache.h"

#include <utility>

NS_CC_BEGIN

MeshCommand::MeshCommand()
: RenderCommand(RenderCommand::Type::MESH_COMMAND)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
    // listen the event that renderer was recreated on Android/WP8
    _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(MeshCommand::listenRendererRecreated, this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
#endif
}

void MeshCommand::init(float globalZOrder, Material* material, GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexFormat,
                       ssize_t indexCount, const cocos2d::Mat4& mv, uint32_t flags)
{
    CCASSERT(material, "material cannot be null");

    RenderCommand::init(globalZOrder, mv, flags);

    _globalOrder = globalZOrder;
    _material = material;

    _vertexBuffer = vertexBuffer;
    _indexBuffer = indexBuffer;
    _primitive = primitive;
    _indexFormat = indexFormat;
    _indexCount = indexCount;
    _mv.set(mv);

    _is3D = true;
}

void MeshCommand::init(float globalZOrder, GLuint textureID, GLProgramState* glProgramState, RenderState::StateBlock* stateBlock, GLuint vertexBuffer,
                       GLuint indexBuffer, GLenum primitive, GLenum indexFormat, ssize_t indexCount, const cocos2d::Mat4& mv, uint32_t flags)
{
    CCASSERT(glProgramState, "GLProgramState cannot be null");
    CCASSERT(stateBlock, "StateBlock cannot be null");
    CCASSERT(!_material, "cannot init with GLProgramState if previously inited without GLProgramState");

    RenderCommand::init(globalZOrder, mv, flags);

    _globalOrder = globalZOrder;
    _textureID = textureID;

    // weak ref
    _glProgramState = glProgramState;
    _stateBlock = stateBlock;

    _vertexBuffer = vertexBuffer;
    _indexBuffer = indexBuffer;
    _primitive = primitive;
    _indexFormat = indexFormat;
    _indexCount = indexCount;
    _mv.set(mv);

    _is3D = true;
}

void MeshCommand::setDisplayColor(const Vec4& color)
{
    CCASSERT(!_material, "If using material, you should set the color as a uniform: use u_color");

    _displayColor = color;
}

void MeshCommand::setMatrixPalette(const Vec4* matrixPalette)
{
    CCASSERT(!_material, "If using material, you should set the color as a uniform: use u_matrixPalette");

    _matrixPalette = matrixPalette;
}

void MeshCommand::setMatrixPaletteSize(int size)
{
    CCASSERT(!_material, "If using material, you should set the color as a uniform: use u_matrixPalette with its size");

    _matrixPaletteSize = size;
}

MeshCommand::~MeshCommand()
{
    releaseVAO();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
    Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

void MeshCommand::applyRenderState()
{
    CCASSERT(!_material, "Must not be called when using materials");
    CCASSERT(_stateBlock, "StateBlock must be non null");

    // blend and texture
    GL::bindTexture2D(_textureID);

    _stateBlock->bind();
}

template <class T>
inline constexpr void hash_combine(std::size_t& seed, T const& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

void MeshCommand::genMaterialID(GLuint texID, void* glProgramState, GLuint vertexBuffer, GLuint indexBuffer, BlendFunc blend)
{
    std::size_t seed = 0;

    hash_combine(seed, texID);
    hash_combine(seed, vertexBuffer);
    hash_combine(seed, indexBuffer);
    hash_combine(seed, blend.src);
    hash_combine(seed, blend.dst);
    // glProgramState is hashed because it contains:
    //  *  uniforms/values
    //  *  glProgram
    //
    // we safely can when the same glProgramState is being used then they share those states
    // if they don't have the same glProgramState, they might still have the same
    // uniforms/values and glProgram, but it would be too expensive to check the uniforms.
    hash_combine(seed, reinterpret_cast<std::uintptr_t>(glProgramState));

    _materialID = seed;
}

void MeshCommand::preBatchDraw()
{
    // Do nothing if using material since each pass needs to bind its own VAO
    if (!_material)
    {
        if (Configuration::getInstance()->supportsShareableVAO() && _vao == 0)
            buildVAO();
        if (_vao)
        {
            GL::bindVAO(_vao);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

            // FIXME: Assumes that all the passes in the Material share the same Vertex Attribs
            GLProgramState* programState = _material ? _material->_currentTechnique->_passes.at(0)->getGLProgramState() : _glProgramState;
            programState->applyAttributes();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);
        }
    }
}

void MeshCommand::batchDraw()
{
    if (_material)
    {
        for (const auto& pass : _material->_currentTechnique->_passes)
        {
            pass->bind(_mv);

            glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);
            CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);

            pass->unbind();
        }
    }
    else
    {
        _glProgramState->applyGLProgram(_mv);

        // set render state
        applyRenderState();

        // Draw
        glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);
        CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);
    }
}
void MeshCommand::postBatchDraw()
{
    // when using material, unbind is after draw
    if (!_material)
    {
        if (_vao)
        {
            GL::bindVAO(0);
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // restore the default state since we don't know
        // if the next command will need the default state or not
        RenderState::StateBlock::restore(0);
    }
}

void MeshCommand::execute()
{
    // Draw without VAO
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

    if (_material)
    {
        for (const auto& pass : _material->_currentTechnique->_passes)
        {
            pass->bind(_mv, true);

            glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);
            CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);

            pass->unbind();
        }
    }
    else
    {
        // set render state
        _glProgramState->apply(_mv);

        applyRenderState();

        // Draw
        glDrawElements(_primitive, (GLsizei)_indexCount, _indexFormat, 0);

        CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _indexCount);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshCommand::buildVAO()
{
    // FIXME: Assumes that all the passes in the Material share the same Vertex Attribs
    GLProgramState* programState = (_material != nullptr) ? _material->_currentTechnique->_passes.at(0)->getGLProgramState() : _glProgramState;

    
    
    releaseVAO();
    glGenVertexArrays(1, &_vao);
    GL::bindVAO(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

    GL::enableVertexAttribs(programState->getVertexAttribsFlags(), _vao);
    programState->applyAttributes(false);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

    GL::bindVAO(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void MeshCommand::releaseVAO()
{
    if (_vao)
    {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
        GL::bindVAO(0);
    }
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
void MeshCommand::listenRendererRecreated(EventCustom* event)
{
    _vao = 0;
}

#endif

NS_CC_END
