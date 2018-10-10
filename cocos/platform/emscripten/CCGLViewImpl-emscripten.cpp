#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    include <cocos/2d/CCCamera.h>
#    include <cocos/2d/CCDrawingPrimitives.h>
#    include <cocos/base/CCDirector.h>
#    include <cocos/base/CCEventCustom.h>
#    include <cocos/base/CCEventDispatcher.h>
#    include <cocos/base/CCEventKeyboard.h>
#    include <cocos/base/CCEventMouse.h>
#    include <cocos/base/CCEventType.h>
#    include <cocos/base/ccMacros.h>
#    include <cocos/platform/CCGL.h>
#    include <cocos/platform/emscripten/CCGLViewImpl-emscripten.h>
#    include <cocos/platform/emscripten/EMDebug.h>
#    include <cocos/renderer/CCFrameBuffer.h>
#    include <cocos/renderer/CCGLProgramCache.h>
#    include <cocos/renderer/CCRenderer.h>
#    include <cocos/renderer/CCTextureCache.h>
#    include <cocos/renderer/ccGLStateCache.h>

#    include <cassert>
#    include <cmath>
#    include <cstring>
#    include <emscripten/emscripten.h>
#    include <limits>
#    include <chrono>


NS_CC_BEGIN

/*
    This code was never tested, commenting until actually needed

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
    (void)userData; // Unused for now

    auto key = findKeyFromHTML5Key(e->key);
    if (key == EventKeyboard::KeyCode::KEY_NONE)
        key = findKeyFromHTML5Code(e->code);
    EventKeyboard event(key, EMSCRIPTEN_EVENT_KEYPRESS == eventType);
    switch(eventType)
    {
        case EMSCRIPTEN_EVENT_KEYPRESS:
            //CCLOG("[EMSCRIPTEN_EVENT_KEYPRESS] key=%s code=%s charCode=%d => key_enum=%d", e->key, e->code, e->charCode, key);
            break;
        case EMSCRIPTEN_EVENT_KEYUP:
            //CCLOG("[EMSCRIPTEN_EVENT_KEYUP] key=%s code=%s", e->key, e->code);
            break;
        case EMSCRIPTEN_EVENT_KEYDOWN:
            //CCLOG("[EMSCRIPTEN_EVENT_KEYDOWN] key=%s code=%s", e->key, e->code);
            break;
    }
    return EM_TRUE;
}

*/

GLViewImpl::GLViewImpl()
:   _display(EGL_NO_DISPLAY)
,   _context(EGL_NO_CONTEXT)
,   _surface(EGL_NO_SURFACE)
,   _config(nullptr)

,   _retinaFactor(static_cast<float>(emscripten_get_device_pixel_ratio()))
,   _retinaChangeDetector([this]() { handleRetinaFactorChange(); })

,   _currentCursorShape(CursorShape::DEFAULT)
,   _mouseMoveInjector()
,   _mouseCaptured(false)

,   _inertiaScrollX()
,   _inertiaScrollY()

,   _fullscreen(false)

{
    // Retrieve the windowed (non-fullscreen) canvas size. We prefer fetching it this way rather than passing/reading from the javascript bootstrap.
    {
        int isFullscreen;   // Ignored - starting in fullscreen isn't supported
        emscripten_get_canvas_size(&_windowedCanvasWidth, &_windowedCanvasHeight, &isFullscreen);
        // printf("### emscripten_get_canvas_size(): (%d, %d)\n", _windowedCanvasWidth, _windowedCanvasHeight);
        assert(!isFullscreen);
    }

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
    emscripten_set_webglcontextlost_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, [](int, const void*, void* userData) {
        auto glview = reinterpret_cast<GLViewImpl*>(userData);
        EMS_ASSERT_PTR(glview);
        glview->em_webglContextLostEvent();
        return EM_TRUE;
    });

    emscripten_set_webglcontextrestored_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, [](int, const void*, void* userData) {
        auto glview = reinterpret_cast<GLViewImpl*>(userData);
        EMS_ASSERT_PTR(glview);
        glview->em_webglContextRestoredEvent();
        return EM_TRUE;
    });

    emscripten_set_fullscreenchange_callback("#document", reinterpret_cast<void*>(this), EM_TRUE, [](int, const EmscriptenFullscreenChangeEvent* e, void* userData) {
        auto glview = reinterpret_cast<GLViewImpl*>(userData);
        EMS_ASSERT_PTR(glview);
        glview->em_fullscreenEvent(e);
        return EM_TRUE;
    });

    const auto mouseEventCb = [](int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
        auto glview = reinterpret_cast<GLViewImpl*>(userData);
        EMS_ASSERT_PTR(glview);
        glview->em_mouseEvent(eventType, mouseEvent);
        return EM_TRUE;
    };

    emscripten_set_mousedown_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, mouseEventCb);
    emscripten_set_mouseup_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, mouseEventCb);
    emscripten_set_mousemove_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, mouseEventCb);
    // It is important to track both mouseleave and mouseenter, as they're being fired (on some browsers...) when switching to/from fullscreen!
    emscripten_set_mouseleave_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, mouseEventCb);
    emscripten_set_mouseenter_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, mouseEventCb);

    emscripten_set_wheel_callback("canvas", reinterpret_cast<void*>(this), EM_TRUE, [](int, const EmscriptenWheelEvent* wheelEvent, void* userData) {
        auto glview = reinterpret_cast<GLViewImpl*>(userData);
        EMS_ASSERT_PTR(glview);
        glview->em_wheelEvent(wheelEvent);
        return EM_TRUE;
    });

    /* This code was never tested, commenting until actually needed
    emscripten_set_keypress_callback("#document", reinterpret_cast<void *>(this), EM_TRUE, &keyCb);
    emscripten_set_keyup_callback("#document", reinterpret_cast<void *>(this), EM_TRUE, &keyCb);
    */
}

void GLViewImpl::unregisterEvents() noexcept
{
    emscripten_set_webglcontextlost_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_webglcontextrestored_callback("canvas", nullptr, EM_TRUE, nullptr);

    emscripten_set_fullscreenchange_callback("#document", nullptr, EM_TRUE, nullptr);

    emscripten_set_mousedown_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseup_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mousemove_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseleave_callback("canvas", nullptr, EM_TRUE, nullptr);
    emscripten_set_mouseenter_callback("canvas", nullptr, EM_TRUE, nullptr);

    emscripten_set_wheel_callback("canvas", nullptr, EM_TRUE, nullptr);

    /*
    emscripten_set_keypress_callback("#document", nullptr, EM_TRUE, nullptr);
    emscripten_set_keyup_callback("#document", nullptr, EM_TRUE, nullptr);
    */
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
    return _display != EGL_NO_DISPLAY && _context != EGL_NO_CONTEXT && _surface != EGL_NO_SURFACE && _config != nullptr &&
        !emscripten_is_webgl_context_lost("canvas");
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
    (void)bOpen;
    // TODO EMSCRIPTEN: Implement
}

bool GLViewImpl::windowShouldClose()
{
    if (isOpenGLReady())
    {
        deleteEGLContext();
    }
    return true;
}

void GLViewImpl::setCursorVisible(bool isVisible)
{
    // This function is deprecated.
    // For this to behave correctly, we would need to store the shape the cursor had before going invisible.

    // Anyway, the current cursor behavior/shape implementation is simplistic (eg. we don't mess up with the pointer lock...)
    // Also note that the built-in emscripten_hide_mouse() (which, BTW, doesn't have a emscripten_show_mouse() counterpart) is (or will be) deprecated.

    setCursorShape(isVisible ? CursorShape::DEFAULT : CursorShape::NONE);
}

void GLViewImpl::setCursorShape(CursorShape shape)
{
    if (shape != _currentCursorShape)
    {
        const char* cssName;

        switch (shape)
        {
            case CursorShape::DEFAULT:
                cssName = "default";
                break;

            case CursorShape::POINTER:
                cssName = "pointer";
                break;

            case CursorShape::PROGRESS:
                cssName = "progress";
                break;

            case CursorShape::NONE:
                cssName = "none";
                break;
        }

        EM_ASM_({ document.getElementById('canvas').style.cursor = Pointer_stringify($0); }, cssName);

        _currentCursorShape = shape;
    }
}

int GLViewImpl::getRetinaFactor() const
{
    return _retinaFactor;
}

bool GLViewImpl::isRetinaDisplay() const
{
    return _retinaFactor > 1.f;
}

void GLViewImpl::setViewPortInPoints(float x, float y, float w, float h)
{
    experimental::Viewport vp((float)(x * _scaleX * _retinaFactor + _viewPortRect.origin.x * _retinaFactor),
                              (float)(y * _scaleY * _retinaFactor + _viewPortRect.origin.y * _retinaFactor), (float)(w * _scaleX * _retinaFactor),
                              (float)(h * _scaleY * _retinaFactor));
    Camera::setDefaultViewport(vp);
}

void GLViewImpl::setScissorInPoints(float x, float y, float w, float h)
{
    glScissor(static_cast<GLint>(std::ceil((x * _scaleX + _viewPortRect.origin.x) * _retinaFactor)),
              static_cast<GLint>(std::ceil((y * _scaleY + _viewPortRect.origin.y) * _retinaFactor)),
              static_cast<GLsizei>(std::ceil(w * _scaleX * _retinaFactor)), static_cast<GLsizei>(std::ceil(h * _scaleY * _retinaFactor)));
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

/************************************************************************************************************************************/
/*
    Retina handling
*/

void    GLViewImpl::applyRetinaCSSHack() noexcept
{
    // Multiplying the actual canvas size by _retinaFactor, while leaving the CSS size unchanged.
    // See: https://developer.mozilla.org/en-US/docs/Web/API/Window/devicePixelRatio

    // Must not be invoked in fullscreen mode, which is handled for us by Emscripten's fullscreen strategy
    assert(!_fullscreen);

    EM_ASM_(
        {
            (function(width, height)
            {
                'use strict';
                var canvas = document.getElementById('canvas');
                canvas.width = width;
                canvas.height = height;
            })($0, $1);
        },
        static_cast<int>(std::ceil(_windowedCanvasWidth * _retinaFactor)),
        static_cast<int>(std::ceil(_windowedCanvasHeight * _retinaFactor))
    );
}

void    GLViewImpl::handleRetinaFactorChange() noexcept
{
    /*
        We don't attempt to deal with retina change in fullscreen mode because:

            * Retina (that is, canvas-size vs css-size) is dealt with automatically by Emscripten's fullscreen strategy
            * I guess it cannot occur anyway
    */

    if(!_fullscreen)
    {
        _retinaFactor = static_cast<float>(emscripten_get_device_pixel_ratio());

        // The first approach was to emulate the context lost/restore, but it was awfully slow.
        // Surprisingly, this works as well!

        applyRetinaCSSHack();

        updateCanvasSize({ static_cast<float>(_windowedCanvasWidth), static_cast<float>(_windowedCanvasHeight) });
    }
}



/************************************************************************************************************************************/
/*
    Fullscreen management
*/

bool GLViewImpl::setFullscreen(bool fullscreen) noexcept
{
    static EmscriptenFullscreenStrategy strategy{
        // scaleMode                                    //      Result      Changes to the canvas element                   Changed to the body element
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        // EMSCRIPTEN_FULLSCREEN_SCALE_DEFAULT,         //      OK          +"background-color: black;"                     +"background-color: black;"
        EMSCRIPTEN_FULLSCREEN_SCALE_STRETCH,            //      OK          +"background-color: black;"                     +"background-color: black;"
        // EMSCRIPTEN_FULLSCREEN_SCALE_ASPECT,          //      NOK         +"padding: 0px 160px;background-color: black;"  +"background-color: black;"
        //                                                                  Removing the padding gives the same results as the other modes

        // canvasResolutionScaleMode                    //      Result
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        // EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_NONE,     //      OK
        // EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF,   //      OK
        EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF,       //      OK          Handles retina displays.

        // filteringMode
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT,
        // EMSCRIPTEN_FULLSCREEN_FILTERING_NEAREST,
        // EMSCRIPTEN_FULLSCREEN_FILTERING_BILINEAR,

        // canvasResizedCallback                        // Doesn't receive any information, invoked only when returning to windowed mode
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        nullptr,
        //[](int, const void*, void*) { printf("*** canvasResizedCallback()\n"); return EM_TRUE; },

        // canvasResizedCallbackUserData
        // ----------------------------------------------------------------------------------------------------------------------------------------------------------
        nullptr
     };

    if (fullscreen == _fullscreen)
        return false;

    // We don't change the _fullscreen flag here, it is only done in the callback

    // We're disabling the retina change detection, as we don't want it to interfer with the fullscreen change.
    // It will get re-enabled in the fullscreen callback, or immediately if our change request fails.

    _retinaChangeDetector.pauseChecks();

    bool success;

    if(fullscreen)
        success = (emscripten_request_fullscreen_strategy("canvas", EM_FALSE, &strategy) == EMSCRIPTEN_RESULT_SUCCESS);
    else
        success = (emscripten_exit_fullscreen() == EMSCRIPTEN_RESULT_SUCCESS);

    if(!success)
        _retinaChangeDetector.resumeChecks();

    return success;
}

/************************************************************************************************************************************/
/*
    em_*Event() callbacks
*/

void GLViewImpl::em_webglContextLostEvent() noexcept
{
    _retinaChangeDetector.pauseChecks();    // Restored (indirectly) by em_webglContextRestoredEvent()
    Director::getInstance()->stopAnimation();
    deleteEGLContext();
}

void GLViewImpl::em_webglContextRestoredEvent() noexcept
{
    auto director = Director::getInstance();

    GL::invalidateStateCache();
    GLProgramCache::getInstance()->reloadDefaultGLPrograms();
    DrawPrimitives::init();
    VolatileTextureMgr::reloadAllTextures();

    cocos2d::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
    director->getEventDispatcher()->dispatchEvent(&recreatedEvent);
    director->setGLDefaultValues();

    // Indirectly resumes the _retinaChangeDetector checks
    createEGLContext();

    director->startAnimation();
}

void GLViewImpl::em_fullscreenEvent(const EmscriptenFullscreenChangeEvent* e) noexcept
{
    _fullscreen = (e->isFullscreen == EM_TRUE);

    /*
        All of { e->elementWidth, e->elementHeight, e->screenWidth, e->screenHeight } seem to be set to "correct" values ONLY if e->isFullscreen;
        When returning from fullscreen, same values as when going into fullscreen are provided.
        Moreover, they seem to be CSS sizes, not pixels.
    */

    if(_fullscreen)
        updateCanvasSize({ static_cast<float>(e->screenWidth), static_cast<float>(e->screenHeight) });
    else
        updateCanvasSize({ static_cast<float>(_windowedCanvasWidth), static_cast<float>(_windowedCanvasHeight) });

    // Pause mouse move injections until the next move, as the coordinates we previously computed are no longer accurate.
    _mouseMoveInjector.pauseInject();

    // Note that some browsers fire the MOUSELEAVE events when switching the fullscreen mode (and MOUSEENTER when the mouse moves again),
    // while others don't.
    // Also, our attempts to handle it here (by faking a mouseleave event) were not successful on all browsers - some ignored
    // the css cursor switch, which became effective only after the next mouse move. This is a known issue, but solving it
    // might require a huge effort (if at all possible), hence we're postponing it...

    // => Notify the application
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_FULLSCREEN_CHANGED, reinterpret_cast<void*>(_fullscreen));

    // => Resume the retina detection checks, which was paused in setFullscreen()
    _retinaChangeDetector.resumeChecks();
}

void GLViewImpl::em_mouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent) noexcept
{
    /*
        This is quite rudimentary for now, in that that we currently only handle the left mouse button.
        Also, I doubt all the browsers behave in identical way in all situations (eg. when loosing focus), hence there might be glitches.

        Should the implementation be extended to other buttons, please be aware that:

            * Cocos' MOUSE_BUTTON_* values do NOT match EmscriptenMouseEvent::button values, except for the left button.

            * According to docs & source, EmscriptenMouseEvent::buttons is a bitmask of the pressed buttons.

                I couldn't find documentation nor enum/constants actually defining the buttons, but it appears from tests
                that 1 (one) matches the left button (perhaps it's in DOM specs?).

                This contrasts with EmscriptenMouseEvent::button (without the 's') which is set to 0 in UP/DOWN events
                when the left mouse button is [un]pressed.

            * MOUSELEAVE/MOUSEENTER implementation would need to be much more complex, tracking the state of each of the
                buttons individually


        Update 25/01/2018.

            Dispatching MOUSE_UP and MOUSE_DOWN events was useless. The whole implementation was solely based on transforming
            the browser mouse events into touch events.

            However, we now need/want to dispatch the MOUSE_MOVE events - which we already did. However, we now only want to do
            so when not in a touch event, as handling the two at a time would be a mess (<-- this is a design decision that
            restricts possibilities but makes things easier).

        Update 26/01/2018

            Adding a MOUSE_OUT event (so that we may un-highlight a highlighted element)

        Update 26/01/2018

            In order to correctly handle the fullscreen mode which eventually adds black borders, we need to ignore all mouse events
            (that is, treat them as a MOUSELEAVE) that happen outside the rendered area.

            Otherwise, events get processed for elements that are actually invisible (in the black bands) - including touchdown events.

        Update 06/03/2018

            We're now setting up a mechanism to periodically inject dummy mouse-move events, even when the mouse is actually not moving.
            This allows us to handle scene updates (eg. a button/sprite that ended being under/off the cursor, or got enabled/disabled) in a fairly easy way.

    */

    auto cursorX = static_cast<float>(mouseEvent->targetX);
    auto cursorY = static_cast<float>(mouseEvent->targetY);

    float designX, designY;
    bool mouseOutside;

    if (eventType == EMSCRIPTEN_EVENT_MOUSELEAVE)
        mouseOutside = true;
    else
    {
        /*
            We need to figure out whether the mouse is actually over the rendered area (ie. it may be over the fullscreen side black borders).
            If not, set mouseOutside to true, so that the event is treated the same as EMSCRIPTEN_EVENT_MOUSELEAVE.
        */

        const auto viewPortRect = getViewPortRect();

        // Same coordinates computations as in GLView::handleTouchesMove()

        if ((designX = (cursorX - viewPortRect.origin.x) / getScaleX()) < 0.0f || (designY = (cursorY - viewPortRect.origin.y) / getScaleY()) < 0.0f)
            mouseOutside = true;
        else
        {
            const auto& designResolutionSize = getDesignResolutionSize();
            mouseOutside = (designX > designResolutionSize.width || designY > designResolutionSize.height);
        }
    }

    if (mouseOutside)
    {
        if (_mouseCaptured)
        {
            _mouseCaptured = false;
            intptr_t id = 0;

            EM_STICKY(TOUCH);
            EM_STICKY_PRINT("handleTouchesCancel(1, %d, %f, %f)\n", id, cursorX, cursorY);

            // TODO: we're transmitting cursor (not design) coordinated here.
            // While it IS wrong, I haven't figured out yet why would the coordinates be used for a touches cancel event...

            handleTouchesCancel(1, &id, &cursorX, &cursorY);
        }

        EM_STICKY(MOUSE);
        EM_STICKY_PRINT("handleMouseOut()\n");
        handleMouseOut();
    }
    else
    {
        switch (eventType)
        {
            case EMSCRIPTEN_EVENT_MOUSEMOVE:
            {
                if (_mouseCaptured)
                {
                    intptr_t id = 0;

                    EM_STICKY(TOUCH);
                    EM_STICKY_PRINT("handleTouchesMove(1, %d, %f, %f)\n", id, cursorX, cursorY);

                    handleTouchesMove(1, &id, &cursorX, &cursorY);
                }
                else
                {
                    EM_STICKY(MOUSE);
                    EM_STICKY_PRINT("handleMouseMove(%f, %f)\n", designX, designY);

                    handleMouseMove(designX, designY);
                }
            }
            break;

            case EMSCRIPTEN_EVENT_MOUSEDOWN:
            {
                if (mouseEvent->button == 0 /*LEFT*/)
                {
                    _mouseCaptured = true;

                    intptr_t id = 0;

                    EM_STICKY(TOUCH);
                    EM_STICKY_PRINT("handleTouchesBegin(1, %d, %f, %f)\n", id, cursorX, cursorY);

                    handleTouchesBegin(1, &id, &cursorX, &cursorY);

                    // Pause injecting mouse move - since we already stop forwarding the actual moves as long as _mouseCaptured == true ...
                    // (and the injected coordinates would be wrong anyway)
                    _mouseMoveInjector.pauseInject();
                }
            }
            break;

            case EMSCRIPTEN_EVENT_MOUSEUP:
            {
                if (mouseEvent->button == 0 /*LEFT*/)
                {
                    if (_mouseCaptured)
                    {
                        _mouseCaptured = false;
                        intptr_t id = 0;

                        EM_STICKY(TOUCH);
                        EM_STICKY_PRINT("handleTouchesEnd(1, %d, %f, %f)\n", id, cursorX, cursorY);

                        handleTouchesEnd(1, &id, &cursorX, &cursorY);

                        // Also firing a single mousemove here, so that if the drag ended over a control,
                        // it gets notified.
                        EM_STICKY(MOUSE);
                        EM_STICKY_PRINT("handleMouseMove(%f, %f)\n", designX, designY);

                        handleMouseMove(designX, designY);
                    }
                }
            }
            break;
        }
    }
}

void GLViewImpl::em_wheelEvent(const EmscriptenWheelEvent* wheelEvent) noexcept
{
    // For now, ignore scrolls when the mouse is captured (touch events in progress)

    float designX, designY;

    if (!_mouseCaptured && _mouseMoveInjector.getLastKnownPosition(designX, designY))
    {
        // I couldn't find a consistent, cross platform and cross browser to-pixels multipliers for DOM_DELTA_LINE and DOM_DELTA_PAGE modes,
        // nor a way to feature-detect them.
        // Meanwhile, we're using arbitrary 12x and 120x multipliers.
        // That being said, I was unable to trigger anything else but DOM_DELTA_PIXEL...
        // EDIT: check this FB's code for normalizing: https://github.com/facebookarchive/fixed-data-table/blob/master/src/vendor_upstream/dom/normalizeWheel.js

        cocos2d::Vec2   pxDelta{ static_cast<float>(wheelEvent->deltaX), static_cast<float>(wheelEvent->deltaY) };
        const char      *deltaMode;

        (void)deltaMode;

        switch (wheelEvent->deltaMode)
        {
            case DOM_DELTA_PIXEL:
                deltaMode = "DOM_DELTA_PIXEL";
                break;

            case DOM_DELTA_LINE:
                // Using a somehow arbitrary multiplier...
                pxDelta *= 12.f;
                deltaMode = "DOM_DELTA_LINE";
                break;

            case DOM_DELTA_PAGE:
                // Using a somehow arbitrary multiplier...
                pxDelta *= 120.f;
                deltaMode = "DOM_DELTA_PAGE";
                break;

            default:
                deltaMode = "DOM_DELTA_UNKNOWN";
        }


        // Detect "inertia scroll" situations, and ignore the event altogether if we happen to be in one.

        // Both must be evaluated before being ||'d, as the function is not const.
        auto    ignoreX = _inertiaScrollX.inInertiaScroll(pxDelta.x);
        auto    ignoreY = _inertiaScrollY.inInertiaScroll(pxDelta.y);

        if(!ignoreX || !ignoreY)
        {
            const float designDeltaX = pxDelta.x / getScaleX(), designDeltaY = pxDelta.y / getScaleY();

            EM_STICKY(WHEEL);
            EM_STICKY_PRINT("wheelCb(): INPUT:  deltaX: %f, deltaY: %f, deltaZ: %f, deltaMode: %s\n", wheelEvent->deltaX, wheelEvent->deltaY, wheelEvent->deltaZ,
                            deltaMode);
            EM_STICKY_PRINT("wheelCb(): PIXELS: deltaX: %f, deltaY: %f\n", pxDelta.x, pxDelta.y);
            EM_STICKY_PRINT("wheelCb(): COCOS:  deltaX: %f, deltaY: %f\n", designDeltaX, designDeltaY);

            handleMouseScroll(designX, designY, designDeltaX, designDeltaY);
        }
        else
        {
            EM_STICKY(WHEEL);
            EM_STICKY_PRINT("wheelCb(): Detected inertia scroll, ignoring");
        }
    }
    else
    {
        EM_STICKY(WHEEL);
        EM_STICKY_PRINT("wheelCb(): Ignoring input (mouse captured or position unknown)\n");
    }
}

void GLViewImpl::handleMouseMove(float posX, float posY) noexcept
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);

    event.setCursorPosition(posX, posY);

    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

    _mouseMoveInjector.updatePosition(posX, posY);
}

void GLViewImpl::handleMouseOut() noexcept
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_OUT);

    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

    _mouseMoveInjector.pauseInject();
}

void GLViewImpl::handleMouseScroll(float posX, float posY, float deltaX, float deltaY) noexcept
{
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);

    event.setCursorPosition(posX, posY);
    event.setScrollData(deltaX, deltaY);

    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::createEGLContext() noexcept
{
    if(!_fullscreen)
        applyRetinaCSSHack();

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

    EGLint attribList[] = {EGL_RED_SIZE,
                           _glContextAttrs.redBits,
                           EGL_GREEN_SIZE,
                           _glContextAttrs.greenBits,
                           EGL_BLUE_SIZE,
                           _glContextAttrs.blueBits,
                           EGL_ALPHA_SIZE,
                           _glContextAttrs.alphaBits,
                           EGL_DEPTH_SIZE,
                           _glContextAttrs.depthBits,
                           EGL_STENCIL_SIZE,
                           _glContextAttrs.stencilBits,
                           EGL_SURFACE_TYPE,
                           EGL_WINDOW_BIT,
                           EGL_RENDERABLE_TYPE,
                           EGL_OPENGL_ES2_BIT,
                           EGL_NONE};
    ret = eglChooseConfig(_display, attribList, &_config, 1, &numConfigs);
    assert(ret == EGL_TRUE);
    assert(eglGetError() == EGL_SUCCESS);

    _surface = eglCreateWindowSurface(_display, _config, NULL, nullptr);
    assert(_surface != EGL_NO_SURFACE);
    assert(eglGetError() == EGL_SUCCESS);

    EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
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

        // Since we may be invoked from fullscreen as well (eg. due to a context loss while in fullscreen),
        // use the actual, eglQuery'd size. When doing so, we need to take the _retinaFactor into account...

        updateCanvasSize(cocos2d::Size(static_cast<float>(width), static_cast<float>(height)) / _retinaFactor);
    }

    // Retina change detection is started/resumed here
    _retinaChangeDetector.resumeChecks();
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

void GLViewImpl::updateCanvasSize(const cocos2d::Size& canvasSize) noexcept
{
    //printf("### updateCanvasSize(%f, %f)\n", canvasSize.width, canvasSize.height);

    const auto&  prevFrameSize = getFrameSize();

    setFrameSize(canvasSize.width, canvasSize.height);

    // Vary the resolution policy depending on _fullscreen.
    setDesignResolutionSize(_designResolutionSize.width, _designResolutionSize.height, _fullscreen ? ResolutionPolicy::SHOW_ALL : ResolutionPolicy::FIXED_HEIGHT);

    auto    director = Director::getInstance();

    director->setViewport();

    if(!getFrameSize().equals(prevFrameSize))
        director->getEventDispatcher()->dispatchCustomEvent(GLViewImpl::EVENT_WINDOW_RESIZED, nullptr);
}





NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
