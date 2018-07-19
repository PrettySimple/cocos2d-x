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

#ifndef CC_BASE_EVENT_H
#define CC_BASE_EVENT_H

#include "base/CCRef.h"
#include "base/ccConfig.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCPlatformDefine.h"

#include <cstdint>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

class Node;

/** @class Event
 * @brief Base class of all kinds of events.
 */
class CC_DLL Event : public Ref
{
public:
    /** Type Event type.*/
    enum struct Type : std::uint8_t
    {
        NONE,
        TOUCH,
        KEYBOARD,
        ACCELERATION,
        MOUSE,
        FOCUS,
        GAME_CONTROLLER,
        CUSTOM
    };

    CC_CONSTRUCTOR_ACCESS :
        /** Constructor */
        Event(Type type);

public:
    Event() = default;
    Event(Event const&) = default;
    Event& operator=(Event const&) = default;
    Event(Event&&) noexcept;
    Event& operator=(Event&&) noexcept;
    virtual ~Event();

    /** Gets the event type.
     *
     * @return The event type.
     */
    Type getType() const { return _type; }

    /** Stops propagation for current event.
     */
    void stopPropagation() { _isStopped = true; }

    /** Checks whether the event has been stopped.
     *
     * @return True if the event has been stopped.
     */
    bool isStopped() const { return _isStopped; }

    /** Gets current target of the event.
     * @return The target with which the event associates.
     * @note It's only available when the event listener is associated with node.
     *        It returns 0 when the listener is associated with fixed priority.
     */
    Node* getCurrentTarget() { return _currentTarget; }

protected:
    /** Sets current target */
    inline void setCurrentTarget(Node* target) noexcept { _currentTarget = target; }

    Type _type = Type::NONE; ///< Event type

    bool _isStopped = false; ///< whether the event has been stopped.
    Node* _currentTarget = nullptr; ///< Current target

    friend class EventDispatcher;
};

NS_CC_END

// end of base group
/// @}

#endif // CC_BASE_EVENT_H
