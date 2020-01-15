/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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


#pragma once

#include <cocos/base/CCRef.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <chrono>
#include <iosfwd>

NS_CC_BEGIN

class Node;

enum
{
    kActionUpdate
};

/**
 * @addtogroup actions
 * @{
 */

/**
 * @brief Base class for Action objects.
 */
class Action : public Ref, public Clonable
{
public:
    /** Default tag used for all the actions. */
    static constexpr int const INVALID_TAG = -1;

    enum struct Status : std::uint8_t
    {
        UNKNOWN = 0,
        START = 1,
        RUNNING = 2,
        DONE = 3
    };

protected:
    Node* _originalTarget = nullptr;
    /**
     * The "target".
     * The target will be set with the 'startWithTarget' method.
     * When the 'stop' method is called, target will be set to nil.
     * The target is 'assigned', it is not 'retained'.
     */
    Node* _target = nullptr;
    /** The action tag. An identifier of the action. */
    int _tag = -1;
    /** The action flag field. To categorize action into certain groups.*/
    unsigned int _flags = 0;
    Status _status = Status::UNKNOWN;

#if CC_ENABLE_SCRIPT_BINDING
    ccScriptType _scriptType; ///< type of script binding, lua or javascript
#endif

public:
    Action();
    Action(Action const&) = delete;
    Action& operator=(Action const&) = delete;
    Action(Action&&) noexcept = delete;
    Action& operator=(Action&&) noexcept = delete;
    ~Action() override;

    /**
     * @js NA
     * @lua NA
     */
    virtual std::string description() const;

    /** Returns a clone of action.
     *
     * @return A clone action.
     */
    virtual Action* clone() const override;

    /** Returns a new action that performs the exact reverse of the action.
     *
     * @return A new action that performs the exact reverse of the action.
     * @js NA
     */
    virtual Action* reverse() const;

    /** Return true if the action has finished.
     *
     * @return Is true if the action has finished.
     */
    virtual bool isDone() const;

    /** Called before the action start. It will also set the target.
     *
     * @param target A certain target.
     */
    virtual void startWithTarget(Node* target);

    /**
     * Called after the action has finished. It will set the 'target' to nil.
     * IMPORTANT: You should never call "Action::stop()" manually. Instead, use: "target->stopAction(action);".
     */
    virtual void stop();

    /** Called every frame with it's delta time, dt in seconds. DON'T override unless you know what you are doing.
     *
     * @param dt In seconds.
     */
    virtual void step(float dt);

    /**
     * Called once per frame. time a value between 0 and 1.

     * For example:
     * - 0 Means that the action just started.
     * - 0.5 Means that the action is in the middle.
     * - 1 Means that the action is over.
     *
     * @param time A value between 0 and 1.
     */
    virtual void update(float p);
    /** Return certain target.
     *
     * @return A certain target.
     */
    inline Node* getTarget() const noexcept { return _target; }
    /** The action will modify the target properties.
     *
     * @param target A certain target.
     */
    inline void setTarget(Node* target) noexcept { _target = target; }
    /** Return a original Target.
     *
     * @return A original Target.
     */
    inline Node* getOriginalTarget() const noexcept { return _originalTarget; }
    /**
     * Set the original target, since target can be nil.
     * Is the target that were used to run the action. Unless you are doing something complex, like ActionManager, you should NOT call this method.
     * The target is 'assigned', it is not 'retained'.
     * @since v0.8.2
     *
     * @param originalTarget Is 'assigned', it is not 'retained'.
     */
    inline void setOriginalTarget(Node* originalTarget) noexcept { _originalTarget = originalTarget; }
    /** Returns a tag that is used to identify the action easily.
     *
     * @return A tag.
     */
    inline int getTag() const noexcept { return _tag; }
    /** Changes the tag that is used to identify the action easily.
     *
     * @param tag Used to identify the action easily.
     */
    inline void setTag(int tag) noexcept { _tag = tag; }
    /** Returns a flag field that is used to group the actions easily.
     *
     * @return A tag.
     */
    inline unsigned int getFlags() const noexcept { return _flags; }
    /** Changes the flag field that is used to group the actions easily.
     *
     * @param flags Used to group the actions easily.
     */
    inline void setFlags(unsigned int flags) noexcept { _flags = flags; }

    inline Action::Status getStatus() const noexcept { return _status; }
    inline void setStatus(Action::Status status) noexcept { _status = status; }
};

/** @class FiniteTimeAction
 * @brief
 * Base class actions that do have a finite time duration.
 * Possible actions:
 * - An action with a duration of 0 seconds.
 * - An action with a duration of 35.5 seconds.
 * Infinite time actions are valid.
 */
class FiniteTimeAction : public Action
{
protected:
    std::chrono::milliseconds _duration = std::chrono::milliseconds::zero();

public:
    FiniteTimeAction() = default;
    FiniteTimeAction(FiniteTimeAction const&) = delete;
    FiniteTimeAction& operator=(FiniteTimeAction const&) = delete;
    FiniteTimeAction(FiniteTimeAction&&) noexcept = delete;
    FiniteTimeAction& operator=(FiniteTimeAction&&) noexcept = delete;
    ~FiniteTimeAction() override;

    /** Get duration in seconds of the action.
     *
     * @return The duration in seconds of the action.
     */
    inline std::chrono::milliseconds getDuration() const noexcept { return _duration; }
    /** Set duration in seconds of the action.
     *
     * @param duration In seconds of the action.
     */
    inline void setDuration(std::chrono::milliseconds duration) noexcept { _duration = duration; }

    FiniteTimeAction* clone() const override;
    FiniteTimeAction* reverse() const override;
};

class ActionInterval;

/** @class Speed
 * @brief Changes the speed of an action, making it take longer (speed>1)
 * or shorter (speed<1) time.
 * Useful to simulate 'slow motion' or 'fast forward' effect.
 * @warning This action can't be Sequenceable because it is not an IntervalAction.
 */
class CC_DLL Speed : public Action
{
private:
    float _speed = 0.f;
    ActionInterval* _innerAction = nullptr;

public:
    Speed() = default;
    Speed(Speed const&) = delete;
    Speed& operator=(Speed const&) = delete;
    Speed(Speed&&) noexcept = delete;
    Speed& operator=(Speed&&) noexcept = delete;
    ~Speed() override;

    bool initWithAction(ActionInterval* action, float speed);

    /** Create the action and set the speed.
     *
     * @param action An action.
     * @param speed The action speed.
     */
    static Speed* create(ActionInterval* action, float speed);
    /** Return the speed.
     *
     * @return The action speed.
     */
    inline float getSpeed() const noexcept { return _speed; }
    /** Alter the speed of the inner function in runtime.
     *
     * @param speed Alter the speed of the inner function in runtime.
     */
    inline void setSpeed(float speed) noexcept { _speed = speed; }

    /** Replace the interior action.
     *
     * @param action The new action, it will replace the running action.
     */
    void setInnerAction(ActionInterval* action);
    /** Return the interior action.
     *
     * @return The interior action.
     */
    inline ActionInterval* getInnerAction() const noexcept { return _innerAction; }

    //
    // Override
    //
    Speed* clone() const override;
    Speed* reverse() const override;
    void startWithTarget(Node* target) override;
    void stop() override;
    void step(float dt) override;
    bool isDone() const override;
};

/** @class Follow
 * @brief Follow is an action that "follows" a node.
 * Eg:
 * @code
 * layer->runAction(Follow::create(hero));
 * @endcode
 * Instead of using Camera as a "follower", use this action instead.
 * @since v0.99.2
 */
class Follow : public Action
{
protected:
    /** Node to follow. */
    Node* _followedNode = nullptr;

    /** Whether camera should be limited to certain area. */
    bool _boundarySet = false;

    /** If screen size is bigger than the boundary - update not needed. */
    bool _boundaryFullyCovered = false;

    /** Fast access to the screen dimensions. */
    Vec2 _halfScreenSize = Vec2::ZERO;
    Vec2 _fullScreenSize = Vec2::ZERO;

    /** World boundaries. */
    float _leftBoundary = 0.f;
    float _rightBoundary = 0.f;
    float _topBoundary = 0.f;
    float _bottomBoundary = 0.f;

    /** Horizontal (x) and vertical (y) offset values. */
    float _offsetX = 0.f;
    float _offsetY = 0.f;

    Rect _worldRect = Rect::ZERO;

public:
    Follow() = default;
    Follow(Follow const&) = delete;
    Follow& operator=(Follow const&) = delete;
    Follow(Follow&&) noexcept = delete;
    Follow& operator=(Follow&&) noexcept = delete;
    ~Follow() override;

    /**
     * Initializes the action with a set boundary or with no boundary.
     *
     * @param followedNode  The node to be followed.
     * @param rect  The boundary. If \p rect is equal to Rect::ZERO, it'll work
     *              with no boundary.
     */
    bool initWithTarget(Node* followedNode, Rect const& rect = Rect::ZERO);

    /**
     * Initializes the action with a set boundary or with no boundary with offsets.
     *
     * @param followedNode  The node to be followed.
     * @param rect  The boundary. If \p rect is equal to Rect::ZERO, it'll work
     *              with no boundary.
     * @param xOffset The horizontal offset from the center of the screen from which the
     *                node  is to be followed.It can be positive,negative or zero.If
     *                set to zero the node will be horizontally centered followed.
     * @param yOffset The vertical offset from the center of the screen from which the
     *                node is to be followed.It can be positive,negative or zero.
     *                If set to zero the node will be vertically centered followed.
     *   If both xOffset and yOffset are set to zero,then the node will be horizontally and vertically centered followed.

     */
    bool initWithTargetAndOffset(Node* followedNode, float xOffset, float yOffset, Rect const& rect = Rect::ZERO);

    /**
     * Creates the action with a set boundary or with no boundary.
     *
     * @param followedNode  The node to be followed.
     * @param rect  The boundary. If \p rect is equal to Rect::ZERO, it'll work
     *              with no boundary.
     */
    static Follow* create(Node* followedNode, Rect const& rect = Rect::ZERO);

    /**
     * Creates the action with a set boundary or with no boundary with offsets.
     *
     * @param followedNode  The node to be followed.
     * @param rect  The boundary. If \p rect is equal to Rect::ZERO, it'll work
     *              with no boundary.
     * @param xOffset The horizontal offset from the center of the screen from which the
     *               node  is to be followed.It can be positive,negative or zero.If
     *               set to zero the node will be horizontally centered followed.
     *  @param yOffset The vertical offset from the center of the screen from which the
     *                 node is to be followed.It can be positive,negative or zero.
     *                 If set to zero the node will be vertically centered followed.
     *   If both xOffset and yOffset are set to zero,then the node will be horizontally and vertically centered followed.
     */
    static Follow* createWithOffset(Node* followedNode, float xOffset, float yOffset, Rect const& rect = Rect::ZERO);

    /** Return boundarySet.
     *
     * @return Return boundarySet.
     */
    inline bool isBoundarySet() const noexcept { return _boundarySet; }
    /** Alter behavior - turn on/off boundary.
     *
     * @param value Turn on/off boundary.
     */
    inline void setBoundarySet(bool value) noexcept { _boundarySet = value; }

    /** @deprecated Alter behavior - turn on/off boundary.
     *
     * @param value Turn on/off boundary.
     */
    CC_DEPRECATED_ATTRIBUTE inline void setBoudarySet(bool value) noexcept { setBoundarySet(value); }

    //
    // Override
    //
    Follow* clone() const override;
    Follow* reverse() const override;
    void step(float dt) override;
    bool isDone() const override;
};

// end of actions group
/// @}

NS_CC_END