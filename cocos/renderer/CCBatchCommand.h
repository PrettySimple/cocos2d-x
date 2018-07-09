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

#ifndef CC_RENDERER_BATCHCOMMAND_H
#define CC_RENDERER_BATCHCOMMAND_H

#include "base/ccTypes.h"
#include "renderer/CCRenderCommand.h"

NS_CC_BEGIN

class TextureAtlas;
class GLProgram;

/**
Command used to draw batches in one TextureAtlas.
*/
class CC_DLL BatchCommand final : public RenderCommand
{
public:
    BatchCommand();
    BatchCommand(BatchCommand const&) = delete;
    BatchCommand& operator=(BatchCommand const&) = delete;
    BatchCommand(BatchCommand&&) noexcept = delete;
    BatchCommand& operator=(BatchCommand&&) noexcept = delete;
    ~BatchCommand() final;
    /**Init the batch command.
    @param globalZOrder GlobalZOrder of the render command.
    @param shader Shader used for draw the texture atlas.
    @param blendType Blend function for texture atlas.
    @param textureAtlas Texture atlas, will contain both quads and texture for rendering.
    @param modelViewTransform Model view transform used for rendering.
    @param flags Indicate the render command should be rendered in 3D mode or not.
    */
    void init(float globalZOrder, GLProgram* shader, BlendFunc blendType, TextureAtlas* textureAtlas, const Mat4& modelViewTransform, uint32_t flags);

    /**Execute the command, which will call openGL function to draw the texture atlas.*/
    void execute();

protected:
    /**Texture ID used for texture atlas rendering.*/
    GLuint _textureID = 0;
    /**Shaders used for rendering.*/
    GLProgram* _shader = nullptr;
    /**Blend function for rendering.*/
    BlendFunc _blendType = BlendFunc::DISABLE;
    /**Texture atlas for rendering.*/
    TextureAtlas* _textureAtlas = nullptr;

    /**ModelView transform.*/
    Mat4 _mv;
};

NS_CC_END

#endif // CC_RENDERER_BATCHCOMMAND_H
