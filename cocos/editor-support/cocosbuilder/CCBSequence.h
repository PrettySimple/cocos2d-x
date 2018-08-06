#ifndef CC_EDITORSUPPORT_COCOSBUILDER_SEQUENCE_H
#define CC_EDITORSUPPORT_COCOSBUILDER_SEQUENCE_H

#include "CCBSequenceProperty.h"
#include <cocos/base/CCRef.h>

#include <chrono>
#include <string>

namespace cocosbuilder
{
    class CC_DLL CCBSequence : public cocos2d::Ref
    {
    public:
        CCBSequence();
        /**
         * @js NA
         * @lua NA
         */
        ~CCBSequence();
        std::chrono::milliseconds getDuration();
        void setDuration(std::chrono::milliseconds fDuration);

        CCBSequenceProperty* getCallbackChannel();
        void setCallbackChannel(CCBSequenceProperty* callbackChannel);

        CCBSequenceProperty* getSoundChannel();
        void setSoundChannel(CCBSequenceProperty* soundChannel);

        const char* getName();
        void setName(const char* pName);

        int getSequenceId();
        void setSequenceId(int nSequenceId);

        int getChainedSequenceId();
        void setChainedSequenceId(int nChainedSequenceId);

    private:
        std::chrono::milliseconds _duration;
        std::string _name;
        int mSequenceId;
        int mChainedSequenceId;
        CCBSequenceProperty* mCallbackChannel;
        CCBSequenceProperty* mSoundChannel;
    };

} // namespace cocosbuilder

#endif // CC_EDITORSUPPORT_COCOSBUILDER_SEQUENCE_H
