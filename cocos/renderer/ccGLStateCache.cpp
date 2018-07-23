/****************************************************************************
Copyright (c) 2011      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (C) 2013-2016 Chukong Technologies Inc.

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

#include <cocos/renderer/ccGLStateCache.h>

#include <cocos/base/CCConfiguration.h>
#include <cocos/base/CCDirector.h>
#include <cocos/base/ccConfig.h>
#include <cocos/base/ccMacros.h>
#include <cocos/renderer/CCRenderState.h>
#include <cocos/renderer/CCTexture2D.h>

#include <cstddef>
#include <mutex>
#include <array>
#include <limits>
#include <thread>
#include <unordered_map>
#include <utility>

NS_CC_BEGIN

static const int MAX_ATTRIBUTES = 16;
static const int MAX_ACTIVE_TEXTURE = 16;

namespace GL
{
    static std::mutex currentBoundProjectionMatrixMutex;
#if CC_ENABLE_GL_STATE_CACHE
    static std::mutex currentBoundTextureMutex;
    static std::mutex currentBoundShaderMutex;
    static std::mutex currentBlendingSourceMutex;
    static std::mutex currentBlendingTestMutex;
    static std::mutex GLServerStateMutex;
    static std::mutex VAOMutex;
    static std::mutex activeTextureMutex;
#endif
} // namespace GL

GLuint& s_currentProjectionMatrix()
{
    static std::unordered_map<std::thread::id, GLuint> currentProjectionMatrix;

    const auto tId = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(GL::currentBoundProjectionMatrixMutex);
    auto search = currentProjectionMatrix.find(tId);
    if (search != currentProjectionMatrix.end())
    {
        return search->second;
    }

    auto it = currentProjectionMatrix.emplace(tId, -1);
    return it.first->second;
}

static uint32_t s_attributeFlags = 0;

#if CC_ENABLE_GL_STATE_CACHE

std::unordered_map<std::thread::id, GLuint>& currentShaderProgram()
{
    static std::unordered_map<std::thread::id, GLuint> currentShaderProgram;
    return currentShaderProgram;
}

GLuint& s_currentShaderProgram()
{
    auto& shaderProgram = currentShaderProgram();

    const auto tId = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(GL::currentBoundShaderMutex);
    auto search = shaderProgram.find(tId);
    if (search != shaderProgram.end())
    {
        return search->second;
    }

    auto it = shaderProgram.emplace(tId, -1);
    return it.first->second;
}

std::unordered_map<std::thread::id, std::array<GLuint, MAX_ACTIVE_TEXTURE>>& currentBoundTexture()
{
    static std::unordered_map<std::thread::id, std::array<GLuint, MAX_ACTIVE_TEXTURE>> currentBoundTexture;
    return currentBoundTexture;
}

std::array<GLuint, MAX_ACTIVE_TEXTURE>& s_currentBoundTexture()
{
    auto& boundTexture = currentBoundTexture();

    const auto tId = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(GL::currentBoundTextureMutex);
    auto search = boundTexture.find(tId);
    if (search != boundTexture.end())
    {
        return search->second;
    }

    std::array<GLuint, MAX_ACTIVE_TEXTURE> tmp;
    tmp.fill(std::numeric_limits<GLuint>::max());

    auto it = boundTexture.emplace(tId, tmp);
    return it.first->second;
}

GLenum& s_blendingSource()
{
    static std::unordered_map<std::thread::id, GLenum> blendingSource;

    std::lock_guard<std::mutex> lock(GL::currentBlendingSourceMutex);
    const auto tId = std::this_thread::get_id();
    auto search = blendingSource.find(tId);
    if (search != blendingSource.end())
    {
        return search->second;
    }

    auto it = blendingSource.emplace(tId, -1);
    return it.first->second;
}

GLenum& s_blendingDest()
{
    static std::unordered_map<std::thread::id, GLenum> blendingDest;

    std::lock_guard<std::mutex> lock(GL::currentBlendingTestMutex);
    const auto tId = std::this_thread::get_id();
    auto search = blendingDest.find(tId);
    if (search != blendingDest.end())
    {
        return search->second;
    }

    auto it = blendingDest.emplace(tId, -1);
    return it.first->second;
}

int& s_GLServerState()
{
    static std::unordered_map<std::thread::id, int> GLServerState;

    std::lock_guard<std::mutex> lock(GL::GLServerStateMutex);
    const auto tId = std::this_thread::get_id();
    auto search = GLServerState.find(tId);
    if (search != GLServerState.end())
    {
        return search->second;
    }

    auto it = GLServerState.emplace(tId, 0);
    return it.first->second;
}

GLuint& s_VAO()
{
    static std::unordered_map<std::thread::id, GLuint> VAO;

    std::lock_guard<std::mutex> lock(GL::VAOMutex);
    const auto tId = std::this_thread::get_id();
    auto search = VAO.find(tId);
    if (search != VAO.end())
    {
        return search->second;
    }

    auto it = VAO.emplace(tId, 0);
    return it.first->second;
}

GLenum& s_activeTexture()
{
    static std::unordered_map<std::thread::id, GLenum> activeTexture;

    std::lock_guard<std::mutex> lock(GL::activeTextureMutex);
    const auto tId = std::this_thread::get_id();
    auto search = activeTexture.find(tId);
    if (search != activeTexture.end())
    {
        return search->second;
    }

    auto it = activeTexture.emplace(tId, -1);
    return it.first->second;
}

#endif // CC_ENABLE_GL_STATE_CACHE

// GL State Cache functions

namespace GL
{
    void initialize()
    {
        s_currentProjectionMatrix();
        s_currentProjectionMatrix();
        s_attributeFlags = 0;
#if CC_ENABLE_GL_STATE_CACHE
        s_currentShaderProgram();
        s_currentBoundTexture();
        s_blendingSource();
        s_blendingDest();
        s_GLServerState();
        s_VAO();
        s_activeTexture();
#endif
    }

    void invalidateStateCache()
    {
        Director::getInstance()->resetMatrixStack();
        s_currentProjectionMatrix() = -1;
        s_attributeFlags = 0;

#if CC_ENABLE_GL_STATE_CACHE
        s_currentShaderProgram() = -1;
        s_currentBoundTexture().fill(std::numeric_limits<GLuint>::max());

        s_blendingSource() = -1;
        s_blendingDest() = -1;
        s_GLServerState() = 0;
        s_VAO() = 0;

#endif // CC_ENABLE_GL_STATE_CACHE
    }

    void deleteProgram(GLuint program)
    {
#if CC_ENABLE_GL_STATE_CACHE
        for (auto& shaderProgram : currentShaderProgram())
        {
            if (shaderProgram.second == program)
            {
                shaderProgram.second = -1;
            }
        }
#endif // CC_ENABLE_GL_STATE_CACHE

        glDeleteProgram(program);
    }

    void useProgram(GLuint program)
    {
#if CC_ENABLE_GL_STATE_CACHE
        if (program != s_currentShaderProgram())
        {
            s_currentShaderProgram() = program;
            glUseProgram(program);
        }
#else
        glUseProgram(program);
#endif // CC_ENABLE_GL_STATE_CACHE
    }

    static void SetBlending(GLenum sfactor, GLenum dfactor)
    {
        if (sfactor == GL_ONE && dfactor == GL_ZERO)
        {
            glDisable(GL_BLEND);
            RenderState::StateBlock::_defaultState->setBlend(false);
        }
        else
        {
            glEnable(GL_BLEND);
            glBlendFunc(sfactor, dfactor);

            RenderState::StateBlock::_defaultState->setBlend(true);
            RenderState::StateBlock::_defaultState->setBlendSrc(static_cast<RenderState::Blend>(sfactor));
            RenderState::StateBlock::_defaultState->setBlendDst(static_cast<RenderState::Blend>(dfactor));
        }
    }

    void blendFunc(GLenum sfactor, GLenum dfactor)
    {
#if CC_ENABLE_GL_STATE_CACHE
        if (sfactor != s_blendingSource() || dfactor != s_blendingDest())
        {
            s_blendingSource() = sfactor;
            s_blendingDest() = dfactor;
            SetBlending(sfactor, dfactor);
        }
#else
        SetBlending(sfactor, dfactor);
#endif // CC_ENABLE_GL_STATE_CACHE
    }

    void blendResetToCache(void)
    {
        glBlendEquation(GL_FUNC_ADD);
#if CC_ENABLE_GL_STATE_CACHE
        SetBlending(s_blendingSource(), s_blendingDest());
#else
        SetBlending(CC_BLEND_SRC, CC_BLEND_DST);
#endif // CC_ENABLE_GL_STATE_CACHE
    }

    void bindTexture2D(GLuint textureId) { GL::bindTexture2DN(0, textureId); }

    void bindTexture2D(Texture2D* texture)
    {
        GL::bindTexture2DN(0, texture->getName());
        auto alphaTexID = texture->getAlphaTextureName();
        if (alphaTexID > 0)
        {
            GL::bindTexture2DN(1, alphaTexID);
        }
    }

    void bindTexture2DN(GLuint textureUnit, GLuint textureId)
    {
#if CC_ENABLE_GL_STATE_CACHE
        CCASSERT(textureUnit < MAX_ACTIVE_TEXTURE, "textureUnit is too big");
        if (s_currentBoundTexture()[textureUnit] != textureId)
        {
            s_currentBoundTexture()[textureUnit] = textureId;
            activeTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }
#else
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureId);
#endif
    }

    void bindTextureN(GLuint textureUnit, GLuint textureId, GLuint textureType /* = GL_TEXTURE_2D*/)
    {
#if CC_ENABLE_GL_STATE_CACHE
        CCASSERT(textureUnit < MAX_ACTIVE_TEXTURE, "textureUnit is too big");
        if (s_currentBoundTexture()[textureUnit] != textureId)
        {
            s_currentBoundTexture()[textureUnit] = textureId;
            activeTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(textureType, textureId);
        }
#else
        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(textureType, textureId);
#endif
    }

    void deleteTexture(GLuint textureId)
    {
#if CC_ENABLE_GL_STATE_CACHE
        for (auto& boundTexture : currentBoundTexture())
        {
            for (size_t i = 0; i < MAX_ACTIVE_TEXTURE; ++i)
            {
                if (boundTexture.second[i] == textureId)
                {
                    boundTexture.second[i] = -1;
                }
            }
        }
#endif // CC_ENABLE_GL_STATE_CACHE

        glDeleteTextures(1, &textureId);
    }

    void deleteTextureN(GLuint textureUnit, GLuint textureId) { deleteTexture(textureId); }

    void activeTexture(GLenum texture)
    {
#if CC_ENABLE_GL_STATE_CACHE
        if (s_activeTexture() != texture)
        {
            s_activeTexture() = texture;
            glActiveTexture(s_activeTexture());
        }
#else
        glActiveTexture(texture);
#endif
    }

    void bindVAO(GLuint vaoId)
    {
        if (Configuration::getInstance()->supportsShareableVAO())
        {
#if CC_ENABLE_GL_STATE_CACHE
            if (s_VAO() != vaoId)
            {
                s_VAO() = vaoId;
                glBindVertexArray(vaoId);
            }
#else
            glBindVertexArray(vaoId);
#endif // CC_ENABLE_GL_STATE_CACHE
        }
    }

    // GL Vertex Attrib functions

    void enableVertexAttribs(uint32_t flags, GLuint vao)
    {
        // Do not use the state cache in case of vao as the attrib flags context is by vao!
        if (Configuration::getInstance()->supportsShareableVAO() && vao != 0)
        {
            for (int i = 0; i < MAX_ATTRIBUTES; i++)
            {
                std::uint32_t bit = 1 << i;
                if (flags & bit)
                    glEnableVertexAttribArray(i);
            }
            return;
        }

        if (vao == 0)
            bindVAO(vao);

// Uncomment to check that the GL state is equal to the real GPU state
//#define VALIDATE_GL_STATE
#ifdef VALIDATE_GL_STATE
        uint32_t realFlags = 0;
        for (int i = 0; i < MAX_ATTRIBUTES; i++)
        {
            GLint realFlag;
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &realFlag);
            if (realFlag > 0)
                realFlags |= 1 << i;
        }
        assert(realFlags == s_attributeFlags); // The GL state cache is not up to date with the GPU state
                                               // Probably somebody called glEnableVertexAttribArray manually!!
#endif

        // hardcoded!
        for (int i = 0; i < MAX_ATTRIBUTES; i++)
        {
            uint32_t bit = 1 << i;
            // FIXME:Cache is disabled, try to enable cache as before
            bool const enabled = (flags & bit) != 0;
            bool const enabledBefore = (s_attributeFlags & bit) != 0;
            if (enabled != enabledBefore)
            {
                if (enabled)
                    glEnableVertexAttribArray(i);
                else
                    glDisableVertexAttribArray(i);
            }
        }
        s_attributeFlags = flags;
    }

    // GL Uniforms functions

    void setProjectionMatrixDirty(void) { s_currentProjectionMatrix() = -1; }

} // Namespace GL

NS_CC_END
