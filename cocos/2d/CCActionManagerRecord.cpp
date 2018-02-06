//
//  CCActionManagerRecord.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#include "CCActionManagerRecord.h"

#include "2d/CCAction.h"
#include "2d/CCNode.h"

NS_CC_BEGIN

ActionManagerRecord::ActionManagerRecord(ActionManagerOperation o, Node* n /*= nullptr*/, Action* a /*= nullptr*/, bool p /*= false*/, int t /*= -1*/, unsigned int f /*= 0*/) : operation(o)
, target(n)
, action(a)
, paused(p)
, tag(t)
, flags(f)
{
    CC_SAFE_RETAIN(target);
    CC_SAFE_RETAIN(action);
}

ActionManagerRecord::ActionManagerRecord(ActionManagerRecord&& other) noexcept
{
    operation = other.operation;
    target = other.target;
    action = other.action;
    paused = other.paused;
    tag = other.tag;
    flags = other.flags;

    other.target = nullptr;
    other.action = nullptr;
}

ActionManagerRecord& ActionManagerRecord::operator=(ActionManagerRecord&& other) noexcept
{
    operation = other.operation;
    target = other.target;
    action = other.action;
    paused = other.paused;
    tag = other.tag;
    flags = other.flags;

    other.target = nullptr;
    other.action = nullptr;

    return *this;
}

ActionManagerRecord::~ActionManagerRecord()
{
    CC_SAFE_RELEASE(target);
    CC_SAFE_RELEASE(action);
}

NS_CC_END
