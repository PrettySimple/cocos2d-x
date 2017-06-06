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
#include "base/CCEventKeyboard.h"

#include <emscripten/emscripten.h>
#include <cassert>
#include <cmath>
#include <limits>
#include <cstring>

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

static EventKeyboard::KeyCode findKeyFromHTML5Code(const char* code) noexcept
{
    static const std::unordered_map<std::string, EventKeyboard::KeyCode> keyboardCode({
        {"Unidentified", EventKeyboard::KeyCode::KEY_NONE},
        {"Pause", EventKeyboard::KeyCode::KEY_PAUSE},
        //{"Pause", EventKeyboard::KeyCode::KEY_BREAK},
        {"ScrollLock", EventKeyboard::KeyCode::KEY_SCROLL_LOCK},
        {"PrintScreen", EventKeyboard::KeyCode::KEY_PRINT},
        //{"PrintScreen", EventKeyboard::KeyCode::KEY_SYSREQ},
        {"Escape", EventKeyboard::KeyCode::KEY_ESCAPE},
        {"Backspace", EventKeyboard::KeyCode::KEY_BACKSPACE},
        {"Tab", EventKeyboard::KeyCode::KEY_TAB},
        //{"", EventKeyboard::KeyCode::KEY_BACK_TAB},
        {"Enter", EventKeyboard::KeyCode::KEY_RETURN},
        {"CapsLock", EventKeyboard::KeyCode::KEY_CAPS_LOCK},
        {"ShiftLeft", EventKeyboard::KeyCode::KEY_SHIFT},
        {"ShiftRight", EventKeyboard::KeyCode::KEY_RIGHT_SHIFT},
        {"ControlLeft", EventKeyboard::KeyCode::KEY_CTRL},
        {"ControlRight", EventKeyboard::KeyCode::KEY_RIGHT_CTRL},
        {"AltLeft", EventKeyboard::KeyCode::KEY_ALT},
        {"AltRight", EventKeyboard::KeyCode::KEY_RIGHT_ALT},
        {"ContextMenu", EventKeyboard::KeyCode::KEY_MENU},
        //{"", EventKeyboard::KeyCode::KEY_HYPER},
        {"Insert", EventKeyboard::KeyCode::KEY_INSERT},
        {"Home", EventKeyboard::KeyCode::KEY_HOME},
        {"PageUp", EventKeyboard::KeyCode::KEY_PG_UP},
        {"Delete", EventKeyboard::KeyCode::KEY_DELETE},
        {"End", EventKeyboard::KeyCode::KEY_END},
        {"PageDown", EventKeyboard::KeyCode::KEY_PG_DOWN},
        {"ArrowLeft", EventKeyboard::KeyCode::KEY_LEFT_ARROW},
        {"ArrowRight", EventKeyboard::KeyCode::KEY_RIGHT_ARROW},
        {"ArrowUp", EventKeyboard::KeyCode::KEY_UP_ARROW},
        {"ArrowDown", EventKeyboard::KeyCode::KEY_DOWN_ARROW},
        {"NumLock", EventKeyboard::KeyCode::KEY_NUM_LOCK},
        {"NumpadAdd", EventKeyboard::KeyCode::KEY_KP_PLUS},
        {"NumpadSubtract", EventKeyboard::KeyCode::KEY_KP_MINUS},
        {"NumpadMultiply", EventKeyboard::KeyCode::KEY_KP_MULTIPLY},
        {"NumpadDivide", EventKeyboard::KeyCode::KEY_KP_DIVIDE},
        {"NumpadEnter", EventKeyboard::KeyCode::KEY_KP_ENTER},
        //{"", EventKeyboard::KeyCode::KEY_KP_HOME},
        //{"", EventKeyboard::KeyCode::KEY_KP_UP},
        //{"", EventKeyboard::KeyCode::KEY_KP_PG_UP},
        //{"", EventKeyboard::KeyCode::KEY_KP_LEFT},
        //{"", EventKeyboard::KeyCode::KEY_KP_FIVE},
        //{"", EventKeyboard::KeyCode::KEY_KP_RIGHT},
        //{"", EventKeyboard::KeyCode::KEY_KP_END},
        //{"", EventKeyboard::KeyCode::KEY_KP_DOWN},
        //{"", EventKeyboard::KeyCode::KEY_KP_PG_DOWN},
        //{"", EventKeyboard::KeyCode::KEY_KP_INSERT},
        {"NumpadDecimal", EventKeyboard::KeyCode::KEY_KP_DELETE},
        {"F1", EventKeyboard::KeyCode::KEY_F1},
        {"F2", EventKeyboard::KeyCode::KEY_F2},
        {"F3", EventKeyboard::KeyCode::KEY_F3},
        {"F4", EventKeyboard::KeyCode::KEY_F4},
        {"F5", EventKeyboard::KeyCode::KEY_F5},
        {"F6", EventKeyboard::KeyCode::KEY_F6},
        {"F7", EventKeyboard::KeyCode::KEY_F7},
        {"F8", EventKeyboard::KeyCode::KEY_F8},
        {"F9", EventKeyboard::KeyCode::KEY_F9},
        {"F10", EventKeyboard::KeyCode::KEY_F10},
        {"F11", EventKeyboard::KeyCode::KEY_F11},
        {"F12", EventKeyboard::KeyCode::KEY_F12},
        //{"", EventKeyboard::KeyCode::KEY_NUMBER},
        {"BrowserSearch", EventKeyboard::KeyCode::KEY_SEARCH},
        //{"", EventKeyboard::KeyCode::KEY_DPAD_LEFT},
        //{"", EventKeyboard::KeyCode::KEY_DPAD_RIGHT},
        //{"", EventKeyboard::KeyCode::KEY_DPAD_UP},
        //{"", EventKeyboard::KeyCode::KEY_DPAD_DOWN},
        //{"", EventKeyboard::KeyCode::KEY_DPAD_CENTER},
        {"Enter", EventKeyboard::KeyCode::KEY_ENTER},
        {"MediaPlayPause", EventKeyboard::KeyCode::KEY_PLAY}
    });

    auto search = keyboardCode.find(code);
    if (search != keyboardCode.end())
        return search->second;

    return EventKeyboard::KeyCode::KEY_NONE;
}

static EventKeyboard::KeyCode findKeyFromHTML5Key(const char* key) noexcept
{
    static const std::unordered_map<std::string, EventKeyboard::KeyCode> keyboardKey({
        {" ", EventKeyboard::KeyCode::KEY_SPACE},
        {"!", EventKeyboard::KeyCode::KEY_EXCLAM},
        {"\"", EventKeyboard::KeyCode::KEY_QUOTE},
        {"$", EventKeyboard::KeyCode::KEY_DOLLAR},
        {"%", EventKeyboard::KeyCode::KEY_PERCENT},
        {"^", EventKeyboard::KeyCode::KEY_CIRCUMFLEX},
        {"&", EventKeyboard::KeyCode::KEY_AMPERSAND},
        {"\'", EventKeyboard::KeyCode::KEY_APOSTROPHE},
        {"(", EventKeyboard::KeyCode::KEY_LEFT_PARENTHESIS},
        {")", EventKeyboard::KeyCode::KEY_RIGHT_PARENTHESIS},
        {"*", EventKeyboard::KeyCode::KEY_ASTERISK},
        {"+", EventKeyboard::KeyCode::KEY_PLUS},
        {",", EventKeyboard::KeyCode::KEY_COMMA},
        {"-", EventKeyboard::KeyCode::KEY_MINUS},
        {".", EventKeyboard::KeyCode::KEY_PERIOD},
        {"/", EventKeyboard::KeyCode::KEY_SLASH},
        {"0", EventKeyboard::KeyCode::KEY_0},
        {"1", EventKeyboard::KeyCode::KEY_1},
        {"2", EventKeyboard::KeyCode::KEY_2},
        {"3", EventKeyboard::KeyCode::KEY_3},
        {"4", EventKeyboard::KeyCode::KEY_4},
        {"5", EventKeyboard::KeyCode::KEY_5},
        {"6", EventKeyboard::KeyCode::KEY_6},
        {"7", EventKeyboard::KeyCode::KEY_7},
        {"8", EventKeyboard::KeyCode::KEY_8},
        {"9", EventKeyboard::KeyCode::KEY_9},
        {":", EventKeyboard::KeyCode::KEY_COLON},
        {";", EventKeyboard::KeyCode::KEY_SEMICOLON},
        {"<", EventKeyboard::KeyCode::KEY_LESS_THAN},
        {"=", EventKeyboard::KeyCode::KEY_EQUAL},
        {">", EventKeyboard::KeyCode::KEY_GREATER_THAN},
        {"?", EventKeyboard::KeyCode::KEY_QUESTION},
        {"@", EventKeyboard::KeyCode::KEY_AT},
        {"A", EventKeyboard::KeyCode::KEY_CAPITAL_A},
        {"B", EventKeyboard::KeyCode::KEY_CAPITAL_B},
        {"C", EventKeyboard::KeyCode::KEY_CAPITAL_C},
        {"D", EventKeyboard::KeyCode::KEY_CAPITAL_D},
        {"E", EventKeyboard::KeyCode::KEY_CAPITAL_E},
        {"F", EventKeyboard::KeyCode::KEY_CAPITAL_F},
        {"G", EventKeyboard::KeyCode::KEY_CAPITAL_G},
        {"H", EventKeyboard::KeyCode::KEY_CAPITAL_H},
        {"I", EventKeyboard::KeyCode::KEY_CAPITAL_I},
        {"J", EventKeyboard::KeyCode::KEY_CAPITAL_J},
        {"K", EventKeyboard::KeyCode::KEY_CAPITAL_K},
        {"L", EventKeyboard::KeyCode::KEY_CAPITAL_L},
        {"M", EventKeyboard::KeyCode::KEY_CAPITAL_M},
        {"N", EventKeyboard::KeyCode::KEY_CAPITAL_N},
        {"O", EventKeyboard::KeyCode::KEY_CAPITAL_O},
        {"P", EventKeyboard::KeyCode::KEY_CAPITAL_P},
        {"Q", EventKeyboard::KeyCode::KEY_CAPITAL_Q},
        {"R", EventKeyboard::KeyCode::KEY_CAPITAL_R},
        {"S", EventKeyboard::KeyCode::KEY_CAPITAL_S},
        {"T", EventKeyboard::KeyCode::KEY_CAPITAL_T},
        {"U", EventKeyboard::KeyCode::KEY_CAPITAL_U},
        {"V", EventKeyboard::KeyCode::KEY_CAPITAL_V},
        {"W", EventKeyboard::KeyCode::KEY_CAPITAL_W},
        {"X", EventKeyboard::KeyCode::KEY_CAPITAL_X},
        {"Y", EventKeyboard::KeyCode::KEY_CAPITAL_Y},
        {"Z", EventKeyboard::KeyCode::KEY_CAPITAL_Z},
        {"[", EventKeyboard::KeyCode::KEY_LEFT_BRACKET},
        {"\\", EventKeyboard::KeyCode::KEY_BACK_SLASH},
        {"]", EventKeyboard::KeyCode::KEY_RIGHT_BRACKET},
        {"_", EventKeyboard::KeyCode::KEY_UNDERSCORE},
        {"`", EventKeyboard::KeyCode::KEY_GRAVE},
        {"a", EventKeyboard::KeyCode::KEY_A},
        {"b", EventKeyboard::KeyCode::KEY_B},
        {"c", EventKeyboard::KeyCode::KEY_C},
        {"d", EventKeyboard::KeyCode::KEY_D},
        {"e", EventKeyboard::KeyCode::KEY_E},
        {"f", EventKeyboard::KeyCode::KEY_F},
        {"g", EventKeyboard::KeyCode::KEY_G},
        {"h", EventKeyboard::KeyCode::KEY_H},
        {"i", EventKeyboard::KeyCode::KEY_I},
        {"j", EventKeyboard::KeyCode::KEY_J},
        {"k", EventKeyboard::KeyCode::KEY_K},
        {"l", EventKeyboard::KeyCode::KEY_L},
        {"m", EventKeyboard::KeyCode::KEY_M},
        {"n", EventKeyboard::KeyCode::KEY_N},
        {"o", EventKeyboard::KeyCode::KEY_O},
        {"p", EventKeyboard::KeyCode::KEY_P},
        {"q", EventKeyboard::KeyCode::KEY_Q},
        {"r", EventKeyboard::KeyCode::KEY_R},
        {"s", EventKeyboard::KeyCode::KEY_S},
        {"t", EventKeyboard::KeyCode::KEY_T},
        {"u", EventKeyboard::KeyCode::KEY_U},
        {"v", EventKeyboard::KeyCode::KEY_V},
        {"w", EventKeyboard::KeyCode::KEY_W},
        {"x", EventKeyboard::KeyCode::KEY_X},
        {"y", EventKeyboard::KeyCode::KEY_Y},
        {"z", EventKeyboard::KeyCode::KEY_Z},
        {"(", EventKeyboard::KeyCode::KEY_LEFT_BRACE},
        {"|", EventKeyboard::KeyCode::KEY_BAR},
        {")", EventKeyboard::KeyCode::KEY_RIGHT_BRACE},
        {"~", EventKeyboard::KeyCode::KEY_TILDE},
        {"€", EventKeyboard::KeyCode::KEY_EURO},
        {"£", EventKeyboard::KeyCode::KEY_POUND},
        {"¥", EventKeyboard::KeyCode::KEY_YEN},
        {"·", EventKeyboard::KeyCode::KEY_MIDDLE_DOT}
    });

    auto search = keyboardKey.find(key);
    if (search != keyboardKey.end())
        return search->second;

    return EventKeyboard::KeyCode::KEY_NONE;
}

extern "C" EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent* e, void* userData)
{
    auto key = findKeyFromHTML5Key(e->key);
    if (key == EventKeyboard::KeyCode::KEY_NONE)
        key = findKeyFromHTML5Code(e->code);
    EventKeyboard event(g_keyCodeMap[key], GLFW_PRESS == action);
    switch(eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
        {

            CCLOG("[EMSCRIPTEN_EVENT_KEYPRESS] key=%s code=%s charCode=%d => key_enum=%d", e->key, e->code, e->charCode, key);
            break;
        }
        case EMSCRIPTEN_EVENT_KEYUP:
            CCLOG("[EMSCRIPTEN_EVENT_KEYUP] key=%s code=%s", e->key, e->code);
        break;
        case EMSCRIPTEN_EVENT_KEYDOWN:
            CCLOG("[EMSCRIPTEN_EVENT_KEYDOWN] key=%s code=%s", e->key, e->code);
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