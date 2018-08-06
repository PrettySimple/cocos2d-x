/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef CC_BASE_EVENTTOUCH_H
#define CC_BASE_EVENTTOUCH_H

#include <cocos/base/CCEvent.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <vector>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

class Touch;

#define TOUCH_PERF_DEBUG 1

/** @class EventTouch
 * @brief Touch event.
 */
class CC_DLL EventTouch : public Event
{
public:
    static const int MAX_TOUCHES = 15;

    /** EventCode Touch event code.*/
    enum class EventCode : std::uint8_t
    {
        BEGAN,
        MOVED,
        ENDED,
        CANCELLED
    };

    /**
     * Constructor.
     * @js NA
     */
    EventTouch();
    ~EventTouch();

    /** Get event code.
     *
     * @return The code of the event.
     */
    inline EventCode getEventCode() const noexcept { return _eventCode; }

    /** Get the touches.
     *
     * @return The touches of the event.
     */
    inline std::vector<Touch*> const& getTouches() const noexcept { return _touches; }

#if TOUCH_PERF_DEBUG
    /** Set the event code.
     *
     * @param eventCode A given EventCode.
     */
    inline void setEventCode(EventCode eventCode) noexcept { _eventCode = eventCode; }
    /** Set the touches
     *
     * @param touches A given touches vector.
     */
    void setTouches(std::vector<Touch*> const& touches) noexcept { _touches = touches; }
#endif

private:
    EventCode _eventCode;
    std::vector<Touch*> _touches;

    friend class GLView;
};

NS_CC_END

// end of base group
/// @}

#endif // CC_BASE_EVENTTOUCH_H
