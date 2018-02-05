//
//  CCActionManagerOperation.h
//  cocos2d_libs
//
//  Created by Mathieu Garaud on 02/02/2018.
//

#ifndef ACTION_CCACTION_MANAGEROPERATION_H
#define ACTION_CCACTION_MANAGEROPERATION_H

#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

enum struct ActionManagerOperation : std::uint8_t
{
    UNKNOWN = 0,
    PAUSE_TARGET,
    RESUME_TARGET,
    PAUSE_ALL_TARGETS,
    ADD_ACTION,
    REMOVE_ALL_ACTIONS,
    REMOVE_ALL_ACTIONS_FROM_TARGET,
    REMOVE_ACTION,
    REMOVE_ACTION_BY_TAG,
    REMOVE_ALL_ACTIONS_BY_TAG,
    REMOVE_ALL_ACTIONS_BY_FLAG,
    STEP_ACTION
};

NS_CC_END

#endif // ACTION_CCACTION_MANAGEROPERATION_H
