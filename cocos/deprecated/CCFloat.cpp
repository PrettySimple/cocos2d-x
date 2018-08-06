//
//  CCFloat.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 20/06/2018.
//

#include <cocos/deprecated/CCFloat.h>

#include <cocos/base/CCDataVisitor.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <new>

NS_CC_BEGIN

__Float::~__Float()
{
}

__Float* __Float::create(float v)
{
    __Float* pRet = new (std::nothrow) __Float(v);
    if (pRet != nullptr)
    {
        pRet->autorelease();
    }
    return pRet;
}

void __Float::acceptVisitor(DataVisitor& visitor)
{
    visitor.visit(this);
}

__Float* __Float::clone() const
{
    return __Float::create(_value);
}

NS_CC_END
