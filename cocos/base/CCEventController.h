/****************************************************************************
 Copyright (c) 2014 cocos2d-x.org
 Copyright (c) 2014 Chukong Technologies Inc.

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

#ifndef CC_BASE_EVENTCONTROLLER_H
#define CC_BASE_EVENTCONTROLLER_H

#include <cocos/base/CCEvent.h>
#include <cocos/platform/CCPlatformMacros.h>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

/// @cond EventController
class Controller;

/** @class EventController
 * @brief Controller event.
 */
class EventController : public Event
{
    friend class EventListenerController;

public:
    /** ControllerEventType Controller event type.*/
    enum struct ControllerEventType : std::uint8_t
    {
        CONNECTION,
        BUTTON_STATUS_CHANGED,
        AXIS_STATUS_CHANGED,
    };

    /** Create a EventController with controller event type, controller and key code.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param keyCode A given key code.
     * @return An autoreleased EventController object.
     */
    EventController(ControllerEventType type, Controller* controller, int keyCode);
    /** Create a EventController with controller event type, controller and whether or not is connected.
     *
     * @param type A given controller event type.
     * @param controller A given controller pointer.
     * @param isConnected True if it is connected.
     * @return An autoreleased EventController object.
     */
    EventController(ControllerEventType type, Controller* controller, bool isConnected);
    ~EventController() override;

    /** Gets the event type of the controller.
     *
     * @return The event type of the controller.
     */
    inline ControllerEventType getControllerEventType() const noexcept { return _controllerEventType; }
    Controller* getController() const noexcept { return _controller; }

    /** Gets the key code of the controller.
     *
     * @return The key code of the controller.
     */
    inline int getKeyCode() const noexcept { return _keyCode; }
    void setKeyCode(int keyCode) noexcept { _keyCode = keyCode; }

    /** Sets the connect status.
     *
     * @param True if it's connected.
     */
    void setConnectStatus(bool isConnected) noexcept { _isConnected = isConnected; }
    /** Gets the connect status.
     *
     * @return True if it's connected.
     */
    inline bool isConnected() const noexcept { return _isConnected; }

protected:
    ControllerEventType _controllerEventType;
    Controller* _controller;
    int _keyCode;
    bool _isConnected;
};
/// @endcond EventController
NS_CC_END

// end of base group
/// @}

#endif // CC_BASE_EVENTCONTROLLER_H
