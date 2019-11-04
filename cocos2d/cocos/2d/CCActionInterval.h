/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2011      Zynga Inc.
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

#ifndef CC_2D_ACTIONINTERVAL_H
#define CC_2D_ACTIONINTERVAL_H

#include <cocos/2d/CCAction.h>
#include <cocos/2d/CCAnimation.h>
#include <cocos/base/CCVector.h>
#include <cocos/base/ccConfig.h>
#include <cocos/base/ccMacros.h>
#include <cocos/base/ccTypes.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/math/Vec2.h>
#include <cocos/math/Vec3.h>
#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <chrono>
#include <functional>
#include <initializer_list>
#include <vector>

NS_CC_BEGIN

class Node;
class SpriteFrame;
class EventCustom;

/**
 * @addtogroup actions
 * @{
 */

/** @class ActionInterval
@brief An interval action is an action that takes place within a certain period of time.
It has an start time, and a finish time. The finish time is the parameter
duration plus the start time.

These ActionInterval actions have some interesting properties, like:
- They can run normally (default)
- They can run reversed with the reverse method
- They can run with the time altered with the Accelerate, AccelDeccel and Speed actions.

For example, you can simulate a Ping Pong effect running the action normally and
then running it again in Reverse mode.

Example:

@code
auto action = MoveBy::create(1.0f, Vec2::ONE);
auto pingPongAction = Sequence::create(action, action->reverse(), nullptr);
@endcode
*/
class CC_DLL ActionInterval : public FiniteTimeAction
{
protected:
    std::chrono::milliseconds _elapsed = std::chrono::milliseconds::zero();

public:
    ActionInterval() = default;
    ActionInterval(ActionInterval const&) = delete;
    ActionInterval& operator=(ActionInterval const&) = delete;
    ActionInterval(ActionInterval&&) noexcept = delete;
    ActionInterval& operator=(ActionInterval&&) noexcept = delete;
    ~ActionInterval() override;

    bool initWithDuration(std::chrono::milliseconds d);

    /** How many seconds had elapsed since the actions started to run.
     *
     * @return The seconds had elapsed since the actions started to run.
     */
    inline std::chrono::milliseconds getElapsed() const noexcept { return _elapsed; }

    /** Sets the amplitude rate, extension in GridAction
     *
     * @param amp   The amplitude rate.
     */
    virtual inline void setAmplitudeRate(float) noexcept { CCASSERT(false, "must be implemented"); }

    /** Gets the amplitude rate, extension in GridAction
     *
     * @return  The amplitude rate.
     */
    virtual inline float getAmplitudeRate() const noexcept
    {
        CCASSERT(false, "must be implemented");
        return 0.f;
    }

    bool isDone() const override;

    /**
     * @param dt in seconds
     */
    void step(float dt) override;

    ActionInterval* reverse() const override;
    ActionInterval* clone() const override;
};

/** @class Sequence
 * @brief Runs actions sequentially, one after another.
 */
class CC_DLL Sequence : public ActionInterval
{
    std::vector<FiniteTimeAction*> _actions;

public:
    Sequence() = default;
    Sequence(std::initializer_list<FiniteTimeAction*> actions);
    Sequence(Vector<FiniteTimeAction*> const& actions);
    Sequence(Sequence const&) = delete;
    Sequence& operator=(Sequence const&) = delete;
    Sequence(Sequence&&) noexcept = delete;
    Sequence& operator=(Sequence&&) noexcept = delete;
    ~Sequence() override;

    static Sequence* create(std::initializer_list<FiniteTimeAction*> actions);
    static Sequence* create(Vector<FiniteTimeAction*> const& actions);

    Sequence* clone() const override;
    Sequence* reverse() const override;
    void startWithTarget(Node* target) override;
    void stop() override;
    bool isDone() const override;
    void update(float p) override;
};

/** @class Repeat
 * @brief Repeats an action a number of times.
 * To repeat an action forever use the RepeatForever action.
 */
class CC_DLL Repeat : public ActionInterval
{
public:
    /** Creates a Repeat action. Times is an unsigned integer between 1 and pow(2,30).
     *
     * @param action The action needs to repeat.
     * @param times The repeat times.
     * @return An autoreleased Repeat object.
     */
    static Repeat* create(FiniteTimeAction* action, unsigned int times);

    /** Sets the inner action.
     *
     * @param action The inner action.
     */
    inline void setInnerAction(FiniteTimeAction* action) noexcept
    {
        if (_innerAction != action)
        {
            CC_SAFE_RETAIN(action);
            CC_SAFE_RELEASE(_innerAction);
            _innerAction = action;
        }
    }

    /** Gets the inner action.
     *
     * @return The inner action.
     */
    inline FiniteTimeAction* getInnerAction() const noexcept { return _innerAction; }

    //
    // Overrides
    //
    Repeat* clone() const override;
    Repeat* reverse() const override;
    void startWithTarget(Node* target) override;
    void stop() override;
    /**
     * @param dt In seconds.
     */
    void update(float dt) override;
    bool isDone() const override;

    CC_CONSTRUCTOR_ACCESS : Repeat() {}
    ~Repeat() override;

    /** initializes a Repeat action. Times is an unsigned integer between 1 and pow(2,30) */
    bool initWithAction(FiniteTimeAction* pAction, unsigned int times);

protected:
    unsigned int _times;
    unsigned int _total;
    float _nextDt;
    bool _actionInstant;
    /** Inner action */
    FiniteTimeAction* _innerAction;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Repeat)
};

/** @class RepeatForever
 * @brief Repeats an action for ever.
 To repeat the an action for a limited number of times use the Repeat action.
 * @warning This action can't be Sequenceable because it is not an IntervalAction.
 */
class RepeatForever : public ActionInterval
{
protected:
    ActionInterval* _innerAction = nullptr;

public:
    RepeatForever() = default;
    RepeatForever(RepeatForever const&) = delete;
    RepeatForever& operator=(RepeatForever const&) = delete;
    RepeatForever(RepeatForever&&) noexcept = delete;
    RepeatForever& operator=(RepeatForever&&) noexcept = delete;
    ~RepeatForever() override;

    bool initWithAction(ActionInterval* action);
    static RepeatForever* create(ActionInterval* action);

    /** Sets the inner action.
     *
     * @param action The inner action.
     */
    void setInnerAction(ActionInterval* action);

    /** Gets the inner action.
     *
     * @return The inner action.
     */
    inline ActionInterval* getInnerAction() const noexcept { return _innerAction; }

    RepeatForever* clone() const override;
    RepeatForever* reverse(void) const override;
    void startWithTarget(Node* target) override;
    void step(float dt) override;
    void update(float) override;
};

class CC_DLL Spawn : public ActionInterval
{
    std::vector<FiniteTimeAction*> _actions;

public:
    Spawn() = default;
    Spawn(std::initializer_list<FiniteTimeAction*> actions);
    Spawn(Vector<FiniteTimeAction*> const& actions);
    Spawn(Spawn const&) = delete;
    Spawn& operator=(Spawn const&) = delete;
    Spawn(Spawn&&) noexcept = delete;
    Spawn& operator=(Spawn&&) noexcept = delete;
    ~Spawn() override;

    static Spawn* create(std::initializer_list<FiniteTimeAction*> actions);
    static Spawn* create(Vector<FiniteTimeAction*> const& actions);

    Spawn* clone() const final;
    Spawn* reverse() const final;
    void startWithTarget(Node* target) final;
    void stop() final;
    bool isDone() const final;
    void update(float p) final;
};

/** @class RotateTo
 * @brief Rotates a Node object to a certain angle by modifying it's rotation attribute.
 The direction will be decided by the shortest angle.
*/
class CC_DLL RotateTo : public ActionInterval
{
public:
    /**
     * Creates the action with separate rotation angles.
     *
     * @param duration Duration time, in seconds.
     * @param dstAngleX In degreesCW.
     * @param dstAngleY In degreesCW.
     * @return An autoreleased RotateTo object.
     */
    static RotateTo* create(std::chrono::milliseconds duration, float dstAngleX, float dstAngleY);

    /**
     * Creates the action.
     *
     * @param duration Duration time, in seconds.
     * @param dstAngle In degreesCW.
     * @return An autoreleased RotateTo object.
     */
    static RotateTo* create(std::chrono::milliseconds duration, float dstAngle);

    /**
     * Creates the action with 3D rotation angles.
     * @param duration Duration time, in seconds.
     * @param dstAngle3D A Vec3 angle.
     * @return An autoreleased RotateTo object.
     */
    static RotateTo* create(std::chrono::milliseconds duration, const Vec3& dstAngle3D);

    //
    // Overrides
    //
    RotateTo* clone() const override;
    RotateTo* reverse() const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : RotateTo();
    ~RotateTo() override {}

    /**
     * initializes the action
     * @param duration in seconds
     * @param dstAngleX in degreesCW
     * @param dstAngleY in degreesCW
     */
    bool initWithDuration(std::chrono::milliseconds duration, float dstAngleX, float dstAngleY);
    /**
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, const Vec3& dstAngle3D);

    /**
     * calculates the start and diff angles
     * @param dstAngle in degreesCW
     */
    void calculateAngles(float& startAngle, float& diffAngle, float dstAngle);

protected:
    bool _is3D;
    Vec3 _dstAngle;
    Vec3 _startAngle;
    Vec3 _diffAngle;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(RotateTo)
};

/** @class RotateBy
 * @brief Rotates a Node object clockwise a number of degrees by modifying it's rotation attribute.
 */
class CC_DLL RotateBy : public ActionInterval
{
public:
    /**
     * Creates the action.
     *
     * @param duration Duration time, in seconds.
     * @param deltaAngle In degreesCW.
     * @return An autoreleased RotateBy object.
     */
    static RotateBy* create(std::chrono::milliseconds duration, float deltaAngle);
    /**
     * Creates the action with separate rotation angles.
     *
     * @param duration Duration time, in seconds.
     * @param deltaAngleZ_X In degreesCW.
     * @param deltaAngleZ_Y In degreesCW.
     * @return An autoreleased RotateBy object.
     * @warning The physics body contained in Node doesn't support rotate with different x and y angle.
     */
    static RotateBy* create(std::chrono::milliseconds duration, float deltaAngleZ_X, float deltaAngleZ_Y);
    /** Creates the action with 3D rotation angles.
     *
     * @param duration Duration time, in seconds.
     * @param deltaAngle3D A Vec3 angle.
     * @return An autoreleased RotateBy object.
     */
    static RotateBy* create(std::chrono::milliseconds duration, const Vec3& deltaAngle3D);

    //
    // Override
    //
    RotateBy* clone() const override;
    RotateBy* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : RotateBy();
    ~RotateBy() override {}

    /** initializes the action */
    bool initWithDuration(std::chrono::milliseconds duration, float deltaAngle);
    /**
     * @warning The physics body contained in Node doesn't support rotate with different x and y angle.
     * @param deltaAngleZ_X in degreesCW
     * @param deltaAngleZ_Y in degreesCW
     */
    bool initWithDuration(std::chrono::milliseconds duration, float deltaAngleZ_X, float deltaAngleZ_Y);
    bool initWithDuration(std::chrono::milliseconds duration, Vec3 const& deltaAngle3D);

protected:
    bool _is3D;
    Vec3 _deltaAngle;
    Vec3 _startAngle;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(RotateBy)
};

/** @class MoveBy
 * @brief Moves a Node object x,y pixels by modifying it's position attribute.
 x and y are relative to the position of the object.
 Several MoveBy actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class CC_DLL MoveBy : public ActionInterval
{
    bool _is3D = false;
    Vec3 _startPosition = Vec3::ZERO;
    Vec3 _previousPosition = Vec3::ZERO;

protected:
    Vec3 _positionDelta = Vec3::ZERO;

public:
    MoveBy() = default;
    MoveBy(MoveBy const&) = delete;
    MoveBy& operator=(MoveBy const&) = delete;
    MoveBy(MoveBy&&) noexcept = delete;
    MoveBy& operator=(MoveBy&&) noexcept = delete;
    ~MoveBy() override;

    /** initializes the action */
    bool initWithDuration(std::chrono::milliseconds duration, Vec2 const& deltaPosition);
    bool initWithDuration(std::chrono::milliseconds duration, Vec3 const& deltaPosition);

    /**
     * Creates the action.
     *
     * @param duration Duration time, in seconds.
     * @param deltaPosition The delta distance in 2d, it's a Vec2 type.
     * @return An autoreleased MoveBy object.
     */
    static MoveBy* create(std::chrono::milliseconds duration, Vec2 const& deltaPosition);
    /**
     * Creates the action.
     *
     * @param duration Duration time, in seconds.
     * @param deltaPosition The delta distance in 3d, it's a Vec3 type.
     * @return An autoreleased MoveBy object.
     */
    static MoveBy* create(std::chrono::milliseconds duration, Vec3 const& deltaPosition);

    MoveBy* clone() const override;
    MoveBy* reverse() const override;
    void startWithTarget(Node* target) override;
    void update(float progress) override;
};

/** @class MoveTo
 * @brief Moves a Node object to the position x,y. x and y are absolute coordinates by modifying it's position attribute.
 Several MoveTo actions can be concurrently called, and the resulting
 movement will be the sum of individual movements.
 @since v2.1beta2-custom
 */
class CC_DLL MoveTo final : public MoveBy
{
    Vec3 _endPosition = Vec3::ZERO;

public:
    MoveTo() = default;
    MoveTo(MoveTo const&) = delete;
    MoveTo& operator=(MoveTo const&) = delete;
    MoveTo(MoveTo&&) noexcept = delete;
    MoveTo& operator=(MoveTo&&) noexcept = delete;
    ~MoveTo() final;

    /**
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, Vec2 const& position);
    /**
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, Vec3 const& position);

    /**
     * Creates the action.
     * @param duration Duration time, in seconds.
     * @param position The destination position in 2d.
     * @return An autoreleased MoveTo object.
     */
    static MoveTo* create(std::chrono::milliseconds duration, Vec2 const& position);
    /**
     * Creates the action.
     * @param duration Duration time, in seconds.
     * @param position The destination position in 3d.
     * @return An autoreleased MoveTo object.
     */
    static MoveTo* create(std::chrono::milliseconds duration, Vec3 const& position);

    MoveTo* clone() const final;
    MoveTo* reverse() const final;
    void startWithTarget(Node* target) final;
};

/** @class SkewTo
 * @brief Skews a Node object to given angles by modifying it's skewX and skewY attributes
@since v1.0
*/
class CC_DLL SkewTo : public ActionInterval
{
public:
    /**
     * Creates the action.
     * @param t Duration time, in seconds.
     * @param sx Skew x angle.
     * @param sy Skew y angle.
     * @return An autoreleased SkewTo object.
     */
    static SkewTo* create(std::chrono::milliseconds t, float sx, float sy);

    //
    // Overrides
    //
    SkewTo* clone() const override;
    SkewTo* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : SkewTo();
    ~SkewTo() override {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(std::chrono::milliseconds t, float sx, float sy);

protected:
    float _skewX;
    float _skewY;
    float _startSkewX;
    float _startSkewY;
    float _endSkewX;
    float _endSkewY;
    float _deltaX;
    float _deltaY;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(SkewTo)
};

/** @class SkewBy
* @brief Skews a Node object by skewX and skewY degrees.
@since v1.0
*/
class CC_DLL SkewBy : public SkewTo
{
public:
    /**
     * Creates the action.
     * @param t Duration time, in seconds.
     * @param deltaSkewX Skew x delta angle.
     * @param deltaSkewY Skew y delta angle.
     * @return An autoreleased SkewBy object.
     */
    static SkewBy* create(std::chrono::milliseconds t, float deltaSkewX, float deltaSkewY);

    //
    // Overrides
    //
    void startWithTarget(Node* target) override;
    SkewBy* clone() const override;
    SkewBy* reverse(void) const override;

    CC_CONSTRUCTOR_ACCESS : SkewBy() {}
    ~SkewBy() override {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(std::chrono::milliseconds t, float sx, float sy);

private:
    CC_DISALLOW_COPY_AND_ASSIGN(SkewBy)
};

/** @class ResizeTo
 * @brief Resize a Node object to the final size by modifying it's Size attribute.
 */
class CC_DLL ResizeTo : public ActionInterval
{
public:
    /**
     * Creates the action.
     * @brief Resize a Node object to the final size by modifying it's Size attribute. Works on all nodes where setContentSize is effective. But it's mostly
     * useful for nodes where 9-slice is enabled
     * @param duration Duration time, in seconds.
     * @param final_size The target size to reach
     * @return An autoreleased RotateTo object.
     */
    static ResizeTo* create(std::chrono::milliseconds duration, const cocos2d::Size& final_size);

    //
    // Overrides
    //
    ResizeTo* clone() const override;
    void startWithTarget(cocos2d::Node* target) override;
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : ResizeTo() {}
    ~ResizeTo() override {}

    /**
     * initializes the action
     * @param duration in seconds
     * @param final_size in Size type
     */
    bool initWithDuration(std::chrono::milliseconds duration, const cocos2d::Size& final_size);

protected:
    cocos2d::Size _initialSize;
    cocos2d::Size _finalSize;
    cocos2d::Size _sizeDelta;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ResizeTo)
};

/** @class ResizeBy
 * @brief Resize a Node object by a Size. Works on all nodes where setContentSize is effective. But it's mostly useful for nodes where 9-slice is enabled
 */
class CC_DLL ResizeBy : public ActionInterval
{
public:
    /**
     * Creates the action.
     *
     * @param duration Duration time, in seconds.
     * @param deltaSize The delta size.
     * @return An autoreleased ResizeBy object.
     */
    static ResizeBy* create(std::chrono::milliseconds duration, const cocos2d::Size& deltaSize);

    //
    // Overrides
    //
    ResizeBy* clone() const override;
    ResizeBy* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time in seconds
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : ResizeBy() {}
    ~ResizeBy() override {}

    /** initializes the action */
    bool initWithDuration(std::chrono::milliseconds duration, const cocos2d::Size& deltaSize);

protected:
    cocos2d::Size _sizeDelta;
    cocos2d::Size _startSize;
    cocos2d::Size _previousSize;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ResizeBy)
};

/** @class JumpBy
 * @brief Moves a Node object simulating a parabolic jump movement by modifying it's position attribute.
 */
class CC_DLL JumpBy : public ActionInterval
{
public:
    /**
     * Creates the action.
     * @param duration Duration time, in seconds.
     * @param position The jumping distance.
     * @param height The jumping height.
     * @param jumps The jumping times.
     * @return An autoreleased JumpBy object.
     */
    static JumpBy* create(std::chrono::milliseconds duration, const Vec2& position, float height, int jumps);

    //
    // Overrides
    //
    JumpBy* clone() const override;
    JumpBy* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : JumpBy() {}
    ~JumpBy() override {}

    /**
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, const Vec2& position, float height, int jumps);

protected:
    Vec2 _startPosition;
    Vec2 _delta;
    float _height;
    int _jumps;
    Vec2 _previousPos;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(JumpBy)
};

/** @class JumpTo
 * @brief Moves a Node object to a parabolic position simulating a jump movement by modifying it's position attribute.
 */
class CC_DLL JumpTo : public JumpBy
{
public:
    /**
     * Creates the action.
     * @param duration Duration time, in seconds.
     * @param position The jumping destination position.
     * @param height The jumping height.
     * @param jumps The jumping times.
     * @return An autoreleased JumpTo object.
     */
    static JumpTo* create(std::chrono::milliseconds duration, const Vec2& position, float height, int jumps);

    //
    // Override
    //
    void startWithTarget(Node* target) override;
    JumpTo* clone() const override;
    JumpTo* reverse(void) const override;

    CC_CONSTRUCTOR_ACCESS : JumpTo() {}
    ~JumpTo() override {}

    /**
     * initializes the action
     * @param duration In seconds.
     */
    bool initWithDuration(std::chrono::milliseconds duration, const Vec2& position, float height, int jumps);

protected:
    Vec2 _endPosition;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(JumpTo)
};

/** @struct Bezier configuration structure
 */
typedef struct _ccBezierConfig
{
    //! end position of the bezier
    Vec2 endPosition;
    //! Bezier control point 1
    Vec2 controlPoint_1;
    //! Bezier control point 2
    Vec2 controlPoint_2;
} ccBezierConfig;

/** @class BezierBy
 * @brief An action that moves the target with a cubic Bezier curve by a certain distance.
 */
class CC_DLL BezierBy : public ActionInterval
{
public:
    /** Creates the action with a duration and a bezier configuration.
     * @param t Duration time, in seconds.
     * @param c Bezier config.
     * @return An autoreleased BezierBy object.
     * @code
     * When this function bound to js or lua,the input params are changed.
     * in js: var create(var t,var table)
     * in lua: local create(local t, local table)
     * @endcode
     */
    static BezierBy* create(std::chrono::milliseconds t, const ccBezierConfig& c);

    //
    // Overrides
    //
    BezierBy* clone() const override;
    BezierBy* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : BezierBy() {}
    ~BezierBy() override {}

    /**
     * initializes the action with a duration and a bezier configuration
     * @param t in seconds
     */
    bool initWithDuration(std::chrono::milliseconds t, const ccBezierConfig& c);

protected:
    ccBezierConfig _config;
    Vec2 _startPosition;
    Vec2 _previousPosition;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(BezierBy)
};

/** @class BezierTo
 * @brief An action that moves the target with a cubic Bezier curve to a destination point.
 @since v0.8.2
 */
class CC_DLL BezierTo : public BezierBy
{
public:
    /** Creates the action with a duration and a bezier configuration.
     * @param t Duration time, in seconds.
     * @param c Bezier config.
     * @return An autoreleased BezierTo object.
     * @code
     * when this function bound to js or lua,the input params are changed
     * in js: var create(var t,var table)
     * in lua: local create(local t, local table)
     * @endcode
     */
    static BezierTo* create(std::chrono::milliseconds t, const ccBezierConfig& c);

    //
    // Overrides
    //
    void startWithTarget(Node* target) override;
    BezierTo* clone() const override;
    BezierTo* reverse(void) const override;

    CC_CONSTRUCTOR_ACCESS : BezierTo() {}
    ~BezierTo() override {}
    /**
     * @param t In seconds.
     */
    bool initWithDuration(std::chrono::milliseconds t, const ccBezierConfig& c);

protected:
    ccBezierConfig _toConfig;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(BezierTo)
};

/** @class ScaleTo
 @brief Scales a Node object to a zoom factor by modifying it's scale attribute.
 @warning This action doesn't support "reverse".
 @warning The physics body contained in Node doesn't support this action.
 */
class CC_DLL ScaleTo : public ActionInterval
{
public:
    /**
     * Creates the action with the same scale factor for X and Y.
     * @param duration Duration time, in seconds.
     * @param s Scale factor of x and y.
     * @return An autoreleased ScaleTo object.
     */
    static ScaleTo* create(std::chrono::milliseconds duration, float s);

    /**
     * Creates the action with and X factor and a Y factor.
     * @param duration Duration time, in seconds.
     * @param sx Scale factor of x.
     * @param sy Scale factor of y.
     * @return An autoreleased ScaleTo object.
     */
    static ScaleTo* create(std::chrono::milliseconds duration, float sx, float sy);

    /**
     * Creates the action with X Y Z factor.
     * @param duration Duration time, in seconds.
     * @param sx Scale factor of x.
     * @param sy Scale factor of y.
     * @param sz Scale factor of z.
     * @return An autoreleased ScaleTo object.
     */
    static ScaleTo* create(std::chrono::milliseconds duration, float sx, float sy, float sz);

    //
    // Overrides
    //
    ScaleTo* clone() const override;
    ScaleTo* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : ScaleTo() {}
    ~ScaleTo() override {}

    /**
     * initializes the action with the same scale factor for X and Y
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, float s);
    /**
     * initializes the action with and X factor and a Y factor
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, float sx, float sy);
    /**
     * initializes the action with X Y Z factor
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, float sx, float sy, float sz);

protected:
    float _scaleX;
    float _scaleY;
    float _scaleZ;
    float _startScaleX;
    float _startScaleY;
    float _startScaleZ;
    float _endScaleX;
    float _endScaleY;
    float _endScaleZ;
    float _deltaX;
    float _deltaY;
    float _deltaZ;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ScaleTo)
};

/** @class ScaleBy
 * @brief Scales a Node object a zoom factor by modifying it's scale attribute.
 @warning The physics body contained in Node doesn't support this action.
*/
class CC_DLL ScaleBy : public ScaleTo
{
public:
    /**
     * Creates the action with the same scale factor for X and Y.
     * @param duration Duration time, in seconds.
     * @param s Scale factor of x and y.
     * @return An autoreleased ScaleBy object.
     */
    static ScaleBy* create(std::chrono::milliseconds duration, float s);

    /**
     * Creates the action with and X factor and a Y factor.
     * @param duration Duration time, in seconds.
     * @param sx Scale factor of x.
     * @param sy Scale factor of y.
     * @return An autoreleased ScaleBy object.
     */
    static ScaleBy* create(std::chrono::milliseconds duration, float sx, float sy);

    /**
     * Creates the action with X Y Z factor.
     * @param duration Duration time, in seconds.
     * @param sx Scale factor of x.
     * @param sy Scale factor of y.
     * @param sz Scale factor of z.
     * @return An autoreleased ScaleBy object.
     */
    static ScaleBy* create(std::chrono::milliseconds duration, float sx, float sy, float sz);

    //
    // Overrides
    //
    void startWithTarget(Node* target) override;
    ScaleBy* clone() const override;
    ScaleBy* reverse(void) const override;

    CC_CONSTRUCTOR_ACCESS : ScaleBy() {}
    ~ScaleBy() override {}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ScaleBy)
};

/** @class Blink
 * @brief Blinks a Node object by modifying it's visible attribute.
 */
class CC_DLL Blink : public ActionInterval
{
public:
    /**
     * Creates the action.
     * @param duration Duration time, in seconds.
     * @param blinks Blink times.
     * @return An autoreleased Blink object.
     */
    static Blink* create(std::chrono::milliseconds duration, int blinks);

    //
    // Overrides
    //
    Blink* clone() const override;
    Blink* reverse() const override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;
    void startWithTarget(Node* target) override;
    void stop() override;

    CC_CONSTRUCTOR_ACCESS : Blink() {}
    ~Blink() override {}

    /**
     * initializes the action
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, int blinks);

protected:
    int _times;
    bool _originalState;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Blink)
};

/** @class FadeTo
 * @brief Fades an object that implements the RGBAProtocol protocol. It modifies the opacity from the current value to a custom one.
 @warning This action doesn't support "reverse"
 */
class CC_DLL FadeTo : public ActionInterval
{
public:
    /**
     * Creates an action with duration and opacity.
     * @param duration Duration time, in seconds.
     * @param opacity A certain opacity, the range is from 0 to 255.
     * @return An autoreleased FadeTo object.
     */
    static FadeTo* create(std::chrono::milliseconds duration, GLubyte opacity);

    //
    // Overrides
    //
    FadeTo* clone() const override;
    FadeTo* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : FadeTo() {}
    ~FadeTo() override {}

    /**
     * initializes the action with duration and opacity
     * @param duration in seconds
     */
    bool initWithDuration(std::chrono::milliseconds duration, GLubyte opacity);

protected:
    GLubyte _toOpacity;
    GLubyte _fromOpacity;
    friend class FadeOut;
    friend class FadeIn;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FadeTo)
};

/** @class FadeIn
 * @brief Fades In an object that implements the RGBAProtocol protocol. It modifies the opacity from 0 to 255.
 The "reverse" of this action is FadeOut
 */
class CC_DLL FadeIn : public FadeTo
{
public:
    /**
     * Creates the action.
     * @param d Duration time, in seconds.
     * @return An autoreleased FadeIn object.
     */
    static FadeIn* create(std::chrono::milliseconds d);

    //
    // Overrides
    //
    void startWithTarget(Node* target) override;
    FadeIn* clone() const override;
    FadeTo* reverse(void) const override;

    /**
     * @js NA
     */
    void setReverseAction(FadeTo* ac);

    CC_CONSTRUCTOR_ACCESS : FadeIn()
    : _reverseAction()
    {
    }
    ~FadeIn() override {}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FadeIn)
    FadeTo* _reverseAction;
};

/** @class FadeOut
 * @brief Fades Out an object that implements the RGBAProtocol protocol. It modifies the opacity from 255 to 0.
 The "reverse" of this action is FadeIn
*/
class CC_DLL FadeOut : public FadeTo
{
public:
    /**
     * Creates the action.
     * @param d Duration time, in seconds.
     */
    static FadeOut* create(std::chrono::milliseconds d);

    //
    // Overrides
    //
    void startWithTarget(Node* target) override;
    FadeOut* clone() const override;
    FadeTo* reverse(void) const override;

    /**
     * @js NA
     */
    void setReverseAction(FadeTo* ac);

    CC_CONSTRUCTOR_ACCESS : FadeOut()
    : _reverseAction()
    {
    }
    ~FadeOut() override {}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(FadeOut)
    FadeTo* _reverseAction;
};

/** @class TintTo
 * @brief Tints a Node that implements the NodeRGB protocol from current tint to a custom one.
 @warning This action doesn't support "reverse"
 @since v0.7.2
*/
class CC_DLL TintTo : public ActionInterval
{
public:
    /**
     * Creates an action with duration and color.
     * @param duration Duration time, in seconds.
     * @param red Red Color, from 0 to 255.
     * @param green Green Color, from 0 to 255.
     * @param blue Blue Color, from 0 to 255.
     * @return An autoreleased TintTo object.
     */
    static TintTo* create(std::chrono::milliseconds duration, GLubyte red, GLubyte green, GLubyte blue);
    /**
     * Creates an action with duration and color.
     * @param duration Duration time, in seconds.
     * @param color It's a Color3B type.
     * @return An autoreleased TintTo object.
     */
    static TintTo* create(std::chrono::milliseconds duration, const Color3B& color);

    //
    // Overrides
    //
    TintTo* clone() const override;
    TintTo* reverse(void) const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : TintTo() {}
    ~TintTo() override {}

    /** initializes the action with duration and color */
    bool initWithDuration(std::chrono::milliseconds duration, GLubyte red, GLubyte green, GLubyte blue);

protected:
    Color3B _to;
    Color3B _from;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(TintTo)
};

/** @class TintBy
 @brief Tints a Node that implements the NodeRGB protocol from current tint to a custom one.
 @since v0.7.2
 */
class CC_DLL TintBy : public ActionInterval
{
public:
    /**
     * Creates an action with duration and color.
     * @param duration Duration time, in seconds.
     * @param deltaRed Delta red color.
     * @param deltaGreen Delta green color.
     * @param deltaBlue Delta blue color.
     * @return An autoreleased TintBy object.
     */
    static TintBy* create(std::chrono::milliseconds duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue);

    //
    // Overrides
    //
    TintBy* clone() const override;
    TintBy* reverse() const override;
    void startWithTarget(Node* target) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : TintBy() {}
    ~TintBy() override {}

    /** initializes the action with duration and color */
    bool initWithDuration(std::chrono::milliseconds duration, GLshort deltaRed, GLshort deltaGreen, GLshort deltaBlue);

protected:
    GLshort _deltaR;
    GLshort _deltaG;
    GLshort _deltaB;

    GLshort _fromR;
    GLshort _fromG;
    GLshort _fromB;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(TintBy)
};

/** @class DelayTime
 * @brief Delays the action a certain amount of seconds.
 */
class CC_DLL DelayTime : public ActionInterval
{
public:
    /**
     * Creates the action.
     * @param d Duration time, in seconds.
     * @return An autoreleased DelayTime object.
     */
    static DelayTime* create(std::chrono::milliseconds d);

    //
    // Overrides
    //
    /**
     * @param time In seconds.
     */
    void update(float time) override;
    DelayTime* reverse() const override;
    DelayTime* clone() const override;

    CC_CONSTRUCTOR_ACCESS : DelayTime() {}
    ~DelayTime() override {}

private:
    CC_DISALLOW_COPY_AND_ASSIGN(DelayTime)
};

/** @class ReverseTime
 * @brief Executes an action in reverse order, from time=duration to time=0

 @warning Use this action carefully. This action is not
 sequenceable. Use it as the default "reversed" method
 of your own actions, but using it outside the "reversed"
 scope is not recommended.
*/
class CC_DLL ReverseTime : public ActionInterval
{
public:
    /** Creates the action.
     *
     * @param action a certain action.
     * @return An autoreleased ReverseTime object.
     */
    static ReverseTime* create(FiniteTimeAction* action);

    //
    // Overrides
    //
    ReverseTime* reverse() const override;
    ReverseTime* clone() const override;
    void startWithTarget(Node* target) override;
    void stop(void) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    CC_CONSTRUCTOR_ACCESS : ReverseTime();
    ~ReverseTime() override;

    /** initializes the action */
    bool initWithAction(FiniteTimeAction* action);

protected:
    FiniteTimeAction* _other;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ReverseTime)
};

/** @class Animate
 * @brief Animates a sprite given the name of an Animation.
 */
class CC_DLL Animate : public ActionInterval
{
public:
    /** Creates the action with an Animation and will restore the original frame when the animation is over.
     *
     * @param animation A certain animation.
     * @return An autoreleased Animate object.
     */
    static Animate* create(Animation* animation);

    /** Sets the Animation object to be animated
     *
     * @param animation certain animation.
     */
    void setAnimation(Animation* animation);
    /** returns the Animation object that is being animated
     *
     * @return Gets the animation object that is being animated.
     */
    inline Animation* getAnimation() const noexcept { return _animation; }

    /**
     * Gets the index of sprite frame currently displayed.
     * @return int  the index of sprite frame currently displayed.
     */
    inline int getCurrentFrameIndex() const noexcept { return _currFrameIndex; }
    //
    // Overrides
    //
    Animate* clone() const override;
    Animate* reverse() const override;
    void startWithTarget(Node* target) override;
    void stop(void) override;
    /**
     * @param t In seconds.
     */
    void update(float t) override;

    CC_CONSTRUCTOR_ACCESS : Animate() = default;
    ~Animate() override;

    /** initializes the action with an Animation and will restore the original frame when the animation is over */
    bool initWithAnimation(Animation* animation);

protected:
    std::vector<float> _splitTimes;
    std::size_t _nextFrame = 0;
    SpriteFrame* _origFrame = nullptr;
    std::size_t _currFrameIndex = 0;
    unsigned int _executedLoops = 0;
    Animation* _animation = nullptr;

    EventCustom* _frameDisplayedEvent = nullptr;
    AnimationFrame::DisplayedEventInfo _frameDisplayedEventInfo;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(Animate)
};

/** @class TargetedAction
 * @brief Overrides the target of an action so that it always runs on the target
 * specified at action creation rather than the one specified by runAction.
 */
class CC_DLL TargetedAction : public ActionInterval
{
public:
    /** Create an action with the specified action and forced target.
     *
     * @param target The target needs to override.
     * @param action The action needs to override.
     * @return An autoreleased TargetedAction object.
     */
    static TargetedAction* create(Node* target, FiniteTimeAction* action);

    /** Sets the target that the action will be forced to run with.
     *
     * @param forcedTarget The target that the action will be forced to run with.
     */
    void setForcedTarget(Node* forcedTarget);
    /** returns the target that the action is forced to run with.
     *
     * @return The target that the action is forced to run with.
     */
    inline Node* getForcedTarget() const noexcept { return _forcedTarget; }

    //
    // Overrides
    //
    TargetedAction* clone() const override;
    TargetedAction* reverse() const override;
    void startWithTarget(Node* target) override;
    void step(float dt) override;
    void stop(void) override;
    /**
     * @param time In seconds.
     */
    void update(float time) override;

    bool isDone() const override;

    CC_CONSTRUCTOR_ACCESS : TargetedAction();
    ~TargetedAction() override;

    /** Init an action with the specified action and forced target */
    bool initWithTarget(Node* target, FiniteTimeAction* action);

protected:
    FiniteTimeAction* _action;
    Node* _forcedTarget;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(TargetedAction)
};

/**
 * @class ActionFloat
 * @brief Action used to animate any value in range [from,to] over specified time interval
 */
class CC_DLL ActionFloat : public ActionInterval
{
public:
    /**
     *  Callback function used to report back result
     */
    typedef std::function<void(float value)> ActionFloatCallback;

    /**
     * Creates FloatAction with specified duration, from value, to value and callback to report back
     * results
     * @param duration of the action
     * @param from value to start from
     * @param to value to be at the end of the action
     * @param callback to report back result
     *
     * @return An autoreleased ActionFloat object
     */
    static ActionFloat* create(std::chrono::milliseconds duration, float from, float to, ActionFloatCallback callback);

    /**
     * Overridden ActionInterval methods
     */
    void startWithTarget(Node* target) override;
    void update(float delta) override;
    ActionFloat* reverse() const override;
    ActionFloat* clone() const override;

    CC_CONSTRUCTOR_ACCESS : ActionFloat() {}
    ~ActionFloat() override {}

    bool initWithDuration(std::chrono::milliseconds duration, float from, float to, ActionFloatCallback callback);

protected:
    /* From value */
    float _from;
    /* To value */
    float _to;
    /* delta time */
    float _delta;

    /* Callback to report back results */
    ActionFloatCallback _callback;

private:
    CC_DISALLOW_COPY_AND_ASSIGN(ActionFloat)
};

// end of actions group
/// @}

NS_CC_END

#endif // CC_2D_ACTIONINTERVAL_H
