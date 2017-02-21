#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#include "platform/CCGL.h"
#include "platform/emscripten/CCGLViewImpl-emscripten.h"
#include "base/ccMacros.h"
#include "renderer/CCFrameBuffer.h"
#include "2d/CCCamera.h"
#include "base/CCDirector.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/ccGLStateCache.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCRenderer.h"
#include "2d/CCDrawingPrimitives.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventType.h"
#include "base/CCEventCustom.h"
#include "base/CCEventMouse.h"

#include <emscripten/emscripten.h>
#include <cassert>
#include <cmath>
#include <limits>

NS_CC_BEGIN

extern "C" EM_BOOL webglContextCb(int eventType, const void*, void *userData)
{
    GLViewImpl* glview = reinterpret_cast<GLViewImpl*>(userData);
    auto director = Director::getInstance();
    if (eventType == EMSCRIPTEN_EVENT_WEBGLCONTEXTLOST)
    {
        director->stopAnimation();
        glview->deleteEGLContext();
    }
    else if (eventType == EMSCRIPTEN_EVENT_WEBGLCONTEXTRESTORED)
    {
        GL::invalidateStateCache();
        GLProgramCache::getInstance()->reloadDefaultGLPrograms();
        DrawPrimitives::init();
        VolatileTextureMgr::reloadAllTextures();

        cocos2d::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent);
        director->setGLDefaultValues();

        glview->createEGLContext();

        director->startAnimation();
    }

    return EM_TRUE;
}

extern "C" EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    GLViewImpl* glview = reinterpret_cast<GLViewImpl*>(userData);
    float cursorX = (mouseEvent->targetX / glview->_retinaFactor);
    float cursorY = (mouseEvent->targetY / glview->_retinaFactor);
    //float cursorY = glview->_viewPortRect.origin.y + glview->_viewPortRect.size.height - (mouseEvent->targetY / glview->_retinaFactor);

    float evtCursorX = ((mouseEvent->targetX / glview->_retinaFactor) - glview->_viewPortRect.origin.x) / glview->_scaleX;
    float evtCursorY = (glview->_viewPortRect.origin.y + glview->_viewPortRect.size.height - (mouseEvent->targetY / glview->_retinaFactor)) / glview->_scaleY;
    evtCursorX /= glview->getFrameZoomFactor();
    evtCursorY /= glview->getFrameZoomFactor();

    switch (eventType)
    {
        case EMSCRIPTEN_EVENT_MOUSEDOWN:
        {
            if (mouseEvent->button == 0 /*LEFT*/)
            {
                glview->_captured = true;
                intptr_t id = 0;
                glview->handleTouchesBegin(1, &id, &cursorX, &cursorY);
            }

            EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
            event.setCursorPosition(evtCursorX, evtCursorY);
            event.setMouseButton(mouseEvent->button);
            Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
        }
        break;
        case EMSCRIPTEN_EVENT_MOUSEUP:
        {
            if (mouseEvent->button == 0 /*LEFT*/ && glview->_captured)
            {
                glview->_captured = false;
                intptr_t id = 0;
                glview->handleTouchesEnd(1, &id, &cursorX, &cursorY);
            }

            EventMouse event(EventMouse::MouseEventType::MOUSE_UP);
            event.setCursorPosition(evtCursorX, evtCursorY);
            event.setMouseButton(mouseEvent->button);
            Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
        }
        break;
        case EMSCRIPTEN_EVENT_MOUSEMOVE:
        {
            if (glview->_captured)
            {
                intptr_t id = 0;
                glview->handleTouchesMove(1, &id, &cursorX, &cursorY);
            }

            EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
            event.setCursorPosition(evtCursorX, evtCursorY);
            event.setMouseButton(mouseEvent->button);
            Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
        }
        break;
    }

    return EM_TRUE;
}

GLViewImpl::GLViewImpl() : _display(EGL_NO_DISPLAY)
, _context(EGL_NO_CONTEXT)
, _surface(EGL_NO_SURFACE)
, _config(nullptr)
, _frameZoomFactor(1.f)
, _retinaFactor(emscripten_get_device_pixel_ratio())
, _captured(false)
{
    emscripten_set_webglcontextlost_callback("canvas", reinterpret_cast<void*>(this), true, &webglContextCb);
    emscripten_set_webglcontextrestored_callback("canvas", reinterpret_cast<void*>(this), true, &webglContextCb);

    emscripten_set_mousedown_callback("canvas", reinterpret_cast<void*>(this), true, &mouseCb);
    emscripten_set_mouseup_callback("canvas", reinterpret_cast<void*>(this), true, &mouseCb);
    emscripten_set_mousemove_callback("canvas", reinterpret_cast<void*>(this), true, &mouseCb);

    createEGLContext();
}
GLViewImpl::~GLViewImpl()
{
    emscripten_set_webglcontextlost_callback("canvas", nullptr, true, nullptr);
    emscripten_set_webglcontextrestored_callback("canvas", nullptr, true, nullptr);

    emscripten_set_mousedown_callback("canvas", nullptr, true, nullptr);
    emscripten_set_mouseup_callback("canvas", nullptr, true, nullptr);
    emscripten_set_mousemove_callback("canvas", nullptr, true, nullptr);

    deleteEGLContext();
}

GLViewImpl* GLViewImpl::create(const std::string& viewName)
{
    auto ret = new (std::nothrow) GLViewImpl();
    ret->setViewName(viewName);
    ret->autorelease();
    return ret;
}

void GLViewImpl::end()
{
    if (isOpenGLReady())
    {
        deleteEGLContext();
    }
}

bool GLViewImpl::isOpenGLReady()
{
    return _display != EGL_NO_DISPLAY && _context != EGL_NO_CONTEXT && _surface != EGL_NO_SURFACE && _config != nullptr && !emscripten_is_webgl_context_lost("canvas");
}

void GLViewImpl::swapBuffers()
{
    if (isOpenGLReady())
    {
        EGLBoolean ret = eglSwapBuffers(_display, _surface);
        assert(eglGetError() == EGL_SUCCESS);
        assert(ret == EGL_TRUE);
    }
}

void GLViewImpl::setIMEKeyboardState(bool bOpen)
{
    //TODO EMSCRIPTEN: Implement
}

bool GLViewImpl::windowShouldClose()
{
    if (isOpenGLReady())
    {
        deleteEGLContext();
    }
    return true;
}

void GLViewImpl::pollEvents()
{
    //TODO EMSCRIPTEN: Implement
}

void GLViewImpl::setFrameZoomFactor(float zoomFactor)
{
    CCASSERT(zoomFactor > 0.0f, "zoomFactor must be larger than 0");
    if (std::abs(_frameZoomFactor - zoomFactor) < std::numeric_limits<float>::epsilon())
        return;
    _frameZoomFactor = zoomFactor;
}

float GLViewImpl::getFrameZoomFactor() const
{
    return _frameZoomFactor;
}

void GLViewImpl::setCursorVisible(bool isVisible)
{
    //TODO EMSCRIPTEN: Implement
}

int GLViewImpl::getRetinaFactor() const
{
    return _retinaFactor;
}

bool GLViewImpl::isRetinaDisplay() const
{
    return _retinaFactor > 1.f;
}

void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    experimental::Viewport vp((float)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor),
        (float)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor),
        (float)(w * _scaleX * _retinaFactor * _frameZoomFactor),
        (float)(h * _scaleY * _retinaFactor * _frameZoomFactor));
    Camera::setDefaultViewport(vp);
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * _scaleX * _retinaFactor * _frameZoomFactor + _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor),
               (GLint)(y * _scaleY * _retinaFactor  * _frameZoomFactor + _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor),
               (GLsizei)(w * _scaleX * _retinaFactor * _frameZoomFactor),
               (GLsizei)(h * _scaleY * _retinaFactor * _frameZoomFactor));
}

Rect GLViewImpl::getScissorRect() const
{
    GLfloat params[4];
    glGetFloatv(GL_SCISSOR_BOX, params);
    float x = (params[0] - _viewPortRect.origin.x * _retinaFactor * _frameZoomFactor) / (_scaleX * _retinaFactor * _frameZoomFactor);
    float y = (params[1] - _viewPortRect.origin.y * _retinaFactor * _frameZoomFactor) / (_scaleY * _retinaFactor  * _frameZoomFactor);
    float w = params[2] / (_scaleX * _retinaFactor * _frameZoomFactor);
    float h = params[3] / (_scaleY * _retinaFactor  * _frameZoomFactor);
    return Rect(x, y, w, h);
}

void GLViewImpl::createEGLContext() noexcept
{
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assert(_display != EGL_NO_DISPLAY);
    assert(eglGetError() == EGL_SUCCESS);

    EGLint major = 0, minor = 0;
    EGLBoolean ret = eglInitialize(_display, &major, &minor);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);
    assert(major * 10000 + minor >= 10004);

    EGLint numConfigs;
    ret = eglGetConfigs(_display, nullptr, 0, &numConfigs);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);

    EGLint attribList[] = {
        EGL_RED_SIZE,        _glContextAttrs.redBits,
        EGL_GREEN_SIZE,      _glContextAttrs.greenBits,
        EGL_BLUE_SIZE,       _glContextAttrs.blueBits,
        EGL_ALPHA_SIZE,      _glContextAttrs.alphaBits,
        EGL_DEPTH_SIZE,      _glContextAttrs.depthBits,
        EGL_STENCIL_SIZE,    _glContextAttrs.stencilBits,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    ret = eglChooseConfig(_display, attribList, &_config, 1, &numConfigs);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);

    _surface = eglCreateWindowSurface(_display, _config, NULL, nullptr);
    assert(_surface != EGL_NO_SURFACE);
    assert(eglGetError() == EGL_SUCCESS);

    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    _context = eglCreateContext(_display, _config, EGL_NO_CONTEXT, contextAttribs);
    assert(_context != EGL_NO_CONTEXT);
    assert(eglGetError() == EGL_SUCCESS);

    ret = eglMakeCurrent(_display, _surface, _surface, _context);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);

    if (_display != nullptr && _surface != nullptr)
    {
        EGLint width, height;
        eglQuerySurface(_display, _surface, EGL_WIDTH, &width);
        eglQuerySurface(_display, _surface, EGL_HEIGHT, &height);

        float frameSizeW = width / _retinaFactor * _frameZoomFactor;
        float frameSizeH = height / _retinaFactor * _frameZoomFactor;

        setFrameSize(frameSizeW, frameSizeH);
        _viewPortRect.setRect(0, 0, frameSizeW, frameSizeH);
        Director::getInstance()->setViewport();
    }
}

void GLViewImpl::deleteEGLContext() noexcept
{
    EGLBoolean ret = eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);

    ret = eglDestroyContext(_display, _context);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);
    _context = EGL_NO_CONTEXT;

    ret = eglDestroySurface(_display, _surface);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);
    _surface = EGL_NO_SURFACE;

    ret = eglTerminate(_display);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);
    _display = EGL_NO_DISPLAY;
}

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN