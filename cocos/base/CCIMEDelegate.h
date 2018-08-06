/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (C) 2013-2016 Chukong Technologies Inc.

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

#ifndef CC_BASE_IMEDELEGATE_H
#define CC_BASE_IMEDELEGATE_H

#include <cocos/base/CCEventKeyboard.h>
#include <cocos/math/CCGeometry.h>

#include <cstdint>
#include <string>

/**
 * @addtogroup base
 * @{
 */
NS_CC_BEGIN

enum class KeyCode : std::uint16_t;

/**
 * Keyboard notification event type.
 */
typedef struct
{
    Rect begin; // the soft keyboard rectangle when animation begins
    Rect end; // the soft keyboard rectangle when animation ends
    float duration; // the soft keyboard animation duration
} IMEKeyboardNotificationInfo;

/**
 *@brief    Input method editor delegate.
 */
class CC_DLL IMEDelegate
{
public:
    /**
     * Default constructor.
     * @js NA
     * @lua NA
     */
    virtual ~IMEDelegate();

    /**
     * Default destructor.
     * @js NA
     * @lua NA
     */
    virtual bool attachWithIME();

    /**
     * Determine whether the IME is detached or not.
     * @js NA
     * @lua NA
     */
    virtual bool detachWithIME();

protected:
    friend class IMEDispatcher;

    /**
    @brief    Decide if the delegate instance is ready to receive an IME message.

    Called by IMEDispatcher.
    * @js NA
    * @lua NA
    */
    virtual bool canAttachWithIME() { return false; }
    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    * @js NA
    * @lua NA
    */
    virtual void didAttachWithIME() {}

    /**
    @brief    Decide if the delegate instance can stop receiving IME messages.
    * @js NA
    * @lua NA
    */
    virtual bool canDetachWithIME() { return false; }

    /**
    @brief    When the delegate detaches from the IME, this method is called by IMEDispatcher.
    * @js NA
    * @lua NA
    */
    virtual void didDetachWithIME() {}

    /**
    @brief    Called by IMEDispatcher when text input received from the IME.
    * @js NA
    * @lua NA
    */
    virtual void insertText(const char*, size_t) {}

    /**
    @brief    Called by IMEDispatcher after the user clicks the backward key.
    * @js NA
    * @lua NA
    */
    virtual void deleteBackward() {}

    /**
    @brief    Called by IMEDispatcher after the user press control key.
    * @js NA
    * @lua NA
    */
    virtual void controlKey(EventKeyboard::KeyCode keyCode) {}

    /**
    @brief    Called by IMEDispatcher for text stored in delegate.
    * @js NA
    * @lua NA
    */
    inline virtual std::string const& getContentText() const
    {
        static std::string const empty = "";
        return empty;
    }

    //////////////////////////////////////////////////////////////////////////
    // keyboard show/hide notification
    //////////////////////////////////////////////////////////////////////////
    /**
     * @js NA
     * @lua NA
     */
    virtual void keyboardWillShow(IMEKeyboardNotificationInfo&) {}
    /**
     * @js NA
     * @lua NA
     */
    virtual void keyboardDidShow(IMEKeyboardNotificationInfo&) {}
    /**
     * @js NA
     * @lua NA
     */
    virtual void keyboardWillHide(IMEKeyboardNotificationInfo&) {}
    /**
     * @js NA
     * @lua NA
     */
    virtual void keyboardDidHide(IMEKeyboardNotificationInfo&) {}

protected:
    /**
     * @js NA
     * @lua NA
     */
    IMEDelegate();
};

NS_CC_END
// end of base group
/// @}

#endif // CC_BASE_IMEDELEGATE_H
