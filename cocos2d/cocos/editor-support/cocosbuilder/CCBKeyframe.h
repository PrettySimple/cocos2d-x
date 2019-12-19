
#pragma once

#include <cocos/base/CCRef.h>
#include <cocos/base/CCValue.h>

#include <chrono>

namespace cocosbuilder
{
    class CC_DLL CCBKeyframe : public cocos2d::Ref
    {
    public:
        enum class EasingType
        {
            INSTANT,

            LINEAR,

            CUBIC_IN,
            CUBIC_OUT,
            CUBIC_INOUT,

            ELASTIC_IN,
            ELASTIC_OUT,
            ELASTIC_INOUT,

            BOUNCE_IN,
            BOUNCE_OUT,
            BOUNCE_INOUT,

            BACK_IN,
            BACK_OUT,
            BACK_INOUT,
        };
        /**
         * @js ctor
         */
        CCBKeyframe();
        /**
         * @js NA
         * @lua NA
         */
        ~CCBKeyframe();

        const cocos2d::Value& getValue() const;
        void setValue(const cocos2d::Value& value);

        cocos2d::Ref* getObject() const;
        void setObject(cocos2d::Ref* obj);

        std::chrono::milliseconds getTime();
        void setTime(std::chrono::milliseconds fTime);

        EasingType getEasingType();
        void setEasingType(EasingType easingType);

        float getEasingOpt();
        void setEasingOpt(float fEasingOpt);

    private:
        cocos2d::Value _value;
        cocos2d::Ref* _object;
        std::chrono::milliseconds _time;
        EasingType _easingType;
        float _easingOpt;
    };

} // namespace cocosbuilder
