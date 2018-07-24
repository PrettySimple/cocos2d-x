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

#ifndef CC_UI_TEXTFIELD_H
#define CC_UI_TEXTFIELD_H

#include <cocos/2d/CCTextFieldTTF.h>
#include <cocos/base/CCRef.h>
#include <cocos/base/ccConfig.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/ui/GUIDefine.h>
#include <cocos/ui/GUIExport.h>
#include <cocos/ui/UIWidget.h>

#include <cstddef>
#include <functional>
#include <iosfwd>

NS_CC_BEGIN

class Camera;
class Event;
class Node;
class Size;
class Touch;
class Vec2;
class Vec3;
enum struct TextHAlignment : std::uint8_t;
enum struct TextVAlignment : std::uint8_t;
struct Color3B;
struct Color4B;

/**
 * @addtogroup ui
 * @{
 */

namespace ui
{
    /**
     * @brief A helper class which inherit from @see `TextFieldTTF` and implements the @see `TextFieldDelegate` protocol.
     * It is mainly be used internally by @see `UITextField` class.
     * @js NA
     * @lua NA
     */
    class CC_GUI_DLL UICCTextField : public TextFieldTTF, public TextFieldDelegate
    {
    public:
        /**
         * Default constructor
         */
        UICCTextField();

        /**
         * Default destructor
         */
        ~UICCTextField() override;

        void onEnter() override;

        /**
         * Create a UICCTextField instance with a placeholder, a fontName and a fontSize.
         *@param placeholder Placeholder in string.
         *@param fontName Font name in string.
         *@param fontSize Font size in float.
         *@return A UICCTextField instance.
         */
        static UICCTextField* create(const std::string& placeholder, const std::string& fontName, float fontSize);

        // override functions
        bool onTextFieldAttachWithIME(TextFieldTTF* pSender) override;
        bool onTextFieldDetachWithIME(TextFieldTTF* pSender) override;
        bool onTextFieldInsertText(TextFieldTTF* pSender, const char* text, std::size_t nLen) override;
        bool onTextFieldDeleteBackward(TextFieldTTF* pSender, const char* delText, size_t nLen) override;
        void insertText(const char* text, std::size_t len) override;
        void deleteBackward() override;

        /**
         * Open up the IME.
         */
        void openIME();

        /**
         * Close the IME.
         */
        void closeIME();

        /**
         * Toggle enable max length limitation.
         *@param enable True to enable max length, false otherwise.
         */
        void setMaxLengthEnabled(bool enable);

        /**
         * Query max length enable state.
         *@return Whether max length is enabled or not.
         */
        bool isMaxLengthEnabled() const;

        /**
         * Set maximize length.
         *@param length  The maximize length in integer.
         */
        void setMaxLength(int length);

        /**
         * Get maximize length.
         *@return Maximize length.
         */
        int getMaxLength() const;

        /**
         * Return the total inputed characters.
         *@return Total inputed character count.
         */
        std::size_t getCharCount() const;

        /**
         * @brief Toggle password input mode.
         *
         * @param enable True if enable password input, false otherwise.
         */
        void setPasswordEnabled(bool enable);

        /**
         * @brief Query whether password input mode is enabled or not.
         *
         * @return True if password input is enabled, false otherwise.
         */
        bool isPasswordEnabled() const;

        /**
         * @brief Change password style text.
         *
         * @param styleText The styleText for password mask, the default value is "*".
         */
        void setPasswordStyleText(const std::string& styleText);

        /**
         * @brief Set the password text content.
         *
         * @param text The content of password.
         */
        void setPasswordText(const std::string& text);

        /**
         * @brief Toggle attach with IME.
         *
         * @param attach True if attach with IME, false otherwise.
         */
        void setAttachWithIME(bool attach);

        /**
         * @brief Query whether the IME is attached or not.
         *
         * @return True if IME is attached, false otherwise.
         */
        bool getAttachWithIME() const;

        /**
         * @brief Toggle detach with IME.
         *
         * @param detach True if detach with IME, false otherwise.
         */
        void setDetachWithIME(bool detach);

        /**
         * @brief Query whether IME is detached or not.
         *
         * @return True if IME is detached, false otherwise.
         */
        bool getDetachWithIME() const;

        /**
         * @brief Toggle enable text insert.
         *
         * @param insert True if enable insert text, false otherwise.
         */
        void setInsertText(bool insert);

        /**
         * @brief Query whether insert text is enabled or not.
         *
         * @return True if insert text is enabled, false otherwise.
         */
        bool getInsertText() const;

        /**
         * @brief Toggle enable delete backward.
         *
         * @param deleteBackward True if enable delete backward, false otherwise.
         */
        void setDeleteBackward(bool deleteBackward);

        /**
         * @brief Query whether delete backward is enabled   or not.
         *
         * @return True if delete backward is enabled, false otherwise.
         */
        bool getDeleteBackward() const;

    protected:
        bool _maxLengthEnabled;
        int _maxLength;
        bool _attachWithIME;
        bool _detachWithIME;
        bool _insertText;
        bool _deleteBackward;
    };

    /**
     * TextField event type.
     * @deprecated Use @see `TextField::EventType` instead.
     */
    typedef enum
    {
        TEXTFIELD_EVENT_ATTACH_WITH_IME,
        TEXTFIELD_EVENT_DETACH_WITH_IME,
        TEXTFIELD_EVENT_INSERT_TEXT,
        TEXTFIELD_EVENT_DELETE_BACKWARD,
    } TextFiledEventType;

    /**
     * A callback which would be called when a TextField event happens.
     * @deprecated Use @see `ccTextFieldCallback` instead.
     */
    typedef void (Ref::*SEL_TextFieldEvent)(Ref*, TextFiledEventType);
#define textfieldeventselector(_SELECTOR) (SEL_TextFieldEvent)(&_SELECTOR)

    /**
     * @brief A widget which allows users to input text.
     * The rendering of the input text are based on @see `TextFieldTTF'.
     * If you want to use system control behavior, please use @see `EditBox` instead.
     * @js NA
     * @lua NA
     */
    class CC_GUI_DLL TextField : public Widget
    {
        DECLARE_CLASS_GUI_INFO

    public:
        /**
         * TextField event type.
         */
        enum class EventType : std::uint8_t
        {
            ATTACH_WITH_IME,
            DETACH_WITH_IME,
            INSERT_TEXT,
            DELETE_BACKWARD,
        };
        /**
         * A callback which would be called when a TextField event happens.
         */
        using ccTextFieldCallback = std::function<void(Ref*, EventType)>;

        /**
         * @brief Default constructor.
         *
         */
        TextField();

        /**
         * @brief Default destructor.
         */
        ~TextField() override;

        /**
         * @brief Create an empty TextField.
         *
         * @return A TextField instance.
         */
        static TextField* create();

        /**
         * @brief Create a TextField with a placeholder, a font name and a font size.
         *
         * @param placeholder The placeholder string.
         * @param fontName The font name.
         * @param fontSize The font size.
         * @return A TextField instance.
         */
        static TextField* create(const std::string& placeholder, const std::string& fontName, int fontSize);

        /**
         * @brief Set the touch size
         * The touch size is used for @see `hitTest`.
         * @param size A delimitation zone.
         */
        void setTouchSize(const Size& size);

        /**
         * @brief Get current touch size of TextField.
         *
         * @return The TextField's touch size.
         */
        Size getTouchSize() const;

        /**
         * @brief Toggle enable touch area.
         *
         * @param enable True if enable touch area, false otherwise.
         */
        void setTouchAreaEnabled(bool enable);

        bool hitTest(const Vec2& pt, const Camera* camera, Vec3* p) const override;

        /**
         * @brief Set placeholder of TextField.
         *
         * @param value The string value of placeholder.
         */
        void setPlaceHolder(const std::string& value);

        /**
         * @brief Get the placeholder of TextField.
         *
         * @return A placeholder string.
         */
        const std::string& getPlaceHolder() const;

        /**
         * @brief Query the placeholder string color.
         *
         * @return The color of placeholder.
         */
        const Color4B& getPlaceHolderColor() const;

        /**
         * @brief Change the placeholder color.
         *
         * @param color A color value in `Color3B`.
         */
        void setPlaceHolderColor(const Color3B& color);

        /**
         * @brief Change the placeholder color.
         *
         * @param color A color value in `Color4B`.
         */
        void setPlaceHolderColor(const Color4B& color);

        /**
         * @brief Query the text string color.
         *
         * @return The color of the text.
         */
        const Color4B& getTextColor() const;

        /**
         * @brief Change the text color.
         *
         * @param textColor The color value in `Color4B`.
         */
        void setTextColor(const Color4B& textColor);

        /**
         * @brief Change font size of TextField.
         *
         * @param size The integer font size.
         */
        void setFontSize(int size);

        /**
         * @brief Query the font size.
         *
         * @return The integer font size.
         */
        int getFontSize() const;

        /**
         * @brief Change the font name of TextField.
         *
         * @param name The font name string.
         */
        void setFontName(const std::string& name);

        /**
         * @brief Query the TextField's font name.
         *
         * @return The font name string.
         */
        const std::string& getFontName() const;

        /**
         * @brief Detach the IME.
         */
        virtual void didNotSelectSelf();

        /**
         *Change content of TextField.
         *@deprecated Use @see `setString(const std::string&)` instead.
         *@param text A string content.
         */
        CC_DEPRECATED_ATTRIBUTE void setText(const std::string& text) { this->setString(text); }

        /**
         *Query the content of TextField.
         *@deprecated Use @see `getString` instead.
         *@return The string value of TextField.
         */
        CC_DEPRECATED_ATTRIBUTE const std::string& getStringValue() const { return this->getString(); }

        /**
         *Change content of TextField.
         *@param text A string content.
         */
        void setString(const std::string& text);

        /**
         *Query the content of TextField.
         *@return The string value of TextField.
         */
        const std::string& getString() const;

        bool onTouchBegan(Touch* touch, Event* unusedEvent) override;

        /**
         * @brief Toggle maximize length enable
         *
         * @param enable True if enable maximize length, false otherwise.
         */
        void setMaxLengthEnabled(bool enable);

        /**
         * @brief Query whether max length is enabled or not.
         *
         * @return True if maximize length is enabled, false otherwise.
         */
        bool isMaxLengthEnabled() const;

        /**
         * @brief Change maximize input length limitation.
         *
         * @param length A character count in integer.
         */
        void setMaxLength(int length);

        /**
         * @brief Query maximize input length of TextField.
         *
         * @return The integer value of maximize input length.
         */
        int getMaxLength() const;

        /**
         * @brief Query the input string length.
         *
         * @return A integer length value.
         */
        int getStringLength() const;

        /**
         * @brief Toggle enable password input mode.
         *
         * @param enable True if enable password input mode, false otherwise.
         */
        void setPasswordEnabled(bool enable);

        /**
         * @brief Query whether password is enabled or not.
         *
         * @return True if password is enabled, false otherwise.
         */
        bool isPasswordEnabled() const;

        /**
         * @brief Change password style text.
         *
         * @param styleText The styleText for password mask, the default value is "*".
         */
        void setPasswordStyleText(const char* styleText);

        /**
         * @brief Query the password style text.
         *
         * @return A password style text.
         */
        const char* getPasswordStyleText() const;

        void update(float dt) override;

        /**
         * @brief Query whether the IME is attached or not.
         *
         * @return True if IME is attached, false otherwise.
         */
        bool getAttachWithIME() const;

        /**
         * @brief Toggle attach with IME.
         *
         * @param attach True if attach with IME, false otherwise.
         */
        void setAttachWithIME(bool attach);

        /**
         * @brief Query whether IME is detached or not.
         *
         * @return True if IME is detached, false otherwise.
         */
        bool getDetachWithIME() const;

        /**
         * @brief Toggle detach with IME.
         *
         * @param detach True if detach with IME, false otherwise.
         */
        void setDetachWithIME(bool detach);

        /**
         * @brief Whether it is ready to get the inserted text or not.
         *
         * @return True if the insert text is ready, false otherwise.
         */
        bool getInsertText() const;

        /**
         * @brief Toggle enable insert text mode
         *
         * @param insertText True if enable insert text, false otherwise.
         */
        void setInsertText(bool insertText);

        /**
         * @brief Whether it is ready to delete backward in TextField.
         *
         * @return True is the delete backward is enabled, false otherwise.
         */
        bool getDeleteBackward() const;

        /**
         * @brief Toggle enable delete backward mode.
         *
         * @param deleteBackward True is delete backward is enabled, false otherwise.
         */
        void setDeleteBackward(bool deleteBackward);

        /**
         * Add a event listener to TextField, when some predefined event happens, the callback will be called.
         *@deprecated Use @see `addEventListener` instead.
         *@param target A pointer of `Ref*` type.
         *@param selector A member function pointer with type of `SEL_TextFieldEvent`.
         */
        CC_DEPRECATED_ATTRIBUTE void addEventListenerTextField(Ref* target, SEL_TextFieldEvent selector);
        /**
         * Add a event listener to TextField, when some predefined event happens, the callback will be called.
         *@param callback A callback function with type of `ccTextFieldCallback`.
         */
        void addEventListener(const ccTextFieldCallback& callback);

        /**
         * Returns the "class name" of widget.
         */
        std::string getDescription() const override;

        /**
         * @brief Get the renderer size in auto mode.
         *
         * @return A delimitation zone.
         */
        virtual Size getAutoRenderSize();
        // override functions.
        Size getVirtualRendererSize() const override;
        Node* getVirtualRenderer() override;
        void onEnter() override;

        /**
         * @brief Attach the IME for inputing.
         *
         */
        void attachWithIME();

        /**
         * @brief Change the text area size.
         *
         * @param size A delimitation zone.
         */
        void setTextAreaSize(const Size& size);

        /**
         * @brief Change horizontal text alignment.
         *
         * @param alignment A alignment arguments in @see `TextHAlignment`.
         */
        void setTextHorizontalAlignment(TextHAlignment alignment);

        /**
         * @brief Inquire the horizontal alignment
         *
         * @return The horizontal alignment
         */
        TextHAlignment getTextHorizontalAlignment() const;

        /**
         * @brief Change the vertical text alignment.
         *
         * @param alignment A alignment arguments in @see `TextVAlignment`.
         */
        void setTextVerticalAlignment(TextVAlignment alignment);

        /**
         * @brief Inquire the horizontal alignment
         *
         * @return The horizontal alignment
         */
        TextVAlignment getTextVerticalAlignment() const;

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

        CC_CONSTRUCTOR_ACCESS : bool init() override;

    protected:
        void initRenderer() override;
        void attachWithIMEEvent();
        void detachWithIMEEvent();
        void insertTextEvent();
        void deleteBackwardEvent();
        void onSizeChanged() override;

        void textfieldRendererScaleChangedWithSize();

        Widget* createCloneInstance() override;
        void copySpecialProperties(Widget* model) override;
        void adaptRenderers() override;

    protected:
        UICCTextField* _textFieldRenderer;

        float _touchWidth;
        float _touchHeight;
        bool _useTouchArea;

        Ref* _textFieldEventListener;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 // vs 2005 or higher
#    pragma warning(push)
#    pragma warning(disable : 4996)
#endif
        SEL_TextFieldEvent _textFieldEventSelector;
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#    pragma GCC diagnostic warning "-Wdeprecated-declarations"
#elif _MSC_VER >= 1400 // vs 2005 or higher
#    pragma warning(pop)
#endif
        ccTextFieldCallback _eventCallback;

        bool _textFieldRendererAdaptDirty;

    private:
        enum struct FontType : std::uint8_t
        {
            SYSTEM,
            TTF
        };

        std::string _fontName;
        int _fontSize;
        FontType _fontType;
    };

} // namespace ui

// end of ui group
/// @}
NS_CC_END

#endif // CC_UI_TEXTFIELD_H
