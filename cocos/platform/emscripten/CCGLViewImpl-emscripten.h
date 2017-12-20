#ifndef __CC_EGLViewIMPL_EMSCRIPTEN_H__
#define __CC_EGLViewIMPL_EMSCRIPTEN_H__

#include "base/CCRef.h"
#include "platform/CCCommon.h"
#include "platform/CCGLView.h"
#include "platform/CCGL.h"
#include "math/CCGeometry.h"

#include <string>
#include <emscripten/html5.h>

NS_CC_BEGIN

extern "C" EM_BOOL webglContextCb(int eventType, const void*, void* userData);
extern "C" EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
extern "C" EM_BOOL webglFullscreenChangeCb(int eventType, const EmscriptenFullscreenChangeEvent* fullscreenChangeEvent, void* userData);
extern "C" EM_BOOL wheelCb(int eventType, const EmscriptenWheelEvent* e, void* userData);
extern "C" EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

class CC_DLL GLViewImpl : public GLView
{
protected:
    GLViewImpl();
    GLViewImpl(GLViewImpl&& other) noexcept =delete;
    GLViewImpl& operator =(GLViewImpl&& other) noexcept =delete;
    GLViewImpl(const GLViewImpl& other) =delete;
    GLViewImpl& operator =(const GLViewImpl& other) =delete;
    ~GLViewImpl();

public:
    static GLViewImpl* create(const std::string& viewName);

    void end() override;
    bool isOpenGLReady() override;
    void swapBuffers() override;
    void setIMEKeyboardState(bool bOpen) override; //TODO
    bool windowShouldClose() override;
    void pollEvents() override; //TODO
    void setCursorVisible(bool isVisible) override; //TODO
    int getRetinaFactor() const override;
    bool isRetinaDisplay() const override;

    void setFrameZoomFactor(float zoomFactor) override;
    void setViewPortInPoints(float x , float y , float w , float h) override;
    void setScissorInPoints(float x , float y , float w , float h) override;
    Rect getScissorRect() const override;

private:
    friend EM_BOOL webglContextCb(int eventType, const void*, void* userData);
    friend EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
    friend EM_BOOL webglFullscreenChangeCb(int eventType, const EmscriptenFullscreenChangeEvent* fullscreenChangeEvent, void* userData);
    friend EM_BOOL wheelCb(int eventType, const EmscriptenWheelEvent* e, void* userData);
    friend EM_BOOL keyCb(int eventType, const EmscriptenKeyboardEvent* e, void* userData);

    void createEGLContext() noexcept;
    void deleteEGLContext() noexcept;

    void registerEvents() noexcept;
    void unregisterEvents() noexcept;
    void updateCanvasSize(int width, int height) noexcept;

public:
    static constexpr const char* EVENT_WINDOW_RESIZED = "glview_window_resized";
private:
    EGLDisplay _display;
    EGLContext _context;
    EGLSurface _surface;
    EGLConfig _config;

    float _retinaFactor;
	bool _mouseCaptured;
	bool _mouseBtnDown;

    cocos2d::Size _screenSizeBeforeFullscreen;
};

NS_CC_END // end of namespace cocos2d

#endif  // end of __CC_EGLViewIMPL_EMSCRIPTEN_H__
