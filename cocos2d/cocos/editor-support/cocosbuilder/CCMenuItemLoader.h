
#pragma once

#include <cocos/2d/CCMenuItem.h>
#include <cocos/base/CCRef.h>

#include "CCLayerLoader.h"

namespace cocosbuilder
{
    /* Forward declaration. */
    class CCBReader;

    class CC_DLL MenuItemLoader : public NodeLoader
    {
    public:
        /**
         * @js NA
         * @lua NA
         */
        virtual ~MenuItemLoader(){};

    protected:
        /**
         * @js NA
         * @lua NA
         */
        CCB_PURE_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(cocos2d::MenuItem);
        /**
         * @js NA
         * @lua NA
         */
        virtual void onHandlePropTypeBlock(cocos2d::Node* pNode, cocos2d::Node* pParent, const char* pPropertyName, BlockData* pBlockData, CCBReader* ccbReader);
        /**
         * @js NA
         * @lua NA
         */
        virtual void onHandlePropTypeCheck(cocos2d::Node* pNode, cocos2d::Node* pParent, const char* pPropertyName, bool pCheck, CCBReader* ccbReader);
    };

} // namespace cocosbuilder
