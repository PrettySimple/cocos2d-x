/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#pragma once

#include <cocos/math/CCGeometry.h>
#include <cocos/math/Mat4.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

/**@{
 Affine transform
 a   b    0
 c   d    0
 tx  ty   1

 Identity
 1   0    0
 0   1    0
 0   0    1
 */

struct CC_DLL AffineTransform final
{
#ifdef __ARM_NEON
    using f32x2_t = __attribute__((neon_vector_type(2))) float;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    using f32x2_t = float[2];
#else
    using f32x2_t = __attribute__((ext_vector_type(2))) float;
#endif
    using f32x2x2_t = f32x2_t[2];

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
    union
    {
        struct
        {
            f32x2x2_t r;
            f32x2_t t;
        } simd = {{{0.f, 0.f}, {0.f, 0.f}}, {0.f, 0.f}};
        struct
        {
            float a, b, c, d;
            float tx, ty;
        };
    };
#pragma clang diagnostic pop

    AffineTransform() = default;
    AffineTransform(AffineTransform const&) = default;
    AffineTransform& operator=(AffineTransform const&) = default;
    AffineTransform(AffineTransform&&) noexcept = default;
    AffineTransform& operator=(AffineTransform&&) noexcept = default;
    ~AffineTransform() = default;

    constexpr AffineTransform(float aa, float bb, float cc, float dd, float txx, float tyy)
    : simd{{{aa, bb}, {cc, dd}}, {txx, tyy}}
    {
    }

    static const AffineTransform IDENTITY;
};

/**@}*/

/**Make affine transform.*/
CC_DLL AffineTransform __CCAffineTransformMake(float a, float b, float c, float d, float tx, float ty);
#define AffineTransformMake __CCAffineTransformMake

/**Multiply point (x,y,1) by a  affine transform.*/
CC_DLL Vec2 __CCPointApplyAffineTransform(const Vec2& point, const AffineTransform& t);
#define PointApplyAffineTransform __CCPointApplyAffineTransform

/**Multiply size (width,height,0) by a  affine transform.*/
CC_DLL Size __CCSizeApplyAffineTransform(const Size& size, const AffineTransform& t);
#define SizeApplyAffineTransform __CCSizeApplyAffineTransform
/**Make identity affine transform.*/
CC_DLL AffineTransform AffineTransformMakeIdentity();
/**Transform Rect, which will transform the four vertices of the point.*/
CC_DLL Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform);
/**@{
 Transform vec2 and Rect by Mat4.
 */
CC_DLL Rect RectApplyTransform(const Rect& rect, const Mat4& transform);
CC_DLL Vec2 PointApplyTransform(const Vec2& point, const Mat4& transform);
/**@}*/
/**
 Translation, equals
 1  0  1
 0  1  0   * affine transform
 tx ty 1
 */
CC_DLL AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty);
/**
 Rotation, equals
 cos(angle)   sin(angle)   0
 -sin(angle)  cos(angle)   0  * AffineTransform
 0            0            1
 */
CC_DLL AffineTransform AffineTransformRotate(const AffineTransform& aTransform, float anAngle);
/**
 Scale, equals
 sx   0   0
 0    sy  0  * affineTransform
 0    0   1
 */
CC_DLL AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy);
/**Concat two affine transform, t1 * t2*/
CC_DLL AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2);
/**Compare affine transform.*/
CC_DLL bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2);
/**Get the inverse of affine transform.*/
CC_DLL AffineTransform AffineTransformInvert(const AffineTransform& t);
/**Concat Mat4, return t1 * t2.*/
CC_DLL Mat4 TransformConcat(const Mat4& t1, const Mat4& t2);

NS_CC_END

// end of base transform
/// @}
