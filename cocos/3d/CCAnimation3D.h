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

#ifndef CC_3D_ANIMATION3D_H
#define CC_3D_ANIMATION3D_H

#include <cocos/3d/CCAnimationCurve.h>
#include <cocos/3d/CCBundle3DData.h>
#include <cocos/base/CCRef.h>
#include <cocos/base/ccMacros.h>

#include <chrono>
#include <unordered_map>

NS_CC_BEGIN
/**
 * @addtogroup _3d
 * @{
 */

/**
 * @brief static animation data, shared
 */
class CC_DLL Animation3D : public Ref
{
    friend class Bundle3D;

public:
    /**
     * animation curve, translation, rotation, and scale
     */
    class Curve
    {
    public:
        typedef AnimationCurve<3> AnimationCurveVec3;
        typedef AnimationCurve<4> AnimationCurveQuat;
        /**translation curve*/
        AnimationCurveVec3* translateCurve;
        /**rotation curve*/
        AnimationCurveQuat* rotCurve;
        /**scaling curve*/
        AnimationCurveVec3* scaleCurve;
        /**constructor */
        Curve();
        /**constructor */
        ~Curve();
    };

    /**read all animation or only the animation with given animationName? animationName == "" read the first.*/
    static Animation3D* create(const std::string& filename, const std::string& animationName = "");

    /**the cache method to create or get an Animation3D object*/
    CC_DEPRECATED_ATTRIBUTE static Animation3D* getOrCreate(const std::string& filename, const std::string& animationName = "")
    {
        return create(filename, animationName);
    }

    /**get duration*/
    inline std::chrono::milliseconds getDuration() const noexcept { return _duration; }

    /**
     * get bone curve
     *
     * @lua NA
     */
    Curve* getBoneCurveByName(const std::string& name) const;

    /**get the bone Curves set*/
    inline std::unordered_map<std::string, Curve*> const& getBoneCurves() const { return _boneCurves; }

    CC_CONSTRUCTOR_ACCESS : Animation3D();
    virtual ~Animation3D();
    /**init Animation3D from bundle data*/
    bool init(const Animation3DData& data);

    /**init Animation3D with file name and animation name*/
    bool initWithFile(const std::string& filename, const std::string& animationName);

protected:
    std::unordered_map<std::string, Curve*> _boneCurves; // bone curves map, key bone name, value AnimationCurve

    std::chrono::milliseconds _duration; // animation duration
};

/**
 * Animation3D Cache
 */
class Animation3DCache
{
public:
    /**get and destroy instance*/
    static Animation3DCache* getInstance();
    static void destroyInstance();

    /**get animation by key*/
    Animation3D* getAnimation(const std::string& key);

    /**add animation to cache*/
    void addAnimation(const std::string& key, Animation3D* animation);

    /**remove all animation*/
    void removeAllAnimations();
    /**remove unused animation*/
    void removeUnusedAnimation();

protected:
    Animation3DCache();
    ~Animation3DCache();

    static Animation3DCache* _cacheInstance; // cache instance

    std::unordered_map<std::string, Animation3D*> _animations; // cached animations
};

// end of 3d group
/// @}
NS_CC_END

#endif // CC_3D_ANIMATION3D_H
