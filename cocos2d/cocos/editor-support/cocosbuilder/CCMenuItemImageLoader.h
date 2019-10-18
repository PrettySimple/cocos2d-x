#ifndef CC_EDITORSUPPORT_COCOSBUILDER_MENUITEMIMAGELOADER_H
#define CC_EDITORSUPPORT_COCOSBUILDER_MENUITEMIMAGELOADER_H

#include "CCMenuItemLoader.h"

namespace cocosbuilder
{
    /* Forward declaration. */
    class CCBReader;

    class CC_DLL MenuItemImageLoader : public MenuItemLoader
    {
    public:
        /**
         * @js NA
         * @lua NA
         */
        virtual ~MenuItemImageLoader(){};
        /**
         * @js NA
         * @lua NA
         */
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MenuItemImageLoader, loader);

    protected:
        /**
         * @js NA
         * @lua NA
         */
        CCB_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(cocos2d::MenuItemImage);
        /**
         * @js NA
         * @lua NA
         */
        virtual void onHandlePropTypeSpriteFrame(cocos2d::Node* pNode, cocos2d::Node* pParent, const char* pPropertyName, cocos2d::SpriteFrame* pSpriteFrame,
                                                 CCBReader* ccbReader);
    };

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_MENUITEMIMAGELOADER_H
