//
//  CCDetectRetinaChange-emscripten.h
//
#pragma once

/*

This tool class runs a periodic check on retina factor, and allows us to handle the change at runtime.
It it driven by GLViewImpl.

Emscripten doesn't provide any built-in mechanism to achieve this, and their documentation actually suggests
that we implement periodic checks at runtime.

The retina change will typically happen when the browser window is being dragged from a retina to a non-retina screen, and vice-versa.

*/

#include <cocos/2d/CCNode.h>

NS_CC_BEGIN

class CC_DLL DetectRetinaChange : public Node
{
    friend class GLViewImpl;

public:
    using callback_t = std::function<void()>;

private:
    float _retinaFactor;
    callback_t _changeCallback;

private:
    DetectRetinaChange(const callback_t&);

    DetectRetinaChange() = delete;
    DetectRetinaChange(const DetectRetinaChange&) = delete;
    DetectRetinaChange& operator=(const DetectRetinaChange&) = delete;
    DetectRetinaChange(DetectRetinaChange&&) noexcept = delete;
    DetectRetinaChange& operator=(DetectRetinaChange&&) noexcept = delete;
    ~DetectRetinaChange() = default;

    // Making methods public as some are invoked from GLViewImpl's extern "C" functions.
public:
    void pauseChecks();
    void resumeChecks();

private:
    void _check(float);
};

NS_CC_END
