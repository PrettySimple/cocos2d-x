//
//  CCApplicationProtocol.cpp
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 09/07/2018.
//

#include "CCApplicationProtocol.h"

#include "base/CCAutoreleasePool.h"
#include "renderer/ccGLStateCache.h"

NS_CC_BEGIN

ApplicationProtocol::ApplicationProtocol()
{
    GL::initialize(); // Make sure that static cache variables are created first to be deleted last.
}

ApplicationProtocol::~ApplicationProtocol()
{
#if CC_ENABLE_SCRIPT_BINDING
    ScriptEngineManager::destroyInstance();
#endif
    /** clean auto release pool. */
    PoolManager::destroyInstance();
}

NS_CC_END
