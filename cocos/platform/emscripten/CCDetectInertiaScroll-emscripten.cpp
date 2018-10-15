//
//  CCDetectInertiaScroll-emscripten.cpp
//

#include <cocos/platform/CCPlatformConfig.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)

#include <cmath>
#include <cocos/platform/CCPlatformMacros.h>
#include "CCDetectInertiaScroll-emscripten.h"

using namespace std::chrono_literals;


NS_CC_BEGIN


DetectInertiaScroll::DetectInertiaScroll()
:   _inInertiaState(false)
,   _subsequentLowerDeltaCounter(0)
,   _previousDeltaAbsolute(0.f)
,   _previousDeltaPositive(true)
,   _previousDeltaTime(0)
{
}



bool    DetectInertiaScroll::inInertiaScroll(float delta)
{
    // See algorithm comments in the header.

    static constexpr const auto epsilon = std::numeric_limits<float>::epsilon();

    // If delta is ~zero, always reply that we're in inertia scroll...
    // This will avoid further useless processing and will result in the scroll event being ignored by the caller

    float   deltaAbsolute = std::abs(delta);

    if(deltaAbsolute < epsilon)
        return true;

    const bool  deltaPositive = delta > 0.f;
    const auto  now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

    if(_inInertiaState)
    {
        // Detect whether we should get out of inertia scroll state
        if(
            // Detect direction change
            deltaPositive != _previousDeltaPositive ||
            // Detect non-decreasing delta
            deltaAbsolute >= _previousDeltaAbsolute ||
            // Detect outdated inertia state
            now - _previousDeltaTime > std::chrono::milliseconds(100)
        )
        {
            // Getting out of inertia state, reset a couple of internals
            _inInertiaState = false;
            goto reinitialize_counter;
        }

        _previousDeltaTime = now;

        return true;
    }

    // Note that this (_subsequentLowerDeltaCounter == 0) will happen only for the first event ever being processed.
    if(_subsequentLowerDeltaCounter == 0)
        goto reinitialize_counter;

    // Check whether we're still going towards the inertia state, or whether we should reset the counter...

    if(
        deltaPositive == _previousDeltaPositive &&
        deltaAbsolute < _previousDeltaAbsolute &&
        now - _previousDeltaTime <= std::chrono::milliseconds(80)
    )
    {
        _previousDeltaAbsolute = deltaAbsolute;
        // _previousDeltaPositive may not have changed
        _previousDeltaTime = now;

        if(++_subsequentLowerDeltaCounter == 4)
        {
            _inInertiaState = true;
            return true;
        }

        return false;
    }


  reinitialize_counter: // https://xkcd.com/292/

    _subsequentLowerDeltaCounter = 1;
    _previousDeltaAbsolute = deltaAbsolute;
    _previousDeltaPositive = deltaPositive;
    _previousDeltaTime = now;

    return false;
}


NS_CC_END

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN)
