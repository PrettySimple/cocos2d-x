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

#ifndef CC_RENDERER_RENDERER_H
#define CC_RENDERER_RENDERER_H

#include <cocos/base/ccTypes.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/math/Mat4.h>
#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <array>
#include <cstddef>
#include <stack>
#include <vector>

#if !defined(NDEBUG) && CC_TARGET_PLATFORM == CC_PLATFORM_IOS

/// Basic wrapper for glInsertEventMarkerEXT() depending on the current build settings and platform.
#    define CCGL_DEBUG_INSERT_EVENT_MARKER(__message__) glInsertEventMarkerEXT(0, __message__)
/// Basic wrapper for glPushGroupMarkerEXT() depending on the current build settings and platform.
#    define CCGL_DEBUG_PUSH_GROUP_MARKER(__message__) glPushGroupMarkerEXT(0, __message__)
/// Basic wrapper for CCGL_DEBUG_POP_GROUP_MARKER() depending on the current build settings and platform.
#    define CCGL_DEBUG_POP_GROUP_MARKER() glPopGroupMarkerEXT()

#else

#    define CCGL_DEBUG_INSERT_EVENT_MARKER(__message__)
#    define CCGL_DEBUG_PUSH_GROUP_MARKER(__message__)
#    define CCGL_DEBUG_POP_GROUP_MARKER()

#endif

/**
 * @addtogroup renderer
 * @{
 */

NS_CC_BEGIN

class MeshCommand;
class RenderCommand;
class TrianglesCommand;

/** Class that knows how to sort `RenderCommand` objects.
 Since the commands that have `z == 0` are "pushed back" in
 the correct order, the only `RenderCommand` objects that need to be sorted,
 are the ones that have `z < 0` and `z > 0`.
*/
class RenderQueue final
{
public:
    /**
    RenderCommand will be divided into Queue Groups.
    */
    enum struct QUEUE_GROUP : std::uint8_t
    {
        /**Objects with globalZ smaller than 0.*/
        GLOBALZ_NEG = 0,
        /**Opaque 3D objects with 0 globalZ.*/
        OPAQUE_3D,
        /**Transparent 3D objects with 0 globalZ.*/
        TRANSPARENT_3D,
        /**2D objects with 0 globalZ.*/
        GLOBALZ_ZERO,
        /**Objects with globalZ bigger than 0.*/
        GLOBALZ_POS,

        MAX
    };

private:
    /**The commands in the render queue.*/
    std::array<std::vector<RenderCommand*>, static_cast<std::size_t>(QUEUE_GROUP::MAX)> _commands;

    /**Cull state.*/
    bool _isCullEnabled = false;
    /**Depth test enable state.*/
    bool _isDepthEnabled = false;
    /**Depth buffer write state.*/
    GLboolean _isDepthWrite = false;

public:
    RenderQueue() = default;
    RenderQueue(RenderQueue const&) = delete;
    RenderQueue& operator=(RenderQueue const&) = delete;
    RenderQueue(RenderQueue&&) noexcept = default;
    RenderQueue& operator=(RenderQueue&&) noexcept = delete;
    ~RenderQueue() = default;

    /**Push a renderCommand into current renderqueue.*/
    void push_back(RenderCommand* command);
    /**Return the number of render commands.*/
    std::size_t size() const noexcept;
    /**Sort the render commands.*/
    void sort();
    /**Treat sorted commands as an array, access them one by one.*/
    RenderCommand* operator[](std::size_t index) const noexcept;
    /**Clear all rendered commands.*/
    void clear();
    /**Realloc command queues and reserve with given size. Note: this clears any existing commands.*/
    void realloc(std::size_t reserveSize);
    /**Get a sub group of the render queue.*/
    inline std::vector<RenderCommand*>& getSubQueue(QUEUE_GROUP group) noexcept { return _commands[static_cast<std::size_t>(group)]; }
    /**Get the number of render commands contained in a subqueue.*/
    inline std::size_t getSubQueueSize(QUEUE_GROUP group) const noexcept { return _commands[static_cast<std::size_t>(group)].size(); }

    /**Save the current DepthState, CullState, DepthWriteState render state.*/
    void saveRenderState();
    /**Restore the saved DepthState, CullState, DepthWriteState render state.*/
    void restoreRenderState();
};

/* Class responsible for the rendering in.

Whenever possible prefer to use `TrianglesCommand` objects since the renderer will automatically batch them.
 */
class CC_DLL Renderer final
{
public:
    using render_queue_id_t = std::size_t;

    /**The max number of vertices in a vertex buffer object.*/
    static constexpr std::size_t const VBO_SIZE = 65536;
    /**The max number of indices in a index buffer.*/
    static constexpr std::size_t const INDEX_VBO_SIZE = VBO_SIZE * 6 / 4;
    /**The rendercommands which can be batched will be saved into a list, this is the reserved size of this list.*/
    static constexpr std::size_t const BATCH_TRIAGCOMMAND_RESERVED_SIZE = 64;
    /**Reserved for material id, which means that the command could not be batched.*/
    static constexpr std::size_t const MATERIAL_ID_DO_NOT_BATCH = 0;

    Renderer();
    Renderer(Renderer const&) = delete;
    Renderer& operator=(Renderer const&) = delete;
    Renderer(Renderer&&) noexcept = delete;
    Renderer& operator=(Renderer&&) noexcept = delete;
    ~Renderer();

    // TODO: manage GLView inside Render itself
    void initGLView();

    /** Adds a `RenderComamnd` into the renderer */
    void addCommand(RenderCommand* command);

    /** Adds a `RenderComamnd` into the renderer specifying a particular render queue ID */
    void addCommand(RenderCommand* command, render_queue_id_t renderQueue);

    /** Pushes a group into the render queue */
    void pushGroup(render_queue_id_t renderQueueID);

    /** Pops a group from the render queue */
    void popGroup();

    /** Creates a render queue and returns its Id */
    render_queue_id_t createRenderQueue();

    /** Renders into the GLView all the queued `RenderCommand` objects */
    void render();

    /** Cleans all `RenderCommand`s in the queue */
    void clean();

    /** Clear GL buffer and screen */
    void clear();

    /** set color for clear screen */
    inline void setClearColor(Color4F const& clearColor) noexcept { _clearColor = clearColor; }
    /* returns the number of drawn batches in the last frame */
    inline std::size_t getDrawnBatches() const noexcept { return _drawnBatches; }
    /* RenderCommands (except) TrianglesCommand should update this value */
    inline void addDrawnBatches(std::size_t number) { _drawnBatches += number; }
    /* returns the number of drawn triangles in the last frame */
    inline std::size_t getDrawnVertices() const noexcept { return _drawnVertices; }
    /* RenderCommands (except) TrianglesCommand should update this value */
    inline void addDrawnVertices(std::size_t number) noexcept { _drawnVertices += number; }
    /* clear draw stats */
    inline void clearDrawStats() noexcept
    {
        _drawnBatches = 0;
        _drawnVertices = 0;
    }

    /**
     * Enable/Disable depth test
     * For 3D object depth test is enabled by default and can not be changed
     * For 2D object depth test is disabled by default
     */
    void setDepthTest(bool enable);

    /** returns whether or not a rectangle is visible or not */
    bool checkVisibility(Mat4 const& transform, Size const& size);

protected:
    // Setup VBO or VAO based on OpenGL extensions
    void setupBuffer();
    void setupVBOAndVAO();
    void setupVBO();
    void mapBuffers();
    void drawBatchedTriangles();

    // Draw the previews queued triangles and flush previous context
    void flush();

    void flush2D();

    void flush3D();

    void flushTriangles();

    void processRenderCommand(RenderCommand* command);
    void visitRenderQueue(RenderQueue& queue);

    void fillVerticesAndIndices(TrianglesCommand const* cmd);

    /* clear color set outside be used in setGLDefaultValues() */
    Color4F _clearColor = Color4F::BLACK;

    std::stack<render_queue_id_t> _commandGroupStack;

    std::vector<RenderQueue> _renderGroups;

    MeshCommand* _lastBatchedMeshCommand = nullptr;
    std::vector<TrianglesCommand*> _queuedTriangleCommands;

    // for TrianglesCommand
    std::array<V3F_C4B_T2F, VBO_SIZE> _verts;
    std::array<GLushort, INDEX_VBO_SIZE> _indices;
    GLuint _buffersVAO = 0;
    std::array<GLuint, 2> _buffersVBO; // 0: vertex  1: indices

    // Internal structure that has the information for the batches
    struct TriBatchToDraw
    {
        TrianglesCommand* cmd = nullptr; // needed for the Material
        GLsizei indicesToDraw = 0;
        GLsizei offset = 0;
    };
    // the TriBatches
    std::vector<TriBatchToDraw> _triBatchesToDraw;

    std::size_t _filledVertex = 0;
    std::size_t _filledIndex = 0;

    bool _glViewAssigned = false;

    // stats
    std::size_t _drawnBatches = 0;
    std::size_t _drawnVertices = 0;
    // the flag for checking whether renderer is rendering
    bool _isRendering = false;

    bool _isDepthTestFor2D = false;

#if CC_ENABLE_CACHE_TEXTURE_DATA
    EventListenerCustom* _cacheTextureListener = nullptr;
#endif
};

NS_CC_END

/**
 end of support group
 @}
 */
#endif // CC_RENDERER_RENDERER_H
