#ifndef _CCB_CCMENULOADER_H_
#define _CCB_CCMENULOADER_H_

#include "2d/CCMenu.h"
#include "CCLayerLoader.h"
#include "base/CCRef.h"

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

#endif
