//
//  CCDouble.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 20/06/2018.
//

#include "CCDouble.h"

#include <cocos/base/CCDataVisitor.h>

#include <new>

NS_CC_BEGIN

__Double::~__Double()
{
}

__Double* __Double::create(double v)
{
    __Double* pRet = new (std::nothrow) __Double(v);
    if (pRet != nullptr)
    {
        pRet->autorelease();
    }
    return pRet;
}

void __Double::acceptVisitor(DataVisitor& visitor)
{
    visitor.visit(this);
}

__Double* __Double::clone() const
{
    return __Double::create(_value);
}

NS_CC_END
