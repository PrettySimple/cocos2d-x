
#pragma once

#if CC_USE_PHYSICS

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif


#include "scripting/lua-bindings/manual/cocos2d/LuaScriptHandlerMgr.h"

int register_all_cocos2dx_physics_manual(lua_State* tolua_S);

#endif // CC_USE_PHYSICS
