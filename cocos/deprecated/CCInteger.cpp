//
//  CCInteger.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 20/06/2018.
//

#include "CCInteger.h"

#include "base/CCDataVisitor.h"

#include <new>

NS_CC_BEGIN

__Integer::~__Integer()
{
}

__Integer* __Integer::create(int v)
{
    __Integer* pRet = new (std::nothrow) __Integer(v);
    if (pRet != nullptr)
    {
        pRet->autorelease();
    }
    return pRet;
}

void __Integer::acceptVisitor(DataVisitor& visitor)
{
    visitor.visit(this);
}

__Integer* __Integer::clone() const
{
    return __Integer::create(_value);
}

NS_CC_END
