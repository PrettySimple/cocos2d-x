/*
 * Copyright (c) 2012 cocos2d-x.org
 * http://www.cocos2d-x.org
 *
 * Copyright 2012 Yannick Loriot. All rights reserved.
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
 */

#ifndef CC_EXTENSIONS_GUI_CONTROLEXTENSION_STEPPER_H
#define CC_EXTENSIONS_GUI_CONTROLEXTENSION_STEPPER_H

#include "extensions/ExtensionExport.h"
#include "extensions/ExtensionMacros.h"
#include "extensions/GUI/CCControlExtension/CCControl.h"
#include "math/Vec2.h"
#include "platform/CCPlatformMacros.h"

namespace cocos2d
{
    class Event;
    class Label;
    class Sprite;
    class Touch;
}

NS_CC_EXT_BEGIN

/**
 * @addtogroup GUI
 * @{
 * @addtogroup control_extension
 * @{
 */

class CC_EX_DLL ControlStepper : public Control
{
public:
    enum class Part
    {
        MINUS,
        PLUS,
        NONE
    };

    static ControlStepper* create(Sprite* minusSprite, Sprite* plusSprite);
    /**
     * @js ctor
     * @lua new
     */
    ControlStepper();
    /**
     * @js NA
     * @lua NA
     */
    ~ControlStepper() override;

    bool initWithMinusSpriteAndPlusSprite(Sprite* minusSprite, Sprite* plusSprite);

    virtual void setWraps(bool wraps);
    virtual void setMinimumValue(double minimumValue);
    virtual void setMaximumValue(double maximumValue);
    virtual void setValue(double value);

    virtual double getValue() const;
    virtual void setStepValue(double stepValue);
    /** Set the numeric value of the stepper. If send is true, the Control::EventType::VALUE_CHANGED is sent. */
    virtual void setValueWithSendingEvent(double value, bool send);
    virtual bool isContinuous() const;

    // Overrides
    virtual bool onTouchBegan(Touch* pTouch, Event* pEvent) override;
    virtual void onTouchMoved(Touch* pTouch, Event* pEvent) override;
    virtual void onTouchEnded(Touch* pTouch, Event* pEvent) override;
    void update(float dt) override;

    /** Update the layout of the stepper with the given touch location. */
    void updateLayoutUsingTouchLocation(Vec2 location);

    /** Start the autorepeat increment/decrement. */
    void startAutorepeat();

    /** Stop the autorepeat. */
    void stopAutorepeat();

    virtual Sprite* getMinusSprite() const noexcept { return _minusSprite; }
    virtual void setMinusSprite(Sprite* minusSprite) noexcept;
    virtual Sprite* getPlusSprite() const noexcept { return _plusSprite; }
    virtual void setPlusSprite(Sprite* plusSprite) noexcept;
    virtual Label* getMinusLabel() const noexcept { return _minusLabel; }
    virtual void setMinusLabel(Label* minusLabel) noexcept;
    virtual Label* getPlusLabel() const noexcept { return _plusLabel; }
    virtual void setPlusLabel(Label* plusLabel) noexcept;

protected:
    /** The numeric value of the stepper. */
    double _value;
    /** The continuous vs. noncontinuous state of the stepper. */
    bool _continuous;
    /** The automatic vs. nonautomatic repeat state of the stepper. */
    bool _autorepeat;
    /** The wrap vs. no-wrap state of the stepper. */
    bool _wraps;
    /** The lowest possible numeric value for the stepper. */
    double _minimumValue;
    /** The highest possible numeric value for the stepper. */
    double _maximumValue;
    /** The step, or increment, value for the stepper. */
    double _stepValue;
    bool _touchInsideFlag;
    Part _touchedPart;
    int _autorepeatCount;

    // Weak links to children
    Sprite* _minusSprite;
    Sprite* _plusSprite;
    Label* _minusLabel;
    Label* _plusLabel;
};

// end of GUI group
/// @}
/// @}

NS_CC_EXT_END

#endif // CC_EXTENSIONS_GUI_CONTROLEXTENSION_STEPPER_H
