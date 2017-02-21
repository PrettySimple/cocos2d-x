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
    void setFrameZoomFactor(float zoomFactor) override;
    float getFrameZoomFactor() const override;
    void setCursorVisible(bool isVisible) override; //TODO
    int getRetinaFactor() const override;
    bool isRetinaDisplay() const override;
    void setViewPortInPoints(float x , float y , float w , float h) override;
    void setScissorInPoints(float x , float y , float w , float h) override;
    Rect getScissorRect() const override;

private:
    friend EM_BOOL webglContextCb(int eventType, const void*, void* userData);
    friend EM_BOOL mouseCb(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
    void createEGLContext() noexcept;
    void deleteEGLContext() noexcept;
private:
    EGLDisplay _display;
    EGLContext _context;
    EGLSurface _surface;
    EGLConfig _config;

    float _frameZoomFactor;
    float _retinaFactor;
    bool _captured;
};

NS_CC_END // end of namespace cocos2d

#endif  // end of __CC_EGLViewIMPL_EMSCRIPTEN_H__
