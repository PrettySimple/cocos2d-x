/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#ifndef CC_2D_TEXTFIELDTTF_H
#define CC_2D_TEXTFIELDTTF_H

#include "2d/CCLabel.h"
#include "base/CCEventKeyboard.h"
#include "base/CCIMEDelegate.h"
#include "base/ccTypes.h"
#include "math/CCGeometry.h"
#include "math/Mat4.h"
#include "math/Vec2.h"
#include "platform/CCPlatformMacros.h"
#include "platform/CCPlatformDefine.h"

#include <cstdint>
#include <cstddef>
#include <iosfwd>

/**
 * @addtogroup ui
 * @{
 */
NS_CC_BEGIN

class Camera;
class Renderer;
class TextFieldTTF;

/**
 * A input protocol for TextField.
 */
class CC_DLL TextFieldDelegate
{
public:
    /**
     * Destructor for TextFieldDelegate.
     * @js NA
     */
    virtual ~TextFieldDelegate();

    /**
     *@brief    If the sender doesn't want to attach to the IME, return true.
     */
    virtual bool onTextFieldAttachWithIME(TextFieldTTF*);

    /**
     *@brief    If the sender doesn't want to detach from the IME, return true.
     */
    virtual bool onTextFieldDetachWithIME(TextFieldTTF*);

    /**
     *@brief    If the sender doesn't want to insert the text, return true.
     */
    virtual bool onTextFieldInsertText(TextFieldTTF*, const char*, size_t);

    /**
     *@brief    If the sender doesn't want to delete the delText, return true.
     */
    virtual bool onTextFieldDeleteBackward(TextFieldTTF*, const char*, size_t);

    /**
     *@brief    If the sender doesn't want to draw, return true.
     * @js NA
     */
    virtual bool onVisit(TextFieldTTF*, Renderer*, const Mat4&, uint32_t);
};

/**
 *@brief    A simple text input field with TTF font.
 */
class CC_DLL TextFieldTTF : public Label, public IMEDelegate
{
public:
    /**
     * Default constructor.
     * @js ctor
     */
    TextFieldTTF();

    /**
     * Default destructor.
     * @js NA
     * @lua NA
     */
    ~TextFieldTTF() override;

    /** Creates a TextFieldTTF from a fontname, alignment, dimension and font size.
     * @js NA
     */
    static TextFieldTTF*
    textFieldWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);

    /** Creates a TextFieldTTF from a fontname and font size.
     * @js NA
     */
    static TextFieldTTF* textFieldWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

    /** Initializes the TextFieldTTF with a font name, alignment, dimension and font size. */
    bool initWithPlaceHolder(const std::string& placeholder, const Size& dimensions, TextHAlignment alignment, const std::string& fontName, float fontSize);

    /** Initializes the TextFieldTTF with a font name and font size. */
    bool initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize);

    /**
     *@brief    Open keyboard and receive input text.
     */
    bool attachWithIME() override;

    /**
     *@brief    End text input and close keyboard.
     */
    bool detachWithIME() override;

    //////////////////////////////////////////////////////////////////////////
    // properties
    //////////////////////////////////////////////////////////////////////////
    /**
     * @lua NA
     */
    inline TextFieldDelegate* getDelegate() const noexcept { return _delegate; }
    /**
     * @lua NA
     */
    inline void setDelegate(TextFieldDelegate* delegate) noexcept { _delegate = delegate; }

    /**
     * Query the currently inputed character count.
     *@return The total input character count.
     */
    inline std::size_t getCharCount() const noexcept { return _charCount; }

    /**
     * Query the color of place holder.
     *@return The place holder color.
     */
    virtual const Color4B& getColorSpaceHolder();

    /**
     *@brief Change input placeholder color.
     *@param color A color value in `Color3B`.
     */
    virtual void setColorSpaceHolder(const Color3B& color);

    /**
     * Change the placeholder color.
     *@param color The placeholder color in Color4B.
     */
    virtual void setColorSpaceHolder(const Color4B& color);

    /**
     * Change the color of input text.
     *@param textColor The text color in Color4B.
     */
    void setTextColor(const Color4B& textColor) override;

    /**
     * Change input text of TextField.
     *@param text The input text of TextField.
     */
    void setString(const std::string& text) override;

    /**
     * Append to input text of TextField.
     *@param text The append text of TextField.
     */
    virtual void appendString(const std::string& text);

    /**
     * Query the input text of TextField.
     *@return Get the input text of TextField.
     */
    const std::string& getString() const override;

    /**
     * Change placeholder text.
     * place holder text displayed when there is no text in the text field.
     *@param text  The placeholder string.
     */
    virtual void setPlaceHolder(const std::string& text);

    /**
     * Query the placeholder string.
     *@return The placeholder string.
     */
    virtual const std::string& getPlaceHolder() const;

    /**
     * Set enable secure text entry representation.
     * If you want to display password in TextField, this option is very helpful.
     *@param value Whether or not to display text with secure text entry.
     * @js NA
     */
    virtual void setSecureTextEntry(bool value);
    virtual void setPasswordTextStyle(const std::string& text);
    const std::string& getPasswordTextStyle() const;

    /**
     * Query whether the currently display mode is secure text entry or not.
     *@return Whether current text is displayed as secure text entry.
     * @js NA
     */
    virtual bool isSecureTextEntry() const;

    void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    void update(float delta) override;

    /**
     * Set enable cursor use.
     * @js NA
     */
    void setCursorEnabled(bool enabled);

    /**
     * Set char showing cursor.
     * @js NA
     */
    void setCursorChar(char cursor);

    /**
     * Set cursor position, if enabled
     * @js NA
     */
    void setCursorPosition(std::size_t cursorPosition);

    /**
     * Set cursor position to hit letter, if enabled
     * @js NA
     */
    void setCursorFromPoint(const Vec2& point, const Camera* camera);

protected:
    //////////////////////////////////////////////////////////////////////////
    // IMEDelegate interface
    //////////////////////////////////////////////////////////////////////////

    bool canAttachWithIME() override;
    bool canDetachWithIME() override;
    void didAttachWithIME() override;
    void didDetachWithIME() override;
    void insertText(const char* text, size_t len) override;
    void deleteBackward() override;
    std::string const& getContentText() const override;
    void controlKey(EventKeyboard::KeyCode keyCode) override;

    TextFieldDelegate* _delegate;
    std::size_t _charCount;

    std::string _inputText;

    std::string _placeHolder;
    Color4B _colorSpaceHolder;
    Color4B _colorText;

    bool _secureTextEntry;
    std::string _passwordStyleText;

    // Need use cursor
    bool _cursorEnabled;
    // Current position cursor
    std::size_t _cursorPosition;
    // Char showing cursor
    char _cursorChar;
    // >0 - show, <0 - hide
    float _cursorShowingTime;

    bool _isAttachWithIME;

    void makeStringSupportCursor(std::string& displayText);
    void updateCursorDisplayText();
    void setAttachWithIME(bool isAttachWithIME);

private:
    class LengthStack;
    LengthStack* _lens;
};

NS_CC_END
// end of ui group
/// @}

#endif // CC_2D_TEXTFIELDTTF_H
