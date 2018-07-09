/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.

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

#include "2d/CCTweenFunction.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#define _USE_MATH_DEFINES // needed for M_PI and M_PI2
#include <cmath> // M_PI
#undef _USE_MATH_DEFINES
#pragma clang diagnostic pop

#include <limits>

NS_CC_BEGIN

namespace tweenfunc
{
#ifndef M_PI_X_2
#    define M_PI_X_2 static_cast<float>(M_PI) * 2.0f
#endif

    float tweenTo(float time, TweenType type, float* easingParam)
    {
        float delta = 0.0f;

        switch (type)
        {
            case CUSTOM_EASING:
                delta = customEase(time, easingParam);
                break;

            case Linear:
                delta = linear(time);
                break;

            case Sine_EaseIn:
                delta = sineEaseIn(time);
                break;
            case Sine_EaseOut:
                delta = sineEaseOut(time);
                break;
            case Sine_EaseInOut:
                delta = sineEaseInOut(time);
                break;

            case Quad_EaseIn:
                delta = quadEaseIn(time);
                break;
            case Quad_EaseOut:
                delta = quadEaseOut(time);
                break;
            case Quad_EaseInOut:
                delta = quadEaseInOut(time);
                break;

            case Cubic_EaseIn:
                delta = cubicEaseIn(time);
                break;
            case Cubic_EaseOut:
                delta = cubicEaseOut(time);
                break;
            case Cubic_EaseInOut:
                delta = cubicEaseInOut(time);
                break;

            case Quart_EaseIn:
                delta = quartEaseIn(time);
                break;
            case Quart_EaseOut:
                delta = quartEaseOut(time);
                break;
            case Quart_EaseInOut:
                delta = quartEaseInOut(time);
                break;

            case Quint_EaseIn:
                delta = quintEaseIn(time);
                break;
            case Quint_EaseOut:
                delta = quintEaseOut(time);
                break;
            case Quint_EaseInOut:
                delta = quintEaseInOut(time);
                break;

            case Expo_EaseIn:
                delta = expoEaseIn(time);
                break;
            case Expo_EaseOut:
                delta = expoEaseOut(time);
                break;
            case Expo_EaseInOut:
                delta = expoEaseInOut(time);
                break;

            case Circ_EaseIn:
                delta = circEaseIn(time);
                break;
            case Circ_EaseOut:
                delta = circEaseOut(time);
                break;
            case Circ_EaseInOut:
                delta = circEaseInOut(time);
                break;

            case Elastic_EaseIn:
            {
                float period = 0.3f;
                if (easingParam != nullptr)
                {
                    period = easingParam[0];
                }
                delta = elasticEaseIn(time, period);
            }
            break;
            case Elastic_EaseOut:
            {
                float period = 0.3f;
                if (easingParam != nullptr)
                {
                    period = easingParam[0];
                }
                delta = elasticEaseOut(time, period);
            }
            break;
            case Elastic_EaseInOut:
            {
                float period = 0.3f;
                if (easingParam != nullptr)
                {
                    period = easingParam[0];
                }
                delta = elasticEaseInOut(time, period);
            }
            break;

            case Back_EaseIn:
                delta = backEaseIn(time);
                break;
            case Back_EaseOut:
                delta = backEaseOut(time);
                break;
            case Back_EaseInOut:
                delta = backEaseInOut(time);
                break;

            case Bounce_EaseIn:
                delta = bounceEaseIn(time);
                break;
            case Bounce_EaseOut:
                delta = bounceEaseOut(time);
                break;
            case Bounce_EaseInOut:
                delta = bounceEaseInOut(time);
                break;

            case TWEEN_EASING_MAX:
                break;

            default:
                delta = sineEaseInOut(time);
                break;
        }

        return delta;
    }

    // Linear
    float linear(float time) { return time; }

    // Sine Ease
    float sineEaseIn(float time) { return -1.0f * std::cos(time * static_cast<float>(M_PI_2)) + 1.0f; }

    float sineEaseOut(float time) { return std::sin(time * static_cast<float>(M_PI_2)); }

    float sineEaseInOut(float time) { return -0.5f * (std::cos(static_cast<float>(M_PI) * time) - 1.0f); }

    // Quad Ease
    float quadEaseIn(float time) { return time * time; }

    float quadEaseOut(float time) { return -1.0f * time * (time - 2.0f); }

    float quadEaseInOut(float time)
    {
        time = time * 2.0f;
        if (time < 1.0f)
            return 0.5f * time * time;
        time -= 1.0f;
        return -0.5f * (time * (time - 2.0f) - 1.0f);
    }

    // Cubic Ease
    float cubicEaseIn(float time) { return time * time * time; }
    float cubicEaseOut(float time)
    {
        time -= 1.0f;
        return (time * time * time) + 1.0f;
    }
    float cubicEaseInOut(float time)
    {
        time = time * 2.0f;
        if (time < 1.0f)
            return 0.5f * time * time * time;
        time -= 2.0f;
        return 0.5f * (time * time * time + 2.0f);
    }

    // Quart Ease
    float quartEaseIn(float time) { return time * time * time * time; }

    float quartEaseOut(float time)
    {
        time -= 1.0f;
        return -(time * time * time * time - 1.0f);
    }

    float quartEaseInOut(float time)
    {
        time = time * 2.0f;
        if (time < 1.0f)
            return 0.5f * time * time * time * time;
        time -= 2.0f;
        return -0.5f * (time * time * time * time - 2.0f);
    }

    // Quint Ease
    float quintEaseIn(float time) { return time * time * time * time * time; }

    float quintEaseOut(float time)
    {
        time -= 1.0f;
        return (time * time * time * time * time + 1.0f);
    }

    float quintEaseInOut(float time)
    {
        time = time * 2.0f;
        if (time < 1.0f)
            return 0.5f * time * time * time * time * time;
        time -= 2.0f;
        return 0.5f * (time * time * time * time * time + 2.0f);
    }

    // Expo Ease
    float expoEaseIn(float time) { return std::abs(time) < std::numeric_limits<float>::epsilon() ? 0.0f : pow(2.0f, 10.0f * (time - 1.0f)) - 1.0f * 0.001f; }
    float expoEaseOut(float time) { return std::abs(time - 1.f) < std::numeric_limits<float>::epsilon() ? 1.0f : (-std::pow(2.0f, -10.0f * time) + 1.0f); }
    float expoEaseInOut(float time)
    {
        time /= 0.5f;
        if (time < 1.0f)
        {
            time = 0.5f * std::pow(2.0f, 10.0f * (time - 1.0f));
        }
        else
        {
            time = 0.5f * (-std::pow(2.0f, -10.0f * (time - 1.0f)) + 2.0f);
        }

        return time;
    }

    // Circ Ease
    float circEaseIn(float time) { return -1.0f * (std::sqrt(1.0f - time * time) - 1.0f); }
    float circEaseOut(float time)
    {
        time = time - 1.0f;
        return std::sqrt(1.0f - time * time);
    }
    float circEaseInOut(float time)
    {
        time = time * 2.0f;
        if (time < 1.0f)
            return -0.5f * (std::sqrt(1.0f - time * time) - 1.0f);
        time -= 2.0f;
        return 0.5f * (std::sqrt(1.0f - time * time) + 1.0f);
    }

    // Elastic Ease
    float elasticEaseIn(float time, float period)
    {
        float newT = 0.0f;
        if (std::abs(time) < std::numeric_limits<float>::epsilon() || std::abs(time - 1.0f) < std::numeric_limits<float>::epsilon())
        {
            newT = time;
        }
        else
        {
            float s = period * 0.25f;
            time = time - 1.0f;
            newT = -std::pow(2.0f, 10.0f * time) * std::sin((time - s) * M_PI_X_2 / period);
        }
        return newT;
    }
    float elasticEaseOut(float time, float period)
    {
        float newT = 0.0f;
        if (std::abs(time) < std::numeric_limits<float>::epsilon() || std::abs(time - 1.0f) < std::numeric_limits<float>::epsilon())
        {
            newT = time;
        }
        else
        {
            float s = period * 0.25f;
            newT = std::pow(2.0f, -10.0f * time) * std::sin((time - s) * M_PI_X_2 / period) + 1.0f;
        }

        return newT;
    }
    float elasticEaseInOut(float time, float period)
    {
        float newT = 0.0f;
        if (std::abs(time) < std::numeric_limits<float>::epsilon() || std::abs(time - 1.f) < std::numeric_limits<float>::epsilon())
        {
            newT = time;
        }
        else
        {
            time = time * 2.0f;
            if (std::abs(period) < std::numeric_limits<float>::epsilon())
            {
                period = 0.3f * 1.5f;
            }

            float s = period * 0.25f;

            time = time - 1.0f;
            if (time < 0.0f)
            {
                newT = -0.5f * std::pow(2.0f, 10.0f * time) * std::sin((time - s) * M_PI_X_2 / period);
            }
            else
            {
                newT = std::pow(2.0f, -10.0f * time) * std::sin((time - s) * M_PI_X_2 / period) * 0.5f + 1.0f;
            }
        }
        return newT;
    }

    // Back Ease
    float backEaseIn(float time)
    {
        float overshoot = 1.70158f;
        return time * time * ((overshoot + 1.0f) * time - overshoot);
    }
    float backEaseOut(float time)
    {
        float overshoot = 1.70158f;

        time = time - 1.0f;
        return time * time * ((overshoot + 1.0f) * time + overshoot) + 1.0f;
    }
    float backEaseInOut(float time)
    {
        float overshoot = 1.70158f * 1.525f;

        time = time * 2.0f;
        if (time < 1.0f)
        {
            return (time * time * ((overshoot + 1.0f) * time - overshoot)) * 0.5f;
        }
        else
        {
            time = time - 2.0f;
            return (time * time * ((overshoot + 1.0f) * time + overshoot)) * 0.5f + 1.0f;
        }
    }

    // Bounce Ease
    float bounceTime(float time)
    {
        if (time < 1.0f / 2.75f)
        {
            return 7.5625f * time * time;
        }
        else if (time < 2.0f / 2.75f)
        {
            time -= 1.5f / 2.75f;
            return 7.5625f * time * time + 0.75f;
        }
        else if (time < 2.5f / 2.75f)
        {
            time -= 2.25f / 2.75f;
            return 7.5625f * time * time + 0.9375f;
        }

        time -= 2.625f / 2.75f;
        return 7.5625f * time * time + 0.984375f;
    }
    float bounceEaseIn(float time) { return 1.0f - bounceTime(1.0f - time); }

    float bounceEaseOut(float time) { return bounceTime(time); }

    float bounceEaseInOut(float time)
    {
        float newT = 0.0f;
        if (time < 0.5f)
        {
            time = time * 2.0f;
            newT = (1.0f - bounceTime(1.0f - time)) * 0.5f;
        }
        else
        {
            newT = bounceTime(time * 2.0f - 1.0f) * 0.5f + 0.5f;
        }

        return newT;
    }

    // Custom Ease
    float customEase(float time, float* easingParam)
    {
        if (easingParam != nullptr)
        {
            float tt = 1.0f - time;
            return easingParam[1] * tt * tt * tt + 3.0f * easingParam[3] * time * tt * tt + 3.0f * easingParam[5] * time * time * tt +
                easingParam[7] * time * time * time;
        }
        return time;
    }

    float easeIn(float time, float rate) { return std::pow(time, rate); }

    float easeOut(float time, float rate) { return std::pow(time, 1.0f / rate); }

    float easeInOut(float time, float rate)
    {
        time *= 2.0f;
        if (time < 1.0f)
        {
            return 0.5f * std::pow(time, rate);
        }
        else
        {
            return (1.0f - 0.5f * std::pow(2.f - time, rate));
        }
    }

    float quadraticIn(float time) { return std::pow(time, 2.0f); }

    float quadraticOut(float time) { return -time * (time - 2.0f); }

    float quadraticInOut(float time)
    {
        float resultTime = time;
        time = time * 2.0f;
        if (time < 1.0f)
        {
            resultTime = time * time * 0.5f;
        }
        else
        {
            --time;
            resultTime = -0.5f * (time * (time - 2.0f) - 1.0f);
        }
        return resultTime;
    }

    float bezieratFunction(float a, float b, float c, float d, float t)
    {
        return (std::pow(1.0f - t, 3.0f) * a + 3.0f * t * (std::pow(1.0f - t, 2.0f)) * b + 3.0f * std::pow(t, 2.0f) * (1.0f - t) * c + std::pow(t, 3.0f) * d);
    }

} // namespace tweenfunc

NS_CC_END
