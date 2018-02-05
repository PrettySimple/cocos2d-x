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
}

NS_CC_END
