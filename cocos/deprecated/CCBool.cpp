//
//  CCBool.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 20/06/2018.
//

#include <cocos/deprecated/CCBool.h>

#include <cocos/base/CCDataVisitor.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <new>

NS_CC_BEGIN

__Bool::~__Bool()
{
}

__Bool* __Bool::create(bool v)
{
    __Bool* pRet = new (std::nothrow) __Bool(v);
    if (pRet != nullptr)
    {
        pRet->autorelease();
    }
    return pRet;
}

void __Bool::acceptVisitor(DataVisitor& visitor)
{
    visitor.visit(this);
}

__Bool* __Bool::clone() const
{
    return __Bool::create(_value);
}

NS_CC_END
