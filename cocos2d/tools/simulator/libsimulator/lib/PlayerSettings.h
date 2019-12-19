
#pragma once

#include "PlayerMacros.h"

PLAYER_NS_BEGIN

class PlayerSettings
{
public:
    PlayerSettings()
        : openLastProject(false)
        , offsetX(0)
        , offsetY(0)
    {}

    bool openLastProject;
    int offsetX;
    int offsetY;
};

PLAYER_NS_END
