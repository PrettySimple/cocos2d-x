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

#include "renderer/CCRenderer.h"

#include "2d/CCCamera.h"
#include "2d/CCScene.h"
#include "base/CCConfiguration.h"
#include "base/CCDirector.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventListenerCustom.h"
#include "base/CCEventType.h"
#include "platform/CCPlatformMacros.h"
#include "renderer/CCBatchCommand.h"
#include "renderer/CCCustomCommand.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCMaterial.h"
#include "renderer/CCMeshCommand.h"
#include "renderer/CCPass.h"
#include "renderer/CCPrimitiveCommand.h"
#include "renderer/CCRenderState.h"
#include "renderer/CCTechnique.h"
#include "renderer/CCTrianglesCommand.h"
#include "renderer/ccGLStateCache.h"

#include <algorithm>
#include <limits>

NS_CC_BEGIN

// helper
static bool compareRenderCommand(RenderCommand const* a, RenderCommand const* b)
{
    return a->getGlobalOrder() < b->getGlobalOrder();
}

static bool compare3DCommand(RenderCommand const* a, RenderCommand const* b)
{
    return a->getDepth() > b->getDepth();
}

// queue

void RenderQueue::push_back(RenderCommand* command)
{
    float const z = command->getGlobalOrder();
    if (z < 0.f)
    {
        _commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_NEG)].emplace_back(command);
    }
    else if (z > 0.f)
    {
        _commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_POS)].emplace_back(command);
    }
    else
    {
        if (command->is3D())
        {
            if (command->isTransparent())
            {
                _commands[static_cast<std::size_t>(QUEUE_GROUP::TRANSPARENT_3D)].emplace_back(command);
            }
            else
            {
                _commands[static_cast<std::size_t>(QUEUE_GROUP::OPAQUE_3D)].emplace_back(command);
            }
        }
        else
        {
            _commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_ZERO)].emplace_back(command);
        }
    }
}

std::size_t RenderQueue::size() const noexcept
{
    std::size_t result = 0;
    for (auto const& cmd : _commands)
    {
        result += cmd.size();
    }
    return result;
}

void RenderQueue::sort()
{
    // Don't sort _queue0, it already comes sorted
    std::sort(_commands[static_cast<std::size_t>(QUEUE_GROUP::TRANSPARENT_3D)].begin(), _commands[static_cast<std::size_t>(QUEUE_GROUP::TRANSPARENT_3D)].end(),
              compare3DCommand);
    std::sort(_commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_NEG)].begin(), _commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_NEG)].end(),
              compareRenderCommand);
    std::sort(_commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_POS)].begin(), _commands[static_cast<std::size_t>(QUEUE_GROUP::GLOBALZ_POS)].end(),
              compareRenderCommand);
}

RenderCommand* RenderQueue::operator[](std::size_t index) const noexcept
{
    for (auto const& cmd : _commands)
    {
        if (index < cmd.size())
        {
            return cmd[index];
        }
        else
        {
            index -= cmd.size();
        }
    }

    CCASSERT(false, "invalid index");
    return nullptr;
}

void RenderQueue::clear()
{
    for (auto& cmd : _commands)
    {
        cmd.clear();
    }
}

void RenderQueue::realloc(std::size_t reserveSize)
{
    for (auto& cmd : _commands)
    {
        std::vector<RenderCommand*> vec;
        vec.reserve(reserveSize);
        cmd = vec;
    }
}

void RenderQueue::saveRenderState()
{
    _isDepthEnabled = glIsEnabled(GL_DEPTH_TEST) != GL_FALSE;
    _isCullEnabled = glIsEnabled(GL_CULL_FACE) != GL_FALSE;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &_isDepthWrite);

    CHECK_GL_ERROR_DEBUG();
}

void RenderQueue::restoreRenderState()
{
    if (_isCullEnabled)
    {
        glEnable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setCullFace(true);
    }
    else
    {
        glDisable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setCullFace(false);
    }

    if (_isDepthEnabled)
    {
        glEnable(GL_DEPTH_TEST);
        RenderState::StateBlock::_defaultState->setDepthTest(true);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        RenderState::StateBlock::_defaultState->setDepthTest(false);
    }

    glDepthMask(_isDepthWrite);
    RenderState::StateBlock::_defaultState->setDepthWrite(_isDepthEnabled);

    CHECK_GL_ERROR_DEBUG();
}

//
//
//
static constexpr Renderer::render_queue_id_t const DEFAULT_RENDER_QUEUE = 0;

//
// constructors, destructor, init
//
Renderer::Renderer()
: _triBatchesToDraw(500)
{
    _commandGroupStack.push(DEFAULT_RENDER_QUEUE);
    _renderGroups.emplace_back(RenderQueue{});
    _queuedTriangleCommands.reserve(BATCH_TRIAGCOMMAND_RESERVED_SIZE);
}

Renderer::~Renderer()
{
    _renderGroups.clear();

    glDeleteBuffers(2, _buffersVBO.data());

    if (Configuration::getInstance()->supportsShareableVAO())
    {
        glDeleteVertexArrays(1, &_buffersVAO);
        GL::bindVAO(0);
    }

#if CC_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_cacheTextureListener);
#endif
}

void Renderer::initGLView()
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    _cacheTextureListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event) {
        /** listen the event that renderer was recreated on Android/WP8 */
        this->setupBuffer();
    });

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_cacheTextureListener, -1);
#endif

    setupBuffer();

    _glViewAssigned = true;
}

void Renderer::setupBuffer()
{
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        setupVBOAndVAO();
    }
    else
    {
        setupVBO();
    }
}

void Renderer::setupVBOAndVAO()
{
    // generate vbo and vao for trianglesCommand
    glGenVertexArrays(1, &_buffersVAO);
    GL::bindVAO(_buffersVAO);

    glGenBuffers(2, &_buffersVBO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(V3F_C4B_T2F) * VBO_SIZE), reinterpret_cast<GLvoid*>(_verts.data()), GL_DYNAMIC_DRAW);

    GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POSITION | GL::VERTEX_ATTRIB_FLAG_COLOR | GL::VERTEX_ATTRIB_FLAG_TEX_COORD, _buffersVAO);
    
    // vertices
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof( V3F_C4B_T2F, vertices));

    // colors
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof( V3F_C4B_T2F, colors));

    // tex coords
    glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F), (GLvoid*) offsetof( V3F_C4B_T2F, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLushort) * INDEX_VBO_SIZE), reinterpret_cast<GLvoid*>(_indices.data()), GL_STATIC_DRAW);

    // Must unbind the VAO before changing the element buffer.
    GL::bindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void Renderer::setupVBO()
{
    glGenBuffers(2, _buffersVBO.data());
    // Issue #15652
    // Should not initialzie VBO with a large size (VBO_SIZE=65536),
    // it may cause low FPS on some Android devices like LG G4 & Nexus 5X.
    // It's probably because some implementations of OpenGLES driver will
    // copy the whole memory of VBO which initialzied at the first time
    // once glBufferData/glBufferSubData is invoked.
    // For more discussion, please refer to https://github.com/cocos2d/cocos2d-x/issues/15652
    //    mapBuffers();
}

void Renderer::mapBuffers()
{
    // Avoid changing the element buffer for whatever VAO might be bound.
    GL::bindVAO(0);

    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(V3F_C4B_T2F) * VBO_SIZE), reinterpret_cast<GLvoid*>(_verts.data()), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLushort) * INDEX_VBO_SIZE), reinterpret_cast<GLvoid*>(_indices.data()), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    CHECK_GL_ERROR_DEBUG();
}

void Renderer::addCommand(RenderCommand* command)
{
    render_queue_id_t renderQueue = _commandGroupStack.top();
    addCommand(command, renderQueue);
}

void Renderer::addCommand(RenderCommand* command, render_queue_id_t renderQueue)
{
    CCASSERT(!_isRendering, "Cannot add command while rendering");
    CCASSERT(renderQueue >= 0, "Invalid render queue");
    CCASSERT(command->getType() != RenderCommand::Type::UNKNOWN_COMMAND, "Invalid Command Type");

    _renderGroups[renderQueue].push_back(command);
}

void Renderer::pushGroup(render_queue_id_t renderQueueID)
{
    CCASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.push(renderQueueID);
}

void Renderer::popGroup()
{
    CCASSERT(!_isRendering, "Cannot change render queue while rendering");
    _commandGroupStack.pop();
}

Renderer::render_queue_id_t Renderer::createRenderQueue()
{
    _renderGroups.emplace_back(RenderQueue{});
    return static_cast<render_queue_id_t>(_renderGroups.size() - 1);
}

void Renderer::processRenderCommand(RenderCommand* command)
{
    auto const commandType = command->getType();
    if (RenderCommand::Type::TRIANGLES_COMMAND == commandType)
    {
        // flush other queues
        flush3D();

        auto cmd = static_cast<TrianglesCommand*>(command);

        // flush own queue when buffer is full
        if (_filledVertex + cmd->getVertexCount() > VBO_SIZE || _filledIndex + cmd->getIndexCount() > INDEX_VBO_SIZE)
        {
            CCASSERT(cmd->getVertexCount() >= 0 && cmd->getVertexCount() < VBO_SIZE,
                     "VBO for vertex is not big enough, please break the data down or use customized render command");
            CCASSERT(cmd->getIndexCount() >= 0 && cmd->getIndexCount() < INDEX_VBO_SIZE,
                     "VBO for index is not big enough, please break the data down or use customized render command");
            drawBatchedTriangles();
        }

        // queue it
        _queuedTriangleCommands.emplace_back(cmd);
        _filledIndex += cmd->getIndexCount();
        _filledVertex += cmd->getVertexCount();
    }
    else if (RenderCommand::Type::MESH_COMMAND == commandType)
    {
        flush2D();
        auto cmd = static_cast<MeshCommand*>(command);

        if (cmd->isSkipBatching() || _lastBatchedMeshCommand == nullptr || _lastBatchedMeshCommand->getMaterialID() != cmd->getMaterialID())
        {
            flush3D();

            CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_MESH_COMMAND");

            if (cmd->isSkipBatching())
            {
                // XXX: execute() will call bind() and unbind()
                // but unbind() shouldn't be call if the next command is a MESH_COMMAND with Material.
                // Once most of cocos2d-x moves to Pass/StateBlock, only bind() should be used.
                cmd->execute();
            }
            else
            {
                cmd->preBatchDraw();
                cmd->batchDraw();
                _lastBatchedMeshCommand = cmd;
            }
        }
        else
        {
            CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_MESH_COMMAND");
            cmd->batchDraw();
        }
    }
    else if (RenderCommand::Type::GROUP_COMMAND == commandType)
    {
        flush();
        auto renderQueueID = static_cast<GroupCommand*>(command)->getRenderQueueID();
        CCGL_DEBUG_PUSH_GROUP_MARKER("RENDERER_GROUP_COMMAND");
        visitRenderQueue(_renderGroups[renderQueueID]);
        CCGL_DEBUG_POP_GROUP_MARKER();
    }
    else if (RenderCommand::Type::CUSTOM_COMMAND == commandType)
    {
        flush();
        auto cmd = static_cast<CustomCommand*>(command);
        CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_CUSTOM_COMMAND");
        cmd->execute();
    }
    else if (RenderCommand::Type::BATCH_COMMAND == commandType)
    {
        flush();
        auto cmd = static_cast<BatchCommand*>(command);
        CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_BATCH_COMMAND");
        cmd->execute();
    }
    else if (RenderCommand::Type::PRIMITIVE_COMMAND == commandType)
    {
        flush();
        auto cmd = static_cast<PrimitiveCommand*>(command);
        CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_PRIMITIVE_COMMAND");
        cmd->execute();
    }
    else
    {
        CCLOGERROR("Unknown commands in renderQueue");
    }
}

void Renderer::visitRenderQueue(RenderQueue& queue)
{
    queue.saveRenderState();

    //
    // Process Global-Z < 0 Objects
    //
    const auto& zNegQueue = queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_NEG);
    if (zNegQueue.size() > 0)
    {
        if (_isDepthTestFor2D)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(true);
            glEnable(GL_BLEND);
            RenderState::StateBlock::_defaultState->setDepthTest(true);
            RenderState::StateBlock::_defaultState->setDepthWrite(true);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(false);
            glEnable(GL_BLEND);
            RenderState::StateBlock::_defaultState->setDepthTest(false);
            RenderState::StateBlock::_defaultState->setDepthWrite(false);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        glDisable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setCullFace(false);

        for (auto cmd : zNegQueue)
        {
            processRenderCommand(cmd);
        }
        flush();
    }

    //
    // Process Opaque Object
    //
    const auto& opaqueQueue = queue.getSubQueue(RenderQueue::QUEUE_GROUP::OPAQUE_3D);
    if (opaqueQueue.size() > 0)
    {
        // Clear depth to achieve layered rendering
        glEnable(GL_DEPTH_TEST);
        glDepthMask(true);
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setDepthTest(true);
        RenderState::StateBlock::_defaultState->setDepthWrite(true);
        RenderState::StateBlock::_defaultState->setBlend(false);
        RenderState::StateBlock::_defaultState->setCullFace(true);

        for (auto cmd : opaqueQueue)
        {
            processRenderCommand(cmd);
        }
        flush();
    }

    //
    // Process 3D Transparent object
    //
    auto const& transQueue = queue.getSubQueue(RenderQueue::QUEUE_GROUP::TRANSPARENT_3D);
    if (transQueue.size() > 0)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(false);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        RenderState::StateBlock::_defaultState->setDepthTest(true);
        RenderState::StateBlock::_defaultState->setDepthWrite(false);
        RenderState::StateBlock::_defaultState->setBlend(true);
        RenderState::StateBlock::_defaultState->setCullFace(true);

        for (auto cmd : transQueue)
        {
            processRenderCommand(cmd);
        }
        flush();
    }

    //
    // Process Global-Z = 0 Queue
    //
    const auto& zZeroQueue = queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_ZERO);
    if (zZeroQueue.size() > 0)
    {
        if (_isDepthTestFor2D)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(true);
            glEnable(GL_BLEND);

            RenderState::StateBlock::_defaultState->setDepthTest(true);
            RenderState::StateBlock::_defaultState->setDepthWrite(true);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(false);
            glEnable(GL_BLEND);

            RenderState::StateBlock::_defaultState->setDepthTest(false);
            RenderState::StateBlock::_defaultState->setDepthWrite(false);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        glDisable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setCullFace(false);

        for (auto cmd : zZeroQueue)
        {
            processRenderCommand(cmd);
        }
        flush();
    }

    //
    // Process Global-Z > 0 Queue
    //
    const auto& zPosQueue = queue.getSubQueue(RenderQueue::QUEUE_GROUP::GLOBALZ_POS);
    if (zPosQueue.size() > 0)
    {
        if (_isDepthTestFor2D)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(true);
            glEnable(GL_BLEND);

            RenderState::StateBlock::_defaultState->setDepthTest(true);
            RenderState::StateBlock::_defaultState->setDepthWrite(true);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(false);
            glEnable(GL_BLEND);

            RenderState::StateBlock::_defaultState->setDepthTest(false);
            RenderState::StateBlock::_defaultState->setDepthWrite(false);
            RenderState::StateBlock::_defaultState->setBlend(true);
        }
        glDisable(GL_CULL_FACE);
        RenderState::StateBlock::_defaultState->setCullFace(false);

        for (auto cmd : zPosQueue)
        {
            processRenderCommand(cmd);
        }
        flush();
    }

    queue.restoreRenderState();
}

void Renderer::render()
{
    // Uncomment this once everything is rendered by new renderer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: setup camera or MVP
    _isRendering = true;

    if (_glViewAssigned)
    {
        // Process render commands
        // 1. Sort render commands based on ID
        for (auto& renderqueue : _renderGroups)
        {
            renderqueue.sort();
        }
        visitRenderQueue(_renderGroups[0]);
    }
    clean();
    _isRendering = false;
}

void Renderer::clean()
{
    // Clear render group
    for (auto& renderGroup : _renderGroups)
    {
        // commands are owned by nodes
        // for (const auto &cmd : _renderGroups[j])
        // {
        //     cmd->releaseToCommandPool();
        // }
        renderGroup.clear();
    }

    // Clear batch commands
    _queuedTriangleCommands.clear();
    _filledVertex = 0;
    _filledIndex = 0;
    _lastBatchedMeshCommand = nullptr;
}

void Renderer::clear()
{
    // Enable Depth mask to make sure glClear clear the depth buffer correctly
    glDepthMask(true);
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(false);

    RenderState::StateBlock::_defaultState->setDepthWrite(false);
}

void Renderer::setDepthTest(bool enable)
{
    if (enable)
    {
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        RenderState::StateBlock::_defaultState->setDepthTest(true);
        RenderState::StateBlock::_defaultState->setDepthFunction(RenderState::DEPTH_LEQUAL);

        //        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);

        RenderState::StateBlock::_defaultState->setDepthTest(false);
    }

    _isDepthTestFor2D = enable;
    CHECK_GL_ERROR_DEBUG();
}

void Renderer::fillVerticesAndIndices(TrianglesCommand const* cmd)
{
    auto const vertices = cmd->getVertices();
    auto const verticesCount = cmd->getVertexCount();
    std::copy(vertices, vertices + verticesCount, _verts.begin() + _filledVertex);

    // fill vertex, and convert them to world coordinates
    Mat4 const& modelView = cmd->getModelView();
    for (std::size_t i = 0; i < verticesCount; ++i)
    {
        Vec3& vec1 = _verts[i + _filledVertex].vertices;
        if (std::abs(modelView.m[15] - 1.0f) >= std::numeric_limits<float>::epsilon())
        {
            Vec4 vec4 = Vec4(vec1.x, vec1.y, vec1.z, 1.0f);
            modelView.transformVector(vec4);
            vec4 = vec4 / vec4.w;
            vec1.x = vec4.x;
            vec1.y = vec4.y;
            vec1.z = vec4.z;
        }
        else
        {
            modelView.transformPoint(vec1);
        }
    }

    // fill index
    auto const indices = cmd->getIndices();
    auto const indicesCount = cmd->getIndexCount();
    for (std::size_t i = 0; i < indicesCount; ++i)
    {
        _indices[_filledIndex + i] = _filledVertex + indices[i];
    }

    _filledVertex += verticesCount;
    _filledIndex += indicesCount;
}

void Renderer::drawBatchedTriangles()
{
    if (_queuedTriangleCommands.empty())
        return;

    CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_BATCH_TRIANGLES");

    _filledVertex = 0;
    _filledIndex = 0;

    /************** 1: Setup up vertices/indices *************/

    _triBatchesToDraw[0].offset = 0;
    _triBatchesToDraw[0].indicesToDraw = 0;
    _triBatchesToDraw[0].cmd = nullptr;

    std::size_t batchesTotal = 0;
    std::size_t prevMaterialID = std::numeric_limits<std::size_t>::max();
    bool firstCommand = true;

    for (auto const& cmd : _queuedTriangleCommands)
    {
        std::size_t currentMaterialID = cmd->getMaterialID();
        bool const batchable = !cmd->isSkipBatching();

        fillVerticesAndIndices(cmd);

        // in the same batch ?
        if (batchable && (prevMaterialID == currentMaterialID || firstCommand))
        {
            CC_ASSERT(firstCommand || _triBatchesToDraw[batchesTotal].cmd->getMaterialID() == cmd->getMaterialID() && "argh... error in logic");
            _triBatchesToDraw[batchesTotal].indicesToDraw += cmd->getIndexCount();
            _triBatchesToDraw[batchesTotal].cmd = cmd;
        }
        else
        {
            // is this the first one?
            if (!firstCommand)
            {
                batchesTotal++;
                _triBatchesToDraw[batchesTotal].offset = _triBatchesToDraw[batchesTotal - 1].offset + _triBatchesToDraw[batchesTotal - 1].indicesToDraw;
            }

            _triBatchesToDraw[batchesTotal].cmd = cmd;
            _triBatchesToDraw[batchesTotal].indicesToDraw = static_cast<GLsizei>(cmd->getIndexCount());

            // is this a single batch ? Prevent creating a batch group then
            if (!batchable)
            {
                currentMaterialID = std::numeric_limits<std::size_t>::max();
            }
        }

        // capacity full ?
        if ((batchesTotal + 1) >= _triBatchesToDraw.size())
        {
            _triBatchesToDraw.resize(static_cast<std::size_t>(_triBatchesToDraw.size() * 1.4f));
        }

        prevMaterialID = currentMaterialID;
        firstCommand = false;
    }
    batchesTotal++;

    /************** 2: Copy vertices/indices to GL objects *************/

    auto const conf = Configuration::getInstance();
    if (conf->supportsShareableVAO())
    {
        // Bind VAO
        GL::bindVAO(_buffersVAO);
        // Set VBO data
        glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
        // option 1: subdata
        //        glBufferSubData(GL_ARRAY_BUFFER, sizeof(_quads[0])*start, sizeof(_quads[0]) * n , &_quads[start] );

        // option 2: data
        //        glBufferData(GL_ARRAY_BUFFER, sizeof(_verts[0]) * _filledVertex, _verts, GL_STATIC_DRAW);

        // option 3: orphaning + glMapBuffer
        // FIXME: in order to work as fast as possible, it must "and the exact same size and usage hints it had before."
        //  source: https://www.opengl.org/wiki/Buffer_Object_Streaming#Explicit_multiple_buffering
        // so most probably we won't have any benefit of using it
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(V3F_C4B_T2F) * _filledVertex), reinterpret_cast<GLvoid*>(_verts.data()), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(GLushort) * _filledIndex), reinterpret_cast<GLvoid*>(_indices.data()), GL_STATIC_DRAW);
    }
    else
    {
        // Client Side Arrays
        glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);

        glBufferData(GL_ARRAY_BUFFER, sizeof(V3F_C4B_T2F) * _filledVertex, reinterpret_cast<GLvoid*>(_verts.data()), GL_DYNAMIC_DRAW);

        GL::enableVertexAttribs(GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);

        // vertices
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F),
                              reinterpret_cast<GLvoid*>(offsetof(V3F_C4B_T2F, vertices)));

        // colors
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_C4B_T2F),
                              reinterpret_cast<GLvoid*>(offsetof(V3F_C4B_T2F, colors)));

        // tex coords
        glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_C4B_T2F),
                              reinterpret_cast<GLvoid*>(offsetof(V3F_C4B_T2F, texCoords)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * _filledIndex, reinterpret_cast<GLvoid*>(_indices.data()), GL_STATIC_DRAW);
    }

    /************** 3: Draw *************/
    for (std::size_t i = 0; i < batchesTotal; ++i)
    {
        CC_ASSERT(_triBatchesToDraw[i].cmd && "Invalid batch");
        _triBatchesToDraw[i].cmd->useMaterial();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_triBatchesToDraw[i].indicesToDraw), GL_UNSIGNED_SHORT,
                       reinterpret_cast<GLvoid*>(_triBatchesToDraw[i].offset * sizeof(GLushort)));
        _drawnBatches++;
        _drawnVertices += _triBatchesToDraw[i].indicesToDraw;
    }

    /************** 4: Cleanup *************/
    if (Configuration::getInstance()->supportsShareableVAO())
    {
        // Unbind VAO
        GL::bindVAO(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    _queuedTriangleCommands.clear();
    _filledVertex = 0;
    _filledIndex = 0;
}

void Renderer::flush()
{
    flush2D();
    flush3D();
}

void Renderer::flush2D()
{
    flushTriangles();
}

void Renderer::flush3D()
{
    if (_lastBatchedMeshCommand)
    {
        CCGL_DEBUG_INSERT_EVENT_MARKER("RENDERER_BATCH_MESH");

        _lastBatchedMeshCommand->postBatchDraw();
        _lastBatchedMeshCommand = nullptr;
    }
}

void Renderer::flushTriangles()
{
    drawBatchedTriangles();
}

// helpers
bool Renderer::checkVisibility(const Mat4& transform, const Size& size)
{
    auto scene = Director::getInstance()->getRunningScene();

    // If draw to Rendertexture, return true directly.
    // only cull the default camera. The culling algorithm is valid for default camera.
    if (!scene || (scene && scene->_defaultCamera != Camera::getVisitingCamera()))
        return true;

    auto director = Director::getInstance();
    Rect visiableRect(director->getVisibleOrigin(), director->getVisibleSize());

    // transform center point to screen space
    float hSizeX = size.width / 2;
    float hSizeY = size.height / 2;
    Vec3 v3p(hSizeX, hSizeY, 0);
    transform.transformPoint(v3p);
    Vec2 v2p = Camera::getVisitingCamera()->projectGL(v3p);

    // convert content size to world coordinates
    float wshw = std::max(std::abs(hSizeX * transform.m[0] + hSizeY * transform.m[4]), std::abs(hSizeX * transform.m[0] - hSizeY * transform.m[4]));
    float wshh = std::max(std::abs(hSizeX * transform.m[1] + hSizeY * transform.m[5]), std::abs(hSizeX * transform.m[1] - hSizeY * transform.m[5]));

    // enlarge visible rect half size in screen coord
    visiableRect.origin.x -= wshw;
    visiableRect.origin.y -= wshh;
    visiableRect.size.width += wshw * 2;
    visiableRect.size.height += wshh * 2;
    bool ret = visiableRect.containsPoint(v2p);
    return ret;
}

NS_CC_END
