#ifndef CC_EDITORSUPPORT_COCOSBUILDER_NODELOADERLISTENER_H
#define CC_EDITORSUPPORT_COCOSBUILDER_NODELOADERLISTENER_H

namespace cocosbuilder
{
    class CC_DLL NodeLoaderListener
    {
    public:
        /**
         * @js NA
         * @lua NA
         */
        virtual ~NodeLoaderListener(){};
        /**
         * @js NA
         * @lua NA
         */
        virtual void onNodeLoaded(cocos2d::Node* pNode, NodeLoader* pNodeLoader) = 0;
    };

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_NODELOADERLISTENER_H
