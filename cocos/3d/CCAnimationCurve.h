/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef CC_3D_ANIMATIONCURVE_H
#define CC_3D_ANIMATIONCURVE_H

#include <cocos/base/CCRef.h>
#include <cocos/math/CCMath.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <cmath>
#include <functional>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#    undef NEAR
#endif

NS_CC_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

/**
 * Evaluate Type
 */
enum class EvaluateType
{
    INT_LINEAR,
    INT_NEAR,
    INT_QUAT_SLERP,
    INT_USER_FUNCTION,
};

/**
 * @brief curve of bone's position, rotation or scale
 *
 * @lua NA
 */
template <int componentSize>
class AnimationCurve : public Ref
{
public:
    /**create animation curve*/
    static AnimationCurve* create(float* keytime, float* value, int count);

    /**
     * evaluate value of time
     * @param time Time to be estimated
     * @param dst Estimated value of that time
     * @param type EvaluateType
     */
    void evaluate(float time, float* dst, EvaluateType type) const;

    /**set evaluate function, allow the user use own function*/
    void setEvaluateFun(std::function<void(float time, float* dst)> fun);

    /**get start time*/
    float getStartTime() const;

    /**get end time*/
    float getEndTime() const;

    CC_CONSTRUCTOR_ACCESS :

        AnimationCurve();
    virtual ~AnimationCurve();

    /**
     * Determine index by time.
     */
    int determineIndex(float time) const;

protected:
    float* _value; //
    float* _keytime; // key time(0 - 1), start time _keytime[0], end time _keytime[_count - 1]
    int _count;
    int _componentSizeByte; // component size in byte, position and scale 3 * sizeof(float), rotation 4 * sizeof(float)

    std::function<void(float time, float* dst)> _evaluateFun; // user defined function
};

template <int componentSize>
void AnimationCurve<componentSize>::evaluate(float time, float* dst, EvaluateType type) const
{
    if (_count == 1 || time <= _keytime[0])
    {
        memcpy(dst, _value, _componentSizeByte);
        return;
    }
    else if (time >= _keytime[_count - 1])
    {
        memcpy(dst, &_value[(_count - 1) * componentSize], _componentSizeByte);
        return;
    }

    unsigned int index = determineIndex(time);

    float scale = (_keytime[index + 1] - _keytime[index]);
    float t = (time - _keytime[index]) / scale;

    float* fromValue = &_value[index * componentSize];
    float* toValue = fromValue + componentSize;

    switch (type)
    {
        case EvaluateType::INT_LINEAR:
        {
            for (auto i = 0; i < componentSize; i++)
            {
                dst[i] = fromValue[i] + (toValue[i] - fromValue[i]) * t;
            }
        }
            break;
        case EvaluateType::INT_NEAR:
        {
            float* src = std::abs(t) > 0.5f ? toValue : fromValue;
            memcpy(dst, src, _componentSizeByte);
        }
            break;
        case EvaluateType::INT_QUAT_SLERP:
        {
            // Evaluate.
            Quaternion quat;
            if (t >= 0)
                Quaternion::slerp(Quaternion(fromValue), Quaternion(toValue), t, quat);
            else
                Quaternion::slerp(Quaternion(toValue), Quaternion(fromValue), t, quat);

            dst[0] = quat.x;
            dst[1] = quat.y;
            dst[2] = quat.z;
            dst[3] = quat.w;
        }
            break;
        case EvaluateType::INT_USER_FUNCTION:
        {
            if (_evaluateFun)
                _evaluateFun(time, dst);
        }
            break;
    }
}

template <int componentSize>
void AnimationCurve<componentSize>::setEvaluateFun(std::function<void(float time, float* dst)> fun)
{
    _evaluateFun = fun;
}

// create animation curve
template <int componentSize>
AnimationCurve<componentSize>* AnimationCurve<componentSize>::create(float* keytime, float* value, int count)
{
    int floatSize = sizeof(float);
    AnimationCurve* curve = new (std::nothrow) AnimationCurve();
    curve->_keytime = new float[count];
    memcpy(curve->_keytime, keytime, count * floatSize);

    int compoentSizeByte = componentSize * floatSize;
    int totalByte = count * compoentSizeByte;
    curve->_value = new float[totalByte / floatSize];
    memcpy(curve->_value, value, totalByte);

    curve->_count = count;
    curve->_componentSizeByte = compoentSizeByte;

    curve->autorelease();
    return curve;
}

template <int componentSize>
float AnimationCurve<componentSize>::getStartTime() const
{
    return _keytime[0];
}

template <int componentSize>
float AnimationCurve<componentSize>::getEndTime() const
{
    return _keytime[_count - 1];
}

template <int componentSize>
AnimationCurve<componentSize>::AnimationCurve()
: _value(nullptr)
, _keytime(nullptr)
, _count(0)
, _componentSizeByte(0)
, _evaluateFun(nullptr)
{
}
template <int componentSize>
AnimationCurve<componentSize>::~AnimationCurve()
{
    CC_SAFE_DELETE_ARRAY(_keytime);
    CC_SAFE_DELETE_ARRAY(_value);
}

template <int componentSize>
int AnimationCurve<componentSize>::determineIndex(float time) const
{
    unsigned int min = 0;
    unsigned int max = _count - 1;
    unsigned int mid = 0;

    do
    {
        mid = (min + max) >> 1;

        if (time >= _keytime[mid] && time <= _keytime[mid + 1])
            return mid;
        else if (time < _keytime[mid])
            max = mid - 1;
        else
            min = mid + 1;
    } while (min <= max);

    // We should never hit this!
    return -1;
}

// end of 3d group
/// @}

NS_CC_END

#endif // CC_3D_ANIMATIONCURVE_H
