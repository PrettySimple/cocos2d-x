/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2012 Stewart Hamilton-Arrandale.
 * http://creativewax.co.uk
 *
 * Modified by Yannick Loriot.
 * http://yannickloriot.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Converted to c++ / cocos2d-x by Angus C
 */

#ifndef CC_EXTENSIONS_GUI_CONTROLEXTENSION_HUEPICKER_H
#define CC_EXTENSIONS_GUI_CONTROLEXTENSION_HUEPICKER_H

#include <cocos/extensions/ExtensionExport.h>
#include <cocos/extensions/ExtensionMacros.h>
#include <cocos/extensions/GUI/CCControlExtension/CCControl.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCPlatformMacros.h>

namespace cocos2d
{
    class Event;
    class Node;
    class Sprite;
    class Touch;
} // namespace cocos2d

NS_CC_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

class CC_EX_DLL ControlHuePicker : public Control
{
public:
    static ControlHuePicker* create(Node* target, Vec2 pos);
    /**
     * @js ctor
     */
    ControlHuePicker();
    /**
     * @js NA
     * @lua NA
     */
    ~ControlHuePicker() override;
    virtual bool initWithTargetAndPos(Node* target, Vec2 pos);

    void setEnabled(bool enabled) override;

    // overrides
    bool onTouchBegan(Touch* touch, Event* pEvent) override;
    void onTouchMoved(Touch* pTouch, Event* pEvent) override;

    virtual Sprite* getBackground() const noexcept { return _background; }
    virtual void setBackground(Sprite* background) noexcept;
    virtual Sprite* getSlider() const noexcept { return _slider; }
    virtual void setSlider(Sprite* slider) noexcept;

protected:
    void updateSliderPosition(Vec2 location);
    bool checkSliderPosition(Vec2 location);

    // manually put in the setters
    CC_SYNTHESIZE_READONLY(float, _hue, Hue)
    virtual void setHue(float val);
    CC_SYNTHESIZE_READONLY(float, _huePercentage, HuePercentage)
    virtual void setHuePercentage(float val);

    // not sure if these need to be there actually. I suppose someone might want to access the sprite?
    Sprite* _background = nullptr;
    Sprite* _slider = nullptr;
    CC_SYNTHESIZE_READONLY(Vec2, _startPos, StartPos)
};

// end of GUI group
/// @}
/// @}

NS_CC_EXT_END

#endif // CC_EXTENSIONS_GUI_CONTROLEXTENSION_HUEPICKER_H
