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

extern "C" EM_BOOL webglContextCb(int eventType, const void*, void* userData)
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

extern "C" EM_BOOL webglFullscreenChangeCb(int eventType, const EmscriptenFullscreenChangeEvent* e, void* userData)
{
    GLViewImpl* glview = reinterpret_cast<GLViewImpl*>(userData);
    if (e->isFullscreen == EM_TRUE)
    {
        glview->_screenSizeBeforeFullscreen = glview->_screenSize * glview->_retinaFactor;
        emscripten_set_canvas_size(e->screenWidth, e->screenHeight);
        glview->updateCanvasSize(e->screenWidth, e->screenHeight);
    }
    else
    {
        emscripten_set_canvas_size(glview->_screenSizeBeforeFullscreen.width, glview->_screenSizeBeforeFullscreen.height);
        glview->updateCanvasSize(glview->_screenSizeBeforeFullscreen.width, glview->_screenSizeBeforeFullscreen.height);
    }

    return EM_TRUE;
}

extern "C" EM_BOOL wheelCb(int eventType, const EmscriptenWheelEvent* e, void* userData)
{
    return EM_TRUE;
}

KEY_NONE, "", "Unidentified"
KEY_BREAK, KEY_PAUSE, "Pause"
KEY_SCROLL_LOCK, "ScrollLock"
KEY_SYSREQ, KEY_PRINT, "PrintScreen"
KEY_ESCAPE, "Escape"
KEY_BACK = KEY_ESCAPE,
KEY_BACKSPACE, "Backspace"
KEY_TAB, "Tab"
KEY_BACK_TAB,
KEY_RETURN, "Enter"
KEY_CAPS_LOCK, "CapsLock"
KEY_SHIFT, "ShiftLeft"
KEY_LEFT_SHIFT = KEY_SHIFT,
KEY_RIGHT_SHIFT, "ShiftRight"
KEY_CTRL, "ControlLeft"
KEY_LEFT_CTRL = KEY_CTRL,
KEY_RIGHT_CTRL, "ControlRight"
KEY_ALT, "AltLeft"
KEY_LEFT_ALT = KEY_ALT,
KEY_RIGHT_ALT, "AltRight"
KEY_MENU, "ContextMenu"
KEY_HYPER,
KEY_INSERT, "Insert"
KEY_HOME, "Home"
KEY_PG_UP, "PageUp"
KEY_DELETE, "Delete"
KEY_END, "End"
KEY_PG_DOWN, "PageDown"
KEY_LEFT_ARROW, "ArrowLeft"
KEY_RIGHT_ARROW, "ArrowRight"
KEY_UP_ARROW, "ArrowUp"
KEY_DOWN_ARROW, "ArrowDown"
KEY_NUM_LOCK, "NumLock"
KEY_KP_PLUS, "NumpadAdd"
KEY_KP_MINUS, "NumpadSubtract"
KEY_KP_MULTIPLY, "NumpadMultiply"
KEY_KP_DIVIDE, "NumpadDivide"
KEY_KP_ENTER, "NumpadEnter"
KEY_KP_HOME,
KEY_KP_UP,
KEY_KP_PG_UP,
KEY_KP_LEFT,
KEY_KP_FIVE,
KEY_KP_RIGHT,
KEY_KP_END,
KEY_KP_DOWN,
KEY_KP_PG_DOWN,
KEY_KP_INSERT,
KEY_KP_DELETE, "NumpadDecimal"
KEY_F1, "F1"
KEY_F2, "F2"
KEY_F3, "F3"
KEY_F4, "F4"
KEY_F5, "F5"
KEY_F6, "F6"
KEY_F7, "F7"
KEY_F8, "F8"
KEY_F9, "F9"
KEY_F10, "F10"
KEY_F11, "F11"
KEY_F12, "F12"
KEY_SPACE, "Space"
KEY_EXCLAM, "!"
KEY_QUOTE, "\""
KEY_NUMBER,
KEY_DOLLAR, "$"
KEY_PERCENT, "%"
KEY_CIRCUMFLEX, "^"
KEY_AMPERSAND, "&"
KEY_APOSTROPHE, "'"
KEY_LEFT_PARENTHESIS, "("
KEY_RIGHT_PARENTHESIS, ")"
KEY_ASTERISK, "*"
KEY_PLUS, "+"
KEY_COMMA, ","
KEY_MINUS, "-"
KEY_PERIOD, "."
KEY_SLASH, "/"
KEY_0, "0"
KEY_1, "1"
KEY_2, "2"
KEY_3, "3"
KEY_4, "4"
KEY_5, "5"
KEY_6, "6"
KEY_7, "7"
KEY_8, "8"
KEY_9, "9"
KEY_COLON, ":"
KEY_SEMICOLON, ";"
KEY_LESS_THAN, "<"
KEY_EQUAL, "="
KEY_GREATER_THAN, ">"
KEY_QUESTION, "?"
KEY_AT, "@"
KEY_CAPITAL_A, "A"
KEY_CAPITAL_B, "B"
KEY_CAPITAL_C, "C"
KEY_CAPITAL_D, "D"
KEY_CAPITAL_E, "E"
KEY_CAPITAL_F, "F"
KEY_CAPITAL_G, "G"
KEY_CAPITAL_H, "H"
KEY_CAPITAL_I, "I"
KEY_CAPITAL_J, "J"
KEY_CAPITAL_K, "K"
KEY_CAPITAL_L, "L"
KEY_CAPITAL_M, "M"
KEY_CAPITAL_N, "N"
KEY_CAPITAL_O, "O"
KEY_CAPITAL_P, "P"
KEY_CAPITAL_Q, "Q"
KEY_CAPITAL_R, "R"
KEY_CAPITAL_S, "S"
KEY_CAPITAL_T, "T"
KEY_CAPITAL_U, "U"
KEY_CAPITAL_V, "V"
KEY_CAPITAL_W, "W"
KEY_CAPITAL_X, "X"
KEY_CAPITAL_Y, "Y"
KEY_CAPITAL_Z, "Z"
KEY_LEFT_BRACKET, "["
KEY_BACK_SLASH, "\\"
KEY_RIGHT_BRACKET, "]"
KEY_UNDERSCORE, "_"
KEY_GRAVE, "`"
KEY_A, "a"
KEY_B, "b"
KEY_C, "c"
KEY_D, "d"
KEY_E, "e"
KEY_F, "f"
KEY_G, "g"
KEY_H, "h"
KEY_I, "i"
KEY_J, "j"
KEY_K, "k"
KEY_L, "l"
KEY_M, "m"
KEY_N, "n"
KEY_O, "o"
KEY_P, "p"
KEY_Q, "q"
KEY_R, "r"
KEY_S, "s"
KEY_T, "t"
KEY_U, "u"
KEY_V, "v"
KEY_W, "w"
KEY_X, "x"
KEY_Y, "y"
KEY_Z, "z"
KEY_LEFT_BRACE, "("
KEY_BAR, "|"
KEY_RIGHT_BRACE, ")"
KEY_TILDE, "~"
KEY_EURO, "€"
KEY_POUND, "£"
KEY_YEN, "¥"
KEY_MIDDLE_DOT, "·"
KEY_SEARCH, "BrowserSearch"
KEY_DPAD_LEFT,
KEY_DPAD_RIGHT,
KEY_DPAD_UP,
KEY_DPAD_DOWN,
KEY_DPAD_CENTER,
KEY_ENTER, "Enter"
KEY_PLAY, "MediaPlayPause"

extern "C" EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{
    switch(eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
            CCLOG("key=%s | code=%s", e->key, e->code);
        break;
        case EMSCRIPTEN_EVENT_KEYUP:
        break;
    }
    return EM_TRUE;
}

extern "C" EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
    GLViewImpl* glview = reinterpret_cast<GLViewImpl*>(userData);
    float cursorX = (mouseEvent->targetX / glview->_retinaFactor);
    float cursorY = (mouseEvent->targetY / glview->_retinaFactor);

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
, _retinaFactor(emscripten_get_device_pixel_ratio())
, _captured(false)
, _screenSizeBeforeFullscreen(Size::ZERO)
{
    registerEvents();
    createEGLContext();
}
GLViewImpl::~GLViewImpl()
{
    unregisterEvents();
    deleteEGLContext();
}

void GLViewImpl::registerEvents() noexcept
{
    emscripten_set_webglcontextlost_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, &webglContextCb);
    emscripten_set_webglcontextrestored_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, &webglContextCb);

    emscripten_set_mousedown_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, &mouseCb);
    emscripten_set_mouseup_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, &mouseCb);
    emscripten_set_mousemove_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, &mouseCb);

    emscripten_set_fullscreenchange_callback("#document", reinterpret_cast<void*>(this), EM_TRUE, &webglFullscreenChangeCb);

    emscripten_set_keypress_callback("#document", reinterpret_cast<void*>(this), EM_TRUE, &keyCb);
    emscripten_set_keyup_callback("#document", reinterpret_cast<void*>(this), EM_TRUE, &keyCb);
}

void GLViewImpl::unregisterEvents() noexcept
{
    emscripten_set_webglcontextlost_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_webglcontextrestored_callback("canvas", nullptr, EM_TRUE, nullptr);

    emscripten_set_mousedown_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseup_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mousemove_callback("canvas", nullptr, EM_TRUE, nullptr);

    emscripten_set_fullscreenchange_callback("#document", nullptr, EM_TRUE, nullptr);

    emscripten_set_keypress_callback("#document", nullptr, EM_TRUE, nullptr);
    emscripten_set_keyup_callback("#document", nullptr, EM_TRUE, nullptr);
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

void GLViewImpl::setFrameZoomFactor(float zoomFactor)
{
    CCLOG("setFrameZoomFactor %f", zoomFactor);
}

void GLViewImpl::setViewPortInPoints(float x , float y , float w , float h)
{
    experimental::Viewport vp((float)(x * _scaleX * _retinaFactor + _viewPortRect.origin.x * _retinaFactor),
        (float)(y * _scaleY * _retinaFactor + _viewPortRect.origin.y * _retinaFactor),
        (float)(w * _scaleX * _retinaFactor),
        (float)(h * _scaleY * _retinaFactor));
    Camera::setDefaultViewport(vp);
}

void GLViewImpl::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor(static_cast<GLint>(std::ceil((x * _scaleX + _viewPortRect.origin.x) * _retinaFactor)),
                static_cast<GLint>(std::ceil((y * _scaleY + _viewPortRect.origin.y) * _retinaFactor)),
                static_cast<GLsizei>(std::ceil(w * _scaleX * _retinaFactor)),
                static_cast<GLsizei>(std::ceil(h * _scaleY * _retinaFactor)));
}

Rect GLViewImpl::getScissorRect() const
{
    GLfloat params[4];
    glGetFloatv(GL_SCISSOR_BOX, params);
    float x = (params[0] - _viewPortRect.origin.x * _retinaFactor) / (_scaleX * _retinaFactor);
    float y = (params[1] - _viewPortRect.origin.y * _retinaFactor) / (_scaleY * _retinaFactor);
    float w = params[2] / (_scaleX * _retinaFactor);
    float h = params[3] / (_scaleY * _retinaFactor);
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

        updateCanvasSize(width, height);
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

void GLViewImpl::updateCanvasSize(int width, int height) noexcept
{
    const bool sendEnvent = _screenSize.width > 0 && _screenSize.height > 0 && (_screenSize.width != width || _screenSize.height != height);
    setFrameSize(std::ceil(width / _retinaFactor), std::ceil(height / _retinaFactor));
    if (_resolutionPolicy != ResolutionPolicy::UNKNOWN)
    {
        Size baseDesignSize = _designResolutionSize;
        ResolutionPolicy baseResolutionPolicy = _resolutionPolicy;
        setDesignResolutionSize(baseDesignSize.width, baseDesignSize.height, baseResolutionPolicy);
    }
    Director::getInstance()->setViewport();

    if (sendEnvent)
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_RESIZED, nullptr);
}

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN