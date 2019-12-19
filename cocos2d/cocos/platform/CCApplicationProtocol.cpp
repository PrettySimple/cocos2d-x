//
//  CCApplicationProtocol.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 09/07/2018.
//

#include "CCApplicationProtocol.h"

#include <cocos/base/CCAutoreleasePool.h>
#include <cocos/platform/CCPlatformMacros.h>

NS_CC_BEGIN

ApplicationProtocol::~ApplicationProtocol()
{
    /** clean auto release pool. */
    PoolManager::destroyInstance();
}

NS_CC_END
