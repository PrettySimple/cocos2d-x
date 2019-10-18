#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMPSCRIPTEN

#    include <cocos/base/CCConsole.h>
#    include <cocos/platform/CCCommon.h>
#    include <cocos/platform/emscripten/CCStdC-emscripten.h>

NS_CC_BEGIN

void MessageBox(const char* msg, const char* title)
{
    log("%s: %s", title, msg);
}

void LuaLog(const char* format)
{
    puts(format);
}

NS_CC_END

#endif //  CC_TARGET_PLATFORM == CC_PLATFORM_EMPSCRIPTEN
