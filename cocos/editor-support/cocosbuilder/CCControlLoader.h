#ifndef CC_EDITORSUPPORT_COCOSBUILDER_CONTROLLOADER_H
#define CC_EDITORSUPPORT_COCOSBUILDER_CONTROLLOADER_H

#include "CCNodeLoader.h"
#include "extensions/GUI/CCControlExtension/CCControl.h"

namespace cocosbuilder
{
    /* Forward declaration. */
    class CCBReader;

    class CC_DLL ControlLoader : public NodeLoader
    {
    public:
        /**
         * @js NA
         * @lua NA
         */
        virtual ~ControlLoader(){};

    protected:
        CCB_PURE_VIRTUAL_NEW_AUTORELEASE_CREATECCNODE_METHOD(cocos2d::extension::Control);

        virtual void onHandlePropTypeBlockControl(cocos2d::Node* pNode, cocos2d::Node* pParent, const char* pPropertyName, BlockControlData* pBlockControlData,
                                                  CCBReader* ccbReader);
        virtual void onHandlePropTypeCheck(cocos2d::Node* pNode, cocos2d::Node* pParent, const char* pPropertyName, bool pCheck, CCBReader* ccbReader);
    };

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_CONTROLLOADER_H
