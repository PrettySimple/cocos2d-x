/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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

#include <cocos/platform/CCPlatformConfig.h>
#if defined(CC_USE_GL) || defined(CC_USE_GLES)
#include "CommandBufferGL.h"
#include "BufferGL.h"
#include "RenderPipelineGL.h"
#include "TextureGL.h"
#include "DepthStencilStateGL.h"
#include "ProgramGL.h"
#include <cocos/base/ccMacros.h>
#include <cocos/base/CCEventDispatcher.h>
#include <cocos/base/CCEventType.h>
#include <cocos/base/CCDirector.h>
#include <cocos/renderer/backend/opengl/UtilsGL.h>
#include <algorithm>

CC_BACKEND_BEGIN

namespace
{

    GLuint getHandler(TextureBackend *texture)
    {
        switch (texture->getTextureType())
        {
        case TextureType::TEXTURE_2D:
            return static_cast<Texture2DGL*>(texture)->getHandler();
        case TextureType::TEXTURE_CUBE:
            return static_cast<TextureCubeGL*>(texture)->getHandler();
        default:
            assert(false);
            return 0;
        }
    }
}

CommandBufferGL::CommandBufferGL() :
_vertexBuffer(nullptr)
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_defaultFBO);

#if CC_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*){
       if(_generatedFBO)
           glGenFramebuffers(1, &_generatedFBO); //recreate framebuffer
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

CommandBufferGL::~CommandBufferGL()
{
    glDeleteFramebuffers(1, &_generatedFBO);
    CC_SAFE_RELEASE_NULL(_renderPipeline);

    cleanResources();

#if CC_ENABLE_CACHE_TEXTURE_DATA
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
#endif
}

void CommandBufferGL::beginFrame()
{
}

void CommandBufferGL::beginRenderPass(const RenderPassDescriptor& descirptor)
{
    applyRenderPassDescriptor(descirptor);
}

void CommandBufferGL::applyRenderPassDescriptor(const RenderPassDescriptor& descirptor)
{
    if( descirptor.needColorAttachment == _passDescriptor.needColorAttachment &&
    descirptor.depthTestEnabled == _passDescriptor.depthTestEnabled &&
    descirptor.stencilTestEnabled == _passDescriptor.stencilTestEnabled &&
    descirptor.clearDepthValue == _passDescriptor.clearStencilValue &&
    descirptor.clearColorValue == _passDescriptor.clearColorValue &&
    descirptor.needClearColor == _passDescriptor.needClearColor &&
    descirptor.needClearDepth == _passDescriptor.needClearDepth &&
    descirptor.needClearStencil == _passDescriptor.needClearStencil &&
    descirptor.depthAttachmentTexture == _passDescriptor.depthAttachmentTexture &&
    descirptor.stencilAttachmentTexture == _passDescriptor.stencilAttachmentTexture &&
    descirptor.colorAttachmentsTexture[0] == _passDescriptor.colorAttachmentsTexture[0])
        return;

    _passDescriptor = descirptor;

    bool useColorAttachmentExternal = descirptor.needColorAttachment && descirptor.colorAttachmentsTexture[0];
    bool useDepthAttachmentExternal = descirptor.depthTestEnabled && descirptor.depthAttachmentTexture;
    bool useStencilAttachmentExternal = descirptor.stencilTestEnabled && descirptor.stencilAttachmentTexture;
    bool useGeneratedFBO = false;
    if (useColorAttachmentExternal || useDepthAttachmentExternal || useStencilAttachmentExternal)
    {
        if(_generatedFBO == 0)
        {
            glGenFramebuffers(1, &_generatedFBO);
        }
        _currentFBO = _generatedFBO;
        useGeneratedFBO = true;
    }
    else
    {
        _currentFBO = _defaultFBO;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, _currentFBO);
    
    if (useDepthAttachmentExternal)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D,
                               getHandler(descirptor.depthAttachmentTexture),
                               0);
        CHECK_GL_ERROR_DEBUG();

        _generatedFBOBindDepth = true;
    }
    else
    {
        if (_generatedFBOBindDepth && useGeneratedFBO)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   0,
                                   0);
            CHECK_GL_ERROR_DEBUG();

            _generatedFBOBindDepth = false;
        }
    }
        
    if (useStencilAttachmentExternal)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_STENCIL_ATTACHMENT,
                               GL_TEXTURE_2D,
                               getHandler(descirptor.depthAttachmentTexture),
                               0);
        CHECK_GL_ERROR_DEBUG();

        _generatedFBOBindStencil = true;
    }
    else
    {
        if (_generatedFBOBindStencil && useGeneratedFBO)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   0,
                                   0);
            CHECK_GL_ERROR_DEBUG();

            _generatedFBOBindStencil = false;
        }
    }
    
    if (descirptor.needColorAttachment)
    {
        int i = 0;
        for (const auto& texture : descirptor.colorAttachmentsTexture)
        {
            if (texture)
            {
                // TODO: support texture cube
                glFramebufferTexture2D(GL_FRAMEBUFFER,
                                       GL_COLOR_ATTACHMENT0 + i,
                                       GL_TEXTURE_2D,
                                       getHandler(texture),
                                       0);
            }
            CHECK_GL_ERROR_DEBUG();
            ++i;
        }

        if (useGeneratedFBO)
            _generatedFBOBindColor = true;
    }
    else
    {
        if (_generatedFBOBindColor && useGeneratedFBO)
        {
           // FIXME: Now only support attaching to attachment 0.
           glFramebufferTexture2D(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D,
                                  0,
                                  0);

            _generatedFBOBindColor = false;
        }

        // If not draw buffer is needed, should invoke this line explicitly, or it will cause
        // GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER and GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER error.
        // https://stackoverflow.com/questions/28313782/porting-opengl-es-framebuffer-to-opengl
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
#endif
    }
    CHECK_GL_ERROR_DEBUG();
    
    // set viewport and scissor in order to clear correctly
    if( descirptor.colorAttachmentsTexture[0] ) {
        auto texture = dynamic_cast<Texture2DBackend *>(descirptor.colorAttachmentsTexture[0]);
        if( texture ) {
            setViewport(0, 0, (unsigned int) texture->getWidth(),
                        (unsigned int) texture->getHeight());
            setScissorRect(false, 0, 0, 0, 0);
        }
    }
    
    // set clear color, depth and stencil
    GLbitfield mask = 0;
    if (descirptor.needClearColor)
    {
        mask |= GL_COLOR_BUFFER_BIT;
        const auto& clearColor = descirptor.clearColorValue;
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }
    
    CHECK_GL_ERROR_DEBUG();

    bool oldDepthWrite = _depthMask;
    float oldDepthClearValue = _depthClearValue;
    if (descirptor.needClearDepth)
    {
        mask |= GL_DEPTH_BUFFER_BIT;

        setDepthClearValue(descirptor.clearDepthValue);
        setDepthMask(true);
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    if (descirptor.needClearStencil)
    {
        mask |= GL_STENCIL_BUFFER_BIT;
        glClearStencil(descirptor.clearStencilValue);
    }

    if(mask) glClear(mask);
    
    CHECK_GL_ERROR_DEBUG();

    // restore depth test
    if (descirptor.needClearDepth)
    {
        if(!oldDepthWrite)
            setDepthMask(oldDepthWrite);

        if(descirptor.clearDepthValue != oldDepthClearValue)
            setDepthClearValue(oldDepthClearValue);
    }

    CHECK_GL_ERROR_DEBUG();
}

void CommandBufferGL::setRenderPipeline(RenderPipeline* renderPipeline)
{
    assert(renderPipeline != nullptr);
    if (renderPipeline == nullptr)
        return;
    
    RenderPipelineGL* rp = static_cast<RenderPipelineGL*>(renderPipeline);
    rp->retain();
    CC_SAFE_RELEASE(_renderPipeline);
    _renderPipeline = rp;
}

void CommandBufferGL::setViewport(int x, int y, unsigned int w, unsigned int h)
{
    if( x != _viewPort.x || y != _viewPort.y || w != _viewPort.w || h != _viewPort.h ) {
        glViewport(x, y, w, h);
        _viewPort.x = x;
        _viewPort.y = y;
        _viewPort.w = w;
        _viewPort.h = h;
    }
}

void CommandBufferGL::setCullMode(CullMode mode)
{
    if( mode != _cullMode ) {
        _cullMode = mode;
        _updateCullMode = true;
    }
}

void CommandBufferGL::setWinding(Winding winding)
{
    if( winding != _winding ){
        _winding = winding;
        glFrontFace(UtilsGL::toGLFrontFace(winding));
    }
}

void CommandBufferGL::setIndexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr)
        return;
    
    buffer->retain();
    CC_SAFE_RELEASE(_indexBuffer);
    _indexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setVertexBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);
    if (buffer == nullptr || _vertexBuffer == buffer)
        return;
    
    buffer->retain();
    _vertexBuffer = static_cast<BufferGL*>(buffer);
}

void CommandBufferGL::setProgramState(ProgramState* programState)
{
    CC_SAFE_RETAIN(programState);
    CC_SAFE_RELEASE(_programState);
    _programState = programState;
}

void CommandBufferGL::drawArrays(PrimitiveType primitiveType, unsigned int start,  unsigned int count)
{
    prepareDrawing();
    glDrawArrays(UtilsGL::toGLPrimitiveType(primitiveType), start, count);
    
    cleanResources();
}

void CommandBufferGL::drawElements(PrimitiveType primitiveType, IndexFormat indexType, unsigned int count, unsigned int offset)
{
    if( count <= 0 )
        return;
    
    prepareDrawing();
    _indexBuffer->uploadAndBind();
    glDrawElements(UtilsGL::toGLPrimitiveType(primitiveType), count, UtilsGL::toGLIndexType(indexType), reinterpret_cast<GLvoid*>(offset));
    CHECK_GL_ERROR_DEBUG();
    cleanResources();
}

void CommandBufferGL::endRenderPass()
{
}

void CommandBufferGL::endFrame()
{
    _usedProgram = -1;
    _boundTexture = -1;
    _updateStencilState = true;
}

void CommandBufferGL::setDepthStencilState(DepthStencilState* depthStencilState)	
{	
    if (depthStencilState && depthStencilState != _depthStencilStateGL)
    {	
        _depthStencilStateGL = static_cast<DepthStencilStateGL*>(depthStencilState);
        _updateStencilState = true;
    }	
    else if( depthStencilState != _depthStencilStateGL )
    {	
        _depthStencilStateGL = nullptr;
        _updateStencilState = true;
    }	
}

void CommandBufferGL::prepareDrawing()
{   
    const auto& program = _renderPipeline->getProgram();
    if( _usedProgram != program->getHandler() ){
        _usedProgram = program->getHandler();
        glUseProgram(program->getHandler());
    }

    bindVertexBuffer(program);
    setUniforms(program);

    // Set depth/stencil state.
    if (_depthStencilStateGL)
    {
        _depthStencilStateGL->apply(_stencilReferenceValueFront, _stencilReferenceValueBack, this);
        _updateStencilState = false;
    }
        
    else {
        setDepthTest(false);
        setStencilTest(false);
    }
    
    // Set cull mode.
    if( _updateCullMode ) {
        _updateCullMode = false;
        if (CullMode::NONE == _cullMode) {
            glDisable(GL_CULL_FACE);
        } else {
            glEnable(GL_CULL_FACE);
            glCullFace(UtilsGL::toGLCullMode(_cullMode));
        }
    }
}

void CommandBufferGL::bindVertexBuffer(ProgramGL *program)
{
    // Bind vertex buffers and set the attributes.
    auto vertexLayout = _programState->getVertexLayout();
    
    if (!vertexLayout->isValid() ||
        !_vertexBuffer)
        return;

    // Upload and bind
    _vertexBuffer->uploadAndBind();

    const auto& attributes = vertexLayout->getAttributes();

    std::set<unsigned int> toUnbind = _vertexAttribsEnabled;
    for (const auto& attributeInfo : attributes)
    {
        const auto& attribute = attributeInfo.second;
        if( !_vertexAttribsEnabled.count(attribute.index) ) {
            _vertexAttribsEnabled.insert(attribute.index);
            glEnableVertexAttribArray(attribute.index);
        }
        toUnbind.erase(attribute.index);

        glVertexAttribPointer(attribute.index,
            UtilsGL::getGLAttributeSize(attribute.format),
            UtilsGL::toGLAttributeType(attribute.format),
            attribute.needToBeNormallized,
            vertexLayout->getStride(),
            reinterpret_cast<GLvoid*>(attribute.offset));
    }

    // Unbind attribs that are not used
    for( auto e : toUnbind ){
        glDisableVertexAttribArray(e);
        _vertexAttribsEnabled.erase(e);
    }
}

void CommandBufferGL::unbindVertexBuffer(ProgramGL *program)
{
    // Bind vertex buffers and set the attributes.
    auto vertexLayout = _programState->getVertexLayout();
    
    if (!vertexLayout->isValid() ||
        !_vertexBuffer)
        return;
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CommandBufferGL::setUniforms(ProgramGL* program)
{
    if (_programState)
    {
        auto& callbacks = _programState->getCallbackUniforms();
        auto& uniformInfos = _programState->getProgram()->getAllActiveUniformInfo(ShaderStage::VERTEX);
        std::size_t bufferSize = 0;
        char* buffer = nullptr;
        _programState->getVertexUniformBuffer(&buffer, bufferSize);

        for (auto &cb : callbacks)
        {
            cb.second(_programState, cb.first);
        }

        for(auto& iter : uniformInfos)
        {
            auto& uniformInfo = iter.second;
            if(uniformInfo.size <= 0)
                continue;

            int elementCount = uniformInfo.count;
            setUniform(uniformInfo.isArray,
                uniformInfo.location,
                elementCount,
                uniformInfo.type,
                (void*)(buffer + uniformInfo.bufferOffset));
        }
        
        const auto& textureInfo = _programState->getVertexTextureInfos();
        for(const auto& iter : textureInfo)
        {
            const auto& textures = iter.second.textures;
            const auto& slot = iter.second.slot;

            int i = 0;
            for (const auto& texture: textures)
            {
                applyTexture(texture, slot[i]);
                ++i;
            }
        }
    }
}


void CommandBufferGL::applyTexture(TextureBackend* texture, int slot)
{
    switch (texture->getTextureType())
    {
        case TextureType::TEXTURE_2D: {
            auto tex = static_cast<Texture2DGL *>(texture);
            if (tex->getHandler() != _boundTexture) {
                _boundTexture = tex->getHandler();
                tex->apply(slot);
            }
        }
        break;

        case TextureType::TEXTURE_CUBE: {
            auto tex = static_cast<TextureCubeGL *>(texture);
            if (tex->getHandler() != _boundTexture) {
                _boundTexture = tex->getHandler();
                tex->apply(slot);
            }
        }
        break;

        default:
            assert(false);
            return ;
    }
}

#define DEF_TO_INT(pointer, index)     (*((GLint*)(pointer) + index))
#define DEF_TO_FLOAT(pointer, index)   (*((GLfloat*)(pointer) + index))
void CommandBufferGL::setUniform(bool isArray, GLuint location, unsigned int size, GLenum uniformType, void* data) const
{
    GLsizei count = size;
    switch (uniformType)
    {
        case GL_INT:
        case GL_BOOL:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_CUBE:
            if (isArray)
                glUniform1iv(location, count, (GLint*)data);
            else
                glUniform1i(location, DEF_TO_INT(data, 0));
            break;
        case GL_INT_VEC2:
        case GL_BOOL_VEC2:
            if (isArray)
                glUniform2iv(location, count, (GLint*)data);
            else
                glUniform2i(location, DEF_TO_INT(data, 0), DEF_TO_INT(data, 1));
            break;
        case GL_INT_VEC3:
        case GL_BOOL_VEC3:
            if (isArray)
                glUniform3iv(location, count, (GLint*)data);
            else
                glUniform3i(location,
                            DEF_TO_INT(data, 0),
                            DEF_TO_INT(data, 1),
                            DEF_TO_INT(data, 2));
            break;
        case GL_INT_VEC4:
        case GL_BOOL_VEC4:
            if (isArray)
                glUniform4iv(location, count, (GLint*)data);
            else
                glUniform4i(location,
                            DEF_TO_INT(data, 0),
                            DEF_TO_INT(data, 1),
                            DEF_TO_INT(data, 2),
                            DEF_TO_INT(data, 4));
            break;
        case GL_FLOAT:
            if (isArray)
                glUniform1fv(location, count, (GLfloat*)data);
            else
                glUniform1f(location, DEF_TO_FLOAT(data, 0));
            break;
        case GL_FLOAT_VEC2:
            if (isArray)
                glUniform2fv(location, count, (GLfloat*)data);
            else
                glUniform2f(location, DEF_TO_FLOAT(data, 0), DEF_TO_FLOAT(data, 1));
            break;
        case GL_FLOAT_VEC3:
            if (isArray)
                glUniform3fv(location, count, (GLfloat*)data);
            else
                glUniform3f(location,
                            DEF_TO_FLOAT(data, 0),
                            DEF_TO_FLOAT(data, 1),
                            DEF_TO_FLOAT(data, 2));
            break;
        case GL_FLOAT_VEC4:
            if (isArray)
                glUniform4fv(location, count, (GLfloat*)data);
            else
                glUniform4f(location,
                            DEF_TO_FLOAT(data, 0),
                            DEF_TO_FLOAT(data, 1),
                            DEF_TO_FLOAT(data, 2),
                            DEF_TO_FLOAT(data, 3));
            break;
        case GL_FLOAT_MAT2:
            glUniformMatrix2fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        case GL_FLOAT_MAT3:
            glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        case GL_FLOAT_MAT4:
            glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)data);
            break;
        break;
        
        default:
            CCASSERT(false, "invalidate Uniform data type");
        break;
    }
}

void CommandBufferGL::cleanResources()
{
    const auto& program = _renderPipeline->getProgram();
    unbindVertexBuffer(program);
    
    CC_SAFE_RELEASE_NULL(_indexBuffer);
    CC_SAFE_RELEASE_NULL(_programState);  
    CC_SAFE_RELEASE_NULL(_vertexBuffer);
}

void CommandBufferGL::setLineWidth(float lineWidth)
{
    if( lineWidth == _lineWidth )
        return;

    _lineWidth = lineWidth;

    if(lineWidth > 0.0f)
        glLineWidth(lineWidth);
    else
        glLineWidth(1.0f);
    
}


void CommandBufferGL::setScissorRect(bool isEnabled, float x, float y, float width, float height)
{
    if(isEnabled == _scissorTest && _scissorTestX == x && _scissorTestY == y && _scissorTestW == width && _scissorTestH == height)
        return;

    _scissorTest = isEnabled;
    _scissorTestX = x;
    _scissorTestY = y;
    _scissorTestW = width;
    _scissorTestH = height;

    if(isEnabled)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    }
    else
    {
        glDisable(GL_SCISSOR_TEST);
    }
}

void CommandBufferGL::captureScreen(std::function<void(const unsigned char*, int, int)> callback)
{
    int bufferSize = _viewPort.w * _viewPort.h *4;
    std::shared_ptr<GLubyte> buffer(new GLubyte[bufferSize], [](GLubyte* p){ CC_SAFE_DELETE_ARRAY(p); });
    memset(buffer.get(), 0, bufferSize);
    if (!buffer)
    {
        callback(nullptr, 0, 0);
        return;
    }
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, _viewPort.w, _viewPort.h, GL_RGBA, GL_UNSIGNED_BYTE, buffer.get());

    std::shared_ptr<GLubyte> flippedBuffer(new GLubyte[bufferSize], [](GLubyte* p) { CC_SAFE_DELETE_ARRAY(p); });
    memset(flippedBuffer.get(), 0, bufferSize);
    if (!flippedBuffer)
    {
        callback(nullptr, 0, 0);
        return;
    }
    for (int row = 0; row < _viewPort.h; ++row)
    {
        memcpy(flippedBuffer.get() + (_viewPort.h - row - 1) * _viewPort.w * 4, buffer.get() + row * _viewPort.w * 4, _viewPort.w * 4);
    }

    callback(flippedBuffer.get(), _viewPort.w, _viewPort.h);
}

void CommandBufferGL::setDepthTest(bool enabled) {
    if( _depthTest != enabled ){
        _depthTest = enabled;

        if( enabled )
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
    }
}

void CommandBufferGL::setDepthMask(uint8_t enabled) {
    if( _depthMask != enabled ){
        _depthMask = enabled;

        glDepthMask(enabled);
    }
}

void CommandBufferGL::setDepthFunc(CompareFunction function) {
    if( _depthFunction != function ){
        _depthFunction = function;

        glDepthFunc(UtilsGL::toGLComareFunction(function));
    }
}

void CommandBufferGL::setStencilTest(bool enabled) {
    if( _stencilTest != enabled ){
        _stencilTest = enabled;

        if( enabled )
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
    }
}

void CommandBufferGL::setDepthClearValue(float value) {
    if( _depthClearValue != value ){
        _depthClearValue = value;

#ifdef CC_USE_GL
        glClearDepth(value);
#else
        glClearDepthf(value);
#endif
    }
}

void CommandBufferGL::setStencilFunc(CompareFunction f, float ref, uint8_t mask){
    if( _stencilFunction != f || _stencilFunctionRef != ref || _stencilFunctionReadMask != mask ){
        _stencilFunction = f;
        _stencilFunctionRef = ref;
        _stencilFunctionReadMask = mask;

        glStencilFunc(UtilsGL::toGLComareFunction(f), ref, mask);
    }
}
void CommandBufferGL::setStencilOp(StencilOperation fail, StencilOperation zfail, StencilOperation zpass) {
    if( _stencilOpFail != fail || _stencilOpZFail != zfail || _stencilOpZPass != zpass ){
        _stencilOpFail = fail;
        _stencilOpZFail = zfail;
        _stencilOpZPass = zpass;

        glStencilOp(UtilsGL::toGLStencilOperation(fail),
                UtilsGL::toGLStencilOperation(zfail),
                UtilsGL::toGLStencilOperation(zpass));
    }
}

void CommandBufferGL::setStencilMask(uint8_t enabled) {
    if( _stencilMask != enabled ){
        _stencilMask = enabled;

        glStencilMask(enabled);
    }
}

void CommandBufferGL::setStencilFuncSeparate(CompareFunction backF, float backRef, uint8_t backMask,
                            CompareFunction frontF, float frontRef, uint8_t frontMask) {
    if( _stencilFunctionBack != backF || _stencilFunctionRefBack != backRef || _stencilFunctionReadMaskBack != backMask ||
        _stencilFunctionFront != frontF || _stencilFunctionRefFront != frontRef || _stencilFunctionReadMaskFront != frontMask){
        _stencilFunctionBack = backF;
        _stencilFunctionRefBack = backRef;
        _stencilFunctionReadMaskBack = backMask;
        _stencilFunctionFront = frontF;
        _stencilFunctionRefFront = frontRef;
        _stencilFunctionReadMaskFront = frontMask;

        glStencilFuncSeparate(GL_BACK, UtilsGL::toGLComareFunction(backF), backRef, backMask);
        glStencilFuncSeparate(GL_FRONT, UtilsGL::toGLComareFunction(frontF), frontRef, frontMask);
    }
}
void CommandBufferGL::setStencilOpSeparate(StencilOperation backFail, StencilOperation backZfail, StencilOperation backZpass,
                                  StencilOperation frontFail, StencilOperation frontZfail, StencilOperation frontZpass) {
    if( _stencilOpFailBack != backFail || _stencilOpZFailBack != backZfail || _stencilOpZPassBack != backZpass ||
        _stencilOpFailFront != frontFail || _stencilOpZFailFront != frontZfail || _stencilOpZPassFront != frontZpass){
        _stencilOpFailBack = backFail;
        _stencilOpZFailBack = backZfail;
        _stencilOpZPassBack = backZpass;
        _stencilOpFailFront = frontFail;
        _stencilOpZFailFront = frontZfail;
        _stencilOpZPassFront = frontZpass;

        glStencilOpSeparate(GL_BACK,
                UtilsGL::toGLStencilOperation(backFail),
                UtilsGL::toGLStencilOperation(backZfail),
                UtilsGL::toGLStencilOperation(backZpass));
        glStencilOpSeparate(GL_FRONT,
                            UtilsGL::toGLStencilOperation(frontFail),
                            UtilsGL::toGLStencilOperation(frontZfail),
                            UtilsGL::toGLStencilOperation(frontZpass));
    }
}
void CommandBufferGL::setStencilMaskSeparate(uint8_t back, uint8_t front) {
    if( _stencilMaskBack != back || _stencilMaskFront != front ){
        _stencilMaskBack = back;
        _stencilMaskFront = front;

        glStencilMaskSeparate(GL_BACK, back);
        glStencilMaskSeparate(GL_FRONT, front);
    }
}

CC_BACKEND_END
#endif

