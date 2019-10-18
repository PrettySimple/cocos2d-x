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

#ifndef CC_RENDERER_PRIMITIVECOMMAND_H
#define CC_RENDERER_PRIMITIVECOMMAND_H

#include <cocos/base/ccTypes.h>
#include <cocos/math/Mat4.h>
#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/renderer/CCRenderCommand.h>

#include <cstddef>
#include <cstdint>
#include <limits>

NS_CC_BEGIN
class GLProgramState;
class Primitive;
/**
 Command used to render primitive, similar to QuadCommand.
 Every QuadCommand will have generate material ID by give textureID, glProgramState, Blend function.
 However, primitive command could not be batched.
 */
class CC_DLL PrimitiveCommand final : public RenderCommand
{
public:
    PrimitiveCommand();
    PrimitiveCommand(PrimitiveCommand const&) = delete;
    PrimitiveCommand& operator=(PrimitiveCommand const&) = delete;
    PrimitiveCommand(PrimitiveCommand&&) noexcept = default;
    PrimitiveCommand& operator=(PrimitiveCommand&&) noexcept = delete;
    ~PrimitiveCommand() final;

    /** Initializes the command.
     @param globalOrder GlobalZOrder of the command.
     @param textureID The openGL handle of the used texture.
     @param glProgramState The specified glProgram and its uniform.
     @param blendType Blend function for the command.
     @param primitive Rendered primitive for the command.
     @param mv ModelView matrix for the command.
     @param flags to indicate that the command is using 3D rendering or not.
     */
    void init(float globalOrder, GLuint textureID, GLProgramState* glProgramState, BlendFunc blendType, Primitive* primitive, const Mat4& mv, uint32_t flags);
    CC_DEPRECATED_ATTRIBUTE void
    init(float globalOrder, GLuint textureID, GLProgramState* glProgramState, BlendFunc blendType, Primitive* primitive, const Mat4& mv);

    /**Get the generated material ID.*/
    inline std::size_t getMaterialID() const noexcept { return _materialID; }
    /**Get the texture ID used for drawing.*/
    inline GLuint getTextureID() const noexcept { return _textureID; }
    /**Get the glprogramstate used for drawing.*/
    inline GLProgramState* getGLProgramState() const noexcept { return _glProgramState; }
    /**Get the blend function for drawing.*/
    inline BlendFunc getBlendType() const noexcept { return _blendType; }
    /**Get the modelview matrix when draw the primitive.*/
    inline Mat4 const& getModelView() const noexcept { return _mv; }
    /**Execute and draw the command, called by renderer.*/
    void execute() const;

private:
    std::size_t _materialID = std::numeric_limits<std::size_t>::max();
    GLuint _textureID = 0;
    GLProgramState* _glProgramState = nullptr;
    BlendFunc _blendType = BlendFunc::DISABLE;
    Primitive* _primitive = nullptr;
    Mat4 _mv;
};

NS_CC_END

#endif // CC_RENDERER_PRIMITIVECOMMAND_H
