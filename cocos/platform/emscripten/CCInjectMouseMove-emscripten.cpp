//
//  CCInjectMouseMove-emscripten.cpp
//

#include "platform/CCPlatformConfig.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#    include "CCInjectMouseMove-emscripten.h"
#    include "base/CCDirector.h"
#    include "base/CCEventDispatcher.h"

#    include <chrono>

using namespace std::chrono_literals;

namespace
{
    const auto INTERVAL = 125ms;
}

NS_CC_BEGIN

InjectMouseMove::InjectMouseMove()
: Node()
, _designX(0.f)
, _designY(0.f)
, _injecting(false)
{
    // dirty hack to be able to schedule without having to be added to the scene
    _running = true;
}

InjectMouseMove::~InjectMouseMove()
{
}

void InjectMouseMove::updatePosition(float designX, float designY)
{
    _designX = designX;
    _designY = designY;

    // Always unschedule first, as we want the injection to start INTERVAL time after the last actual mouse move
    if (_injecting)
        unscheduleAllCallbacks();

    schedule(CC_SCHEDULE_SELECTOR(InjectMouseMove::_inject), INTERVAL);
    _injecting = true;
}

bool InjectMouseMove::getLastKnownPosition(float& designX, float& designY) const noexcept
{
    if (_injecting)
    {
        designX = _designX;
        designY = _designY;
        return true;
    }

    return false;
}

void InjectMouseMove::pauseInject()
{
    unscheduleAllCallbacks();
    _injecting = false;
}

void InjectMouseMove::_inject(float)
{
    assert(_injecting);
    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    event.setCursorPosition(_designX, _designY);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

NS_CC_END

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
