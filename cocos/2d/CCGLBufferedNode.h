/****************************************************************************
Copyright (c) 2013      Zynga Inc.
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
#ifndef CC_2D_GLBUFFEREDNODE_H
#define CC_2D_GLBUFFEREDNODE_H

#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformMacros.h>

/**
 * @js NA
 */

namespace cocos2d
{
    class EventListenerCustom;
}

class GLBufferedNode
{
public:
    /**
     * @js ctor
     */
    GLBufferedNode();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~GLBufferedNode();

    /**@{
     * Load the given data into this Node's GL Buffer. Needed for WebGL, as it does not support client-side arrays.
     */
    void setGLBufferData(void* buf, GLuint bufSize, int slot);
    void setGLIndexData(void* buf, GLuint bufSize, int slot);
    void onContextRecovered() noexcept;
/**@}*/
/** Allocate 4 buffer objs per node, and index into them as slots.*/
#define BUFFER_SLOTS 4
    /**Handle for vertex buffers.*/
    GLuint _bufferObject[BUFFER_SLOTS];
    /**Size in bytes for vertex buffers.*/
    GLuint _bufferSize[BUFFER_SLOTS];
    /**Handle for index buffers.*/
    GLuint _indexBufferObject[BUFFER_SLOTS];
    /**Size in bytes for index buffers.*/
    GLuint _indexBufferSize[BUFFER_SLOTS];

#if CC_ENABLE_CACHE_TEXTURE_DATA
    cocos2d::EventListenerCustom* _onContextRecovered;
#endif

private:
    void _clearBuffers();
};
#endif // CC_2D_GLBUFFEREDNODE_H
