//
// EMDebug.h
//
#pragma once

#include <cocos/platform/CCPlatformConfig.h>
#if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

class EmscriptenSticky
{
public:
    EmscriptenSticky(const char* ) noexcept;
    ~EmscriptenSticky() noexcept;
};

#    define EM_STICKY(name) EmscriptenSticky __STICKY_##name(#    name)
#    define EM_STICKY_PRINT(...) printf(__VA_ARGS__)

#else // #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

#    define EM_STICKY(name) \
        do                  \
        {                   \
        } while (0)

#    define EM_STICKY_PRINT(...) \
        do                       \
        {                        \
        } while (0)

#endif // #if (CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN) && defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
