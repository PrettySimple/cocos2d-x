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

ActionManagerRecord::ActionManagerRecord(Node* n /*= nullptr*/, Action* a /*= nullptr*/) : target(n)
, action(a)
{
    CC_SAFE_RETAIN(target);
    CC_SAFE_RETAIN(action);
}

ActionManagerRecord::~ActionManagerRecord()
{
    CC_SAFE_RELEASE(target);
    CC_SAFE_RELEASE(action);
}

ActionManagerRecord::ActionManagerRecord(ActionManagerRecord&& other) noexcept
{
    target = other.target;
    action = other.action;

    other.target = nullptr;
    other.action = nullptr;
}

ActionManagerRecord& ActionManagerRecord::operator=(ActionManagerRecord&& other) noexcept
{
    target = other.target;
    action = other.action;

    other.target = nullptr;
    other.action = nullptr;

    return *this;
}

NS_CC_END
