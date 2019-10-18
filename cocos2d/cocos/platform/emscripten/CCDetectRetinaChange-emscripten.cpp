//
//  CCDetectRetinaChange-emscripten.cpp
//

#include <cocos/platform/CCPlatformConfig.h>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#    include "CCDetectRetinaChange-emscripten.h"
#    include <cocos/base/CCDirector.h>
#    include <cocos/base/CCEventDispatcher.h>
#    include <emscripten/emscripten.h>

#    include <chrono>

using namespace std::chrono_literals;

namespace
{
    const auto INTERVAL = 334ms;
}

NS_CC_BEGIN

DetectRetinaChange::DetectRetinaChange(const callback_t& callback)
: Node()
, _changeCallback(callback)
, _retinaFactor(static_cast<float>(emscripten_get_device_pixel_ratio()))
{
    // dirty hack to be able to schedule without having to be added to the scene
    _running = true;
    // However, checks aren't started at this point. An explicit call to resumeChecks() is required.
}

void DetectRetinaChange::pauseChecks()
{
    unscheduleAllCallbacks();
}

void DetectRetinaChange::resumeChecks()
{
    _retinaFactor = static_cast<float>(emscripten_get_device_pixel_ratio());

    schedule(CC_SCHEDULE_SELECTOR(DetectRetinaChange::_check), INTERVAL);
}

void DetectRetinaChange::_check(float)
{
    const float retinaFactor = static_cast<float>(emscripten_get_device_pixel_ratio());

    // TODO: ensure this float
    if (retinaFactor != _retinaFactor)
    {
        _retinaFactor = retinaFactor;
        _changeCallback();
    }
}

NS_CC_END

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
