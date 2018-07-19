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

#ifndef CC_RENDERER_RENDERCOMMAND_H
#define CC_RENDERER_RENDERCOMMAND_H

#include "math/Mat4.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCPlatformDefine.h"

#include <cstdint>

/**
 * @addtogroup renderer
 * @{
 */

NS_CC_BEGIN

/** Base class of the `RenderCommand` hierarchy.
*
 The `Renderer` knows how to render `RenderCommands` objects.
 */
class CC_DLL RenderCommand
{
public:
    /**Enum the type of render command. */
    enum struct Type : std::uint8_t
    {
        /** Reserved type.*/
        UNKNOWN_COMMAND,
        /** Quad command, used for draw quad.*/
        QUAD_COMMAND,
        /**Custom command, used for calling callback for rendering.*/
        CUSTOM_COMMAND,
        /**Batch command, used for draw batches in texture atlas.*/
        BATCH_COMMAND,
        /**Group command, which can group command in a tree hierarchy.*/
        GROUP_COMMAND,
        /**Mesh command, used to draw 3D meshes.*/
        MESH_COMMAND,
        /**Primitive command, used to draw primitives such as lines, points and triangles.*/
        PRIMITIVE_COMMAND,
        /**Triangles command, used to draw triangles.*/
        TRIANGLES_COMMAND
    };

public:
    /**
     Init function, will be called by all the render commands.
     @param globalZOrder The global order of command, used for rendercommand sorting.
     @param modelViewTransform Modelview matrix when submitting the render command.
     @param flags Flag used to indicate whether the command should be draw at 3D mode or not.
     */
    virtual void init(float globalZOrder, const Mat4& modelViewTransform, std::uint32_t flags);

    /** Get global Z order. */
    inline float getGlobalOrder() const noexcept { return _globalOrder; }

    /** Returns the Command type. */
    inline Type getType() const noexcept { return _type; }

    /** Returns whether is transparent. */
    inline bool isTransparent() const noexcept { return _isTransparent; }

    /** Set transparent flag. */
    inline void setTransparent(bool isTransparent) noexcept { _isTransparent = isTransparent; }
    /**
     Get skip batching status, if a rendering is skip batching, it will be forced to be rendering separately.
     */
    inline bool isSkipBatching() const noexcept { return _skipBatching; }
    /**Set skip batching.*/
    inline void setSkipBatching(bool value) noexcept { _skipBatching = value; }
    /**Whether the command should be rendered at 3D mode.*/
    inline bool is3D() const noexcept { return _is3D; }
    /**Set the command rendered in 3D mode or not.*/
    inline void set3D(bool value) noexcept { _is3D = value; }
    /**Get the depth by current model view matrix.*/
    inline float getDepth() const noexcept { return _depth; }

protected:
    RenderCommand() = default;
    RenderCommand(Type _type);
    RenderCommand(RenderCommand const&) = default;
    RenderCommand& operator=(RenderCommand const&) = delete;
    RenderCommand(RenderCommand&&) noexcept = default;
    RenderCommand& operator=(RenderCommand&&) noexcept = delete;
    virtual ~RenderCommand();

    // used for debug but it is not implemented.
    void printID();

protected:
    /**Type used in order to avoid dynamic cast, faster. */
    Type _type = Type::UNKNOWN_COMMAND;

    /** Commands are sort by global Z order. */
    float _globalOrder = 0;

    /** Transparent flag. */
    bool _isTransparent = true;

    /**
     QuadCommand and TrianglesCommand could be auto batched if there material ID is the same, however, if
     a command is skip batching, it would be forced to draw in a separate function call, and break the batch.
     */
    bool _skipBatching = false;

    /** Is the command been rendered on 3D pass. */
    bool _is3D = false;

    /** Depth from the model view matrix.*/
    float _depth = 0.f;
};

NS_CC_END
/**
 end of support group
 @}
 */
#endif // CC_RENDERER_RENDERCOMMAND_H
