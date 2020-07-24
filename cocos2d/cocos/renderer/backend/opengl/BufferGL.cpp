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
#include "BufferGL.h"
#include <cassert>
#include <cocos/base/ccMacros.h>
#include <cocos/base/CCDirector.h>
#include <cocos/base/CCEventType.h>
#include <cocos/base/CCEventDispatcher.h>

CC_BACKEND_BEGIN

namespace {
    GLenum toGLUsage(const BufferUsage& usage)
    {
        switch (usage)
        {
            case BufferUsage::STATIC:
                return GL_STATIC_DRAW;
            case BufferUsage::DYNAMIC:
                return GL_DYNAMIC_DRAW;
            default:
                return GL_DYNAMIC_DRAW;
        }
    }
}

BufferGL::BufferGL(unsigned int size, BufferType type, BufferUsage usage)
: Buffer(size, type, usage)
{
    glGenBuffers(1, &_buffer);

#if CC_ENABLE_CACHE_TEXTURE_DATA
    _backToForegroundListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom*){
        this->reloadBuffer();
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_backToForegroundListener, -1);
#endif
}

BufferGL::~BufferGL()
{
    if (_buffer)
        glDeleteBuffers(1, &_buffer);

    CC_SAFE_DELETE_ARRAY(_data);
    Director::getInstance()->getEventDispatcher()->removeEventListener(_backToForegroundListener);
}

void BufferGL::usingDefaultStoredData(bool needDefaultStoredData)
{
    _needDefaultStoredData = needDefaultStoredData;
}

#if CC_ENABLE_CACHE_TEXTURE_DATA
void BufferGL::reloadBuffer()
{
    glGenBuffers(1, &_buffer);

    if(!_needDefaultStoredData)
        return;

    _bufferAlreadyFilled = true;
    updateData(_data, _bufferAllocated);
}
#endif

void BufferGL::updateData(void* data, unsigned long size)
{
    if( size == 0 )
        return;

    assert(size && size <= _size);
    if( size != _bufferAllocated ) {
        _bufferAllocated = size;
        CC_SAFE_DELETE_ARRAY(_data);
        _data = new(std::nothrow) char[_bufferAllocated];
    }

    memcpy(_data, data, size);
    _dirty = true;
}

void BufferGL::updateSubData(void* data, unsigned long offset, unsigned long size)
{
    if( size == 0 )
        return;

    CCASSERT(_bufferAllocated != 0, "updateData should be invoke before updateSubData");
    CCASSERT(offset + size <= _size, "buffer size overflow");
    CCASSERT(offset + size <= _bufferAllocated, "buffer size overflow");

    memcpy(_data, data, size);
    _dirty = true;
}

void BufferGL::uploadAndBind()
{
    if (_buffer) {
        if (BufferType::VERTEX == _type) {
            glBindBuffer(GL_ARRAY_BUFFER, _buffer);
            if( _dirty )
                glBufferData(GL_ARRAY_BUFFER, _bufferAllocated, _data, toGLUsage(_usage));
        } else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffer);
            if( _dirty )
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _bufferAllocated, _data, toGLUsage(_usage));
        }
        CHECK_GL_ERROR_DEBUG();
    }
}

CC_BACKEND_END
#endif
