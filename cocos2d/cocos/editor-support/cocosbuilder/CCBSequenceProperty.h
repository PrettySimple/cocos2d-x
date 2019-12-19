
#pragma once

#include "CCBKeyframe.h"
#include <cocos/base/CCRef.h>
#include <cocos/base/CCVector.h>

namespace cocosbuilder
{
    class CC_DLL CCBSequenceProperty : public cocos2d::Ref
    {
    public:
        /**
         * @js ctor
         */
        CCBSequenceProperty();
        /**
         * @js NA
         * @lua NA
         */
        ~CCBSequenceProperty();

        virtual bool init();

        const char* getName();
        void setName(const char* pName);

        int getType();
        void setType(int type);

        cocos2d::Vector<CCBKeyframe*>& getKeyframes();

    private:
        std::string _name;
        int _type;
        cocos2d::Vector<CCBKeyframe*> _keyframes;
    };

} // namespace cocosbuilder
