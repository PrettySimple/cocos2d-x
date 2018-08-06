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

#ifndef CC_RENDERER_MESHCOMMAND_H
#define CC_RENDERER_MESHCOMMAND_H

#include <cocos/math/Mat4.h>
#include <cocos/math/Vec4.h>
#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/renderer/CCRenderCommand.h>
#include <cocos/renderer/CCRenderState.h>

#include <cstddef>
#include <cstdint>
#include <limits>

NS_CC_BEGIN

class GLProgramState;
class Material;
struct BlendFunc;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
class EventCustom;
class EventListenerCustom;
#endif

// it is a common mesh
class CC_DLL MeshCommand final : public RenderCommand
{
public:
    MeshCommand();
    MeshCommand(MeshCommand const&) = delete;
    MeshCommand& operator=(MeshCommand const&) = delete;
    MeshCommand(MeshCommand&&) noexcept = delete;
    MeshCommand& operator=(MeshCommand&&) noexcept = delete;
    ~MeshCommand() final;

    void init(float globalZOrder, Material* material, GLuint vertexBuffer, GLuint indexBuffer, GLenum primitive, GLenum indexFormat, std::size_t indexCount,
              const Mat4& mv, uint32_t flags);

    void init(float globalZOrder, GLuint textureID, GLProgramState* glProgramState, RenderState::StateBlock* stateBlock, GLuint vertexBuffer,
              GLuint indexBuffer, GLenum primitive, GLenum indexFormat, std::size_t indexCount, const Mat4& mv, uint32_t flags);

    void setDisplayColor(const Vec4& color);
    void setMatrixPalette(const Vec4* matrixPalette);
    void setMatrixPaletteSize(int size);
    void setLightMask(unsigned int lightmask);

    void execute();

    // used for batch
    void preBatchDraw();
    void batchDraw();
    void postBatchDraw();

    void genMaterialID(GLuint texID, void* glProgramState, GLuint vertexBuffer, GLuint indexBuffer, BlendFunc blend);

    inline std::size_t getMaterialID() const noexcept { return _materialID; }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
    void listenRendererRecreated(EventCustom* event);
#endif

private:
    // build & release vao
    void buildVAO();
    void releaseVAO();

    // apply renderstate, not used when using material
    void applyRenderState();

    Vec4 _displayColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f); // in order to support tint and fade in fade out

    // used for skin
    Vec4 const* _matrixPalette = nullptr;
    int _matrixPaletteSize = 0;

    std::size_t _materialID = std::numeric_limits<std::size_t>::max(); // material ID

    GLuint _vao = 0; // use vao if possible

    GLuint _vertexBuffer = 0;
    GLuint _indexBuffer = 0;
    GLenum _primitive;
    GLenum _indexFormat;
    std::size_t _indexCount = 0;

    // States, default value all false

    // ModelView transform
    Mat4 _mv;

    // Mode A: Material
    // weak ref
    Material* _material = nullptr;

    // Mode B: StateBlock
    // weak ref
    GLProgramState* _glProgramState = nullptr;
    RenderState::StateBlock* _stateBlock = nullptr;
    GLuint _textureID = 0;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT || CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
    EventListenerCustom* _rendererRecreatedListener = nullptr;
#endif
};

NS_CC_END

#endif // CC_RENDERER_MESHCOMMAND_H
