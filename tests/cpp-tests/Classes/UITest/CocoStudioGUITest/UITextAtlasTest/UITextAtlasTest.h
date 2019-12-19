

#pragma once

#include "../UIScene.h"

DEFINE_TEST_SUITE(UITextAtlasTests);

class UITextAtlasTest : public UIScene
{
public:
    CREATE_FUNC(UITextAtlasTest);

    virtual bool init() override;
    void printWidgetResources(cocos2d::Ref* sender);
protected:
    cocos2d::ui::TextAtlas* _textAtlas;
};
