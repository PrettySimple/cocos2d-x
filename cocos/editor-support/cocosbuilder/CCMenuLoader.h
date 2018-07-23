#ifndef CC_EDITORSUPPORT_COCOSBUILDER_MENULOADER_H
#define CC_EDITORSUPPORT_COCOSBUILDER_MENULOADER_H

#include <cocos/2d/CCMenu.h>
#include "CCLayerLoader.h"
#include <cocos/base/CCRef.h>

namespace cocosbuilder
{
    /* Forward declaration. */
    class CCBReader;

    class CC_DLL MenuLoader : public LayerLoader
    {
    public:
        /**
         * @js NA
         * @lua NA
         */
        virtual ~MenuLoader(){};
        /**
         * @js NA
         * @lua NA
         */
        CCB_STATIC_NEW_AUTORELEASE_OBJECT_METHOD(MenuLoader, loader);

    protected:
        virtual cocos2d::Menu* createNode(cocos2d::Node* pParent, cocosbuilder::CCBReader* ccbReader)
        {
            cocos2d::Menu* pMenu = cocos2d::Menu::create();

            pMenu->setContentSize(cocos2d::Size::ZERO);

            return pMenu;
        }
    };

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_MENULOADER_H
