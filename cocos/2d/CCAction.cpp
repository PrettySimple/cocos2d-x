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

#include "2d/CCAction.h"
#include "2d/CCActionInterval.h"
#include "2d/CCNode.h"
#include "base/CCDirector.h"
#include "base/ccMacros.h"
#include "base/ccUTF8.h"

NS_CC_BEGIN
//
// Action Base Class
//

Action::Action()
{
#if CC_ENABLE_SCRIPT_BINDING
    ScriptEngineProtocol* engine = ScriptEngineManager::getInstance()->getScriptEngine();
    _scriptType = engine != nullptr ? engine->getScriptType() : kScriptTypeNone;
#endif
}

Action::~Action()
{
    CCLOGINFO("deallocing Action: %p - tag: %i", this, _tag);
}

std::string Action::description() const
{
    return StringUtils::format("<Action | Tag = %d", _tag);
}

Action* Action::clone() const
{
    CCASSERT(false, "must be implemented");
    return nullptr;
}

Action* Action::reverse() const
{
    CCASSERT(false, "must be implemented");
    return nullptr;
}

bool Action::isDone() const
{
    CCASSERT(false, "must be implemented");
    return true;
}

void Action::startWithTarget(Node* aTarget)
{
    _originalTarget = aTarget;
    _status = Action::Status::START;
    _target = aTarget;
}

void Action::stop()
{
    _target = nullptr;
    _status = Action::Status::DONE;
}

void Action::step(float)
{
    CCASSERT(false, "must be implemented");
}

void Action::update(float)
{
    CCASSERT(false, "must be implemented");
}

// FiniteTimeAction

FiniteTimeAction::~FiniteTimeAction()
{
}

FiniteTimeAction* FiniteTimeAction::clone() const
{
    CCASSERT(false, "must be implemented");
    return nullptr;
}

FiniteTimeAction* FiniteTimeAction::reverse() const
{
    CCASSERT(false, "must be implemented");
    return nullptr;
}

//
// Speed
//

Speed::~Speed()
{
    CC_SAFE_RELEASE(_innerAction);
}

Speed* Speed::create(ActionInterval* action, float speed)
{
    Speed *ret = new (std::nothrow) Speed();
    if (ret && ret->initWithAction(action, speed))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool Speed::initWithAction(ActionInterval* action, float speed)
{
    CCASSERT(action != nullptr, "action must not be NULL");
    if (action == nullptr)
    {
        log("Speed::initWithAction error: action is nullptr!");
        return false;
    }
    
    action->retain();
    _innerAction = action;
    _speed = speed;
    return true;
}

Speed* Speed::clone() const
{
    // no copy constructor
    if (_innerAction)
        return Speed::create(_innerAction->clone(), _speed);
    
    return nullptr;
}

void Speed::startWithTarget(Node* target)
{
    if (target && _innerAction)
    {
        Action::startWithTarget(target);
        _innerAction->startWithTarget(target);
    }
    else
        log("Speed::startWithTarget error: target(%p) or _innerAction(%p) is nullptr!", target, _innerAction);
}

void Speed::stop()
{
    if (_innerAction)
        _innerAction->stop();
    
    Action::stop();
}

void Speed::step(float dt)
{
    _innerAction->step(dt * _speed);
}

bool Speed::isDone() const
{
    return _innerAction->isDone();
}

Speed *Speed::reverse() const
{
    if (_innerAction)
        return Speed::create(_innerAction->reverse(), _speed);
    
    return nullptr;
}

void Speed::setInnerAction(ActionInterval *action)
{
    if (_innerAction != action)
    {
        CC_SAFE_RELEASE(_innerAction);
        _innerAction = action;
        CC_SAFE_RETAIN(_innerAction);
    }
}

//
// Follow
//
Follow::~Follow()
{
    CC_SAFE_RELEASE(_followedNode);
}

Follow* Follow::create(Node *followedNode, const Rect& rect/* = Rect::ZERO*/)
{
    return createWithOffset(followedNode, 0.0, 0.0,rect);
}

Follow* Follow::createWithOffset(Node* followedNode,float xOffset,float yOffset,const Rect& rect/*= Rect::ZERO*/){
    
    
    Follow *follow = new (std::nothrow) Follow();
    
    bool valid;
    
    valid = follow->initWithTargetAndOffset(followedNode, xOffset, yOffset,rect);

    if (follow && valid)
    {
        follow->autorelease();
        return follow;
    }
    
    delete follow;
    return nullptr;
    
}

Follow* Follow::clone() const
{
    // no copy constructor
    return Follow::createWithOffset(_followedNode, _offsetX,_offsetY,_worldRect);
    
}

Follow* Follow::reverse() const
{
    return clone();
}

bool Follow::initWithTargetAndOffset(Node* followedNode, float xOffset, float yOffset, Rect const& rect /*= Rect::ZERO*/)
{
    CCASSERT(followedNode != nullptr, "FollowedNode can't be NULL");
    if(followedNode == nullptr)
    {
        log("Follow::initWithTarget error: followedNode is nullptr!");
        return false;
    }
 
    followedNode->retain();
    _followedNode = followedNode;
    _worldRect = rect;
    _boundarySet = !rect.equals(Rect::ZERO);
    _boundaryFullyCovered = false;

    Size winSize = Director::getInstance()->getWinSize();
    _fullScreenSize.set(winSize.width, winSize.height);
    _halfScreenSize = _fullScreenSize * 0.5f;
    _offsetX=xOffset;
    _offsetY=yOffset;
    _halfScreenSize.x += _offsetX;
    _halfScreenSize.y += _offsetY;
    
    if (_boundarySet)
    {
        _leftBoundary = -((rect.origin.x+rect.size.width) - _fullScreenSize.x);
        _rightBoundary = -rect.origin.x ;
        _topBoundary = -rect.origin.y;
        _bottomBoundary = -((rect.origin.y+rect.size.height) - _fullScreenSize.y);

        if(_rightBoundary < _leftBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            _rightBoundary = _leftBoundary = (_leftBoundary + _rightBoundary) / 2;
        }
        if(_topBoundary < _bottomBoundary)
        {
            // screen width is larger than world's boundary width
            //set both in the middle of the world
            _topBoundary = _bottomBoundary = (_topBoundary + _bottomBoundary) / 2;
        }

        if( (_topBoundary == _bottomBoundary) && (_leftBoundary == _rightBoundary) )
        {
            _boundaryFullyCovered = true;
        }
    }
    
    return true;
}

bool Follow::initWithTarget(Node* followedNode, Rect const& rect /*= Rect::ZERO*/)
{
    return initWithTargetAndOffset(followedNode, 0.0, 0.0,rect);
}

void Follow::step(float)
{
    if(_boundarySet)
    {
        // whole map fits inside a single screen, no need to modify the position - unless map boundaries are increased
        if(_boundaryFullyCovered)
        {
            return;
        }

        Vec2 tempPos = _halfScreenSize - _followedNode->getPosition();

        _target->setPosition(clampf(tempPos.x, _leftBoundary, _rightBoundary),
                                   clampf(tempPos.y, _bottomBoundary, _topBoundary));
    }
    else
    {
        _target->setPosition(_halfScreenSize - _followedNode->getPosition());
    }
}

bool Follow::isDone() const
{
    return !_followedNode->isRunning();
}

NS_CC_END


