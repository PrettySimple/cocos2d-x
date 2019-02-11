//
// EMDebug.cpp
//

#if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)

#    include <stdio.h>
#    include "EMDebug.h"

EmscriptenSticky::EmscriptenSticky(const char* name) noexcept
{
    printf("\n[STICKY]:%s\n", name);
}

EmscriptenSticky::~EmscriptenSticky() noexcept
{
    printf("\n[/STICKY]\n");
}


#endif // #if defined(COCOS2D_DEBUG) && (COCOS2D_DEBUG > 0)
