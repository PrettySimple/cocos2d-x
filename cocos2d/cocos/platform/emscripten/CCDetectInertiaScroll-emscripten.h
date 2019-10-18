//
//  CCDetectInertiaScroll-emscripten.h
//
#pragma once

/*
    Approach to mitigate the "inertia scroll" (also called "momentum scroll") where the OS/browser keeps sending scroll events
    after the human actually stopped scrolling.

    That behavior is very annoying, as these events may continue up to several seconds (2 sec. measured on OSX with trackpad two-finger
    scrolling), resulting in a weird user experiance (eg. elastic scrollview not returning to it's initial position until events stop, that is,
    up to several seconds after the user stopped scrolling).

    I have tried an approach described/used by the following:

        * https://github.com/d4nyll/lethargy
        * https://stackoverflow.com/questions/4339196/how-to-detect-disable-inertial-scrolling-in-mac-safari
        * https://underscorejs.org/#debounce

    yet none gave trully satisfying result. All had a more or less annoying glitches, usually jerky scroll under normal conditions.

    The pure CSS solutions didn't work either ("-webkit-overflow-scrolling: touch", etc).

    Here we're implementing a slightly different approach with very little probability of false positive (though there is one, since this is all empiric).
    We'll consider we're in (and remain in) "inertia scroll" state if the last 3/4 consecutive scroll events were:

        * In the same direction (measured separately for X and Y axis)
        * Each had strictly lower delta (in absolute value) that the preceeding event (measured separately for X and Y axis)
        * Less than a given amount of time has elapsed since the last processed event.

    Once we're in "inertia scroll" state, we remain there, unless:

        * The scroll direction has changed.
        * A given amount of time has elapsed since the last processed event.
        * The delta is not lower (ie. is higher or equal) to the one of the last processed event

*/

#include <chrono>

NS_CC_BEGIN

class DetectInertiaScroll
{
private:
    bool _inInertiaState;
    uint8_t _subsequentLowerDeltaCounter;
    float _previousDeltaAbsolute;
    bool _previousDeltaPositive;
    std::chrono::milliseconds _previousDeltaTime;

public:
    DetectInertiaScroll();

    DetectInertiaScroll(const DetectInertiaScroll&) = delete;
    DetectInertiaScroll& operator=(const DetectInertiaScroll&) = delete;
    DetectInertiaScroll(DetectInertiaScroll&&) noexcept = delete;
    DetectInertiaScroll& operator=(DetectInertiaScroll&&) noexcept = delete;
    ~DetectInertiaScroll() = default;

    // Event should be discarded altogether if inInertiaScroll() returns true.
    bool inInertiaScroll(float delta);
};

NS_CC_END
