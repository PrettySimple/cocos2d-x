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

#include <cocos/math/CCAffineTransform.h>

#include <cocos/math/CCGeometry.h>
#include <cocos/math/Mat4.h>
#include <cocos/math/Vec2.h>
#include <cocos/math/Vec3.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <algorithm>
#include <cmath>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
#    include <limits>
#endif

using namespace std;

NS_CC_BEGIN

AffineTransform __CCAffineTransformMake(float a, float b, float c, float d, float tx, float ty)
{
    return {a, b, c, d, tx, ty};
}

Vec2 __CCPointApplyAffineTransform(const Vec2& point, const AffineTransform& t)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return {t.a * point.x + t.c * point.y + t.tx, t.b * point.x + t.d * point.y + t.ty};
#else
    return {t.simd.r[0] * point.v[0] + t.simd.r[1] * point.v[1] + t.simd.t};
#endif
}

Vec2 PointApplyTransform(const Vec2& point, const Mat4& transform)
{
    Vec3 vec(point.x, point.y, 0);
    transform.transformPoint(vec);
    return {vec.x, vec.y};
}

Size __CCSizeApplyAffineTransform(const Size& size, const AffineTransform& t)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return {t.a * size.width + t.c * size.height, t.b * size.width + t.d * size.height};
#else
    return {t.simd.r[0] * size.v[0] + t.simd.r[1] * size.v[1]};
#endif
}

AffineTransform AffineTransformMakeIdentity()
{
    return {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};
}

const AffineTransform AffineTransform::IDENTITY = {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};

Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform)
{
    float const top = rect.getMinY();
    float const left = rect.getMinX();
    float const right = rect.getMaxX();
    float const bottom = rect.getMaxY();

    Vec2 const topLeft = PointApplyAffineTransform({left, top}, anAffineTransform);
    Vec2 const topRight = PointApplyAffineTransform({right, top}, anAffineTransform);
    Vec2 const bottomLeft = PointApplyAffineTransform({left, bottom}, anAffineTransform);
    Vec2 const bottomRight = PointApplyAffineTransform({right, bottom}, anAffineTransform);

    float const minX = std::min(std::min(topLeft.x, topRight.x), std::min(bottomLeft.x, bottomRight.x));
    float const maxX = std::max(std::max(topLeft.x, topRight.x), std::max(bottomLeft.x, bottomRight.x));
    float const minY = std::min(std::min(topLeft.y, topRight.y), std::min(bottomLeft.y, bottomRight.y));
    float const maxY = std::max(std::max(topLeft.y, topRight.y), std::max(bottomLeft.y, bottomRight.y));

    return {minX, minY, (maxX - minX), (maxY - minY)};
}

Rect RectApplyTransform(const Rect& rect, const Mat4& transform)
{
    float const top = rect.getMinY();
    float const left = rect.getMinX();
    float const right = rect.getMaxX();
    float const bottom = rect.getMaxY();

    Vec3 topLeft({left, top, 0.f});
    Vec3 topRight({right, top, 0.f});
    Vec3 bottomLeft({left, bottom, 0.f});
    Vec3 bottomRight({right, bottom, 0.f});
    transform.transformPoint(topLeft);
    transform.transformPoint(topRight);
    transform.transformPoint(bottomLeft);
    transform.transformPoint(bottomRight);

    float const minX = std::min(std::min(topLeft.x, topRight.x), std::min(bottomLeft.x, bottomRight.x));
    float const maxX = std::max(std::max(topLeft.x, topRight.x), std::max(bottomLeft.x, bottomRight.x));
    float const minY = std::min(std::min(topLeft.y, topRight.y), std::min(bottomLeft.y, bottomRight.y));
    float const maxY = std::max(std::max(topLeft.y, topRight.y), std::max(bottomLeft.y, bottomRight.y));

    return {minX, minY, (maxX - minX), (maxY - minY)};
}

AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return __CCAffineTransformMake(t.a, t.b, t.c, t.d, t.tx + t.a * tx + t.c * ty, t.ty + t.b * tx + t.d * ty);
#else
    AffineTransform tmp;
    tmp.simd.r[0] = t.simd.r[0];
    tmp.simd.r[1] = t.simd.r[1];
    tmp.simd.t = t.simd.t + t.simd.r[0] * tx + t.simd.r[1] * ty;
    return tmp;
#endif
}

AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return __CCAffineTransformMake(t.a * sx, t.b * sx, t.c * sy, t.d * sy, t.tx, t.ty);
#else
    AffineTransform tmp;
    tmp.simd.r[0] = t.simd.r[0] * sx;
    tmp.simd.r[1] = t.simd.r[1] * sy;
    tmp.simd.t = t.simd.t;
    return tmp;
#endif
}

AffineTransform AffineTransformRotate(const AffineTransform& t, float anAngle)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    float const sine = sinf(anAngle);
    float const cosine = cosf(anAngle);

    return __CCAffineTransformMake(t.a * cosine + t.c * sine, t.b * cosine + t.d * sine, t.c * cosine - t.a * sine, t.d * cosine - t.b * sine, t.tx, t.ty);
#else
    AffineTransform tmp;
    float const sine = std::sin(anAngle);
    float const cosine = std::cos(anAngle);

    tmp.simd.r[0] = t.simd.r[0] * cosine + t.simd.r[1] * sine;
    tmp.simd.r[1] = t.simd.r[1] * cosine - t.simd.r[0] * sine;
    tmp.simd.t = t.simd.t;
    return tmp;
#endif
}

/* Concatenate `t2' to `t1' and return the result:
     t' = t1 * t2 */
AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return __CCAffineTransformMake(t1.a * t2.a + t1.b * t2.c, t1.a * t2.b + t1.b * t2.d, // a,b
                                   t1.c * t2.a + t1.d * t2.c, t1.c * t2.b + t1.d * t2.d, // c,d
                                   t1.tx * t2.a + t1.ty * t2.c + t2.tx, // tx
                                   t1.tx * t2.b + t1.ty * t2.d + t2.ty); // ty
#else
    AffineTransform tmp;
    tmp.simd.r[0] = t1.simd.r[0][0] * t2.simd.r[0] + t1.simd.r[0][1] * t2.simd.r[1];
    tmp.simd.r[1] = t1.simd.r[1][0] * t2.simd.r[0] + t1.simd.r[1][1] * t2.simd.r[1];
    tmp.simd.t = t1.simd.t[0] * t2.simd.r[0] + t1.simd.t[1] * t2.simd.r[1] + t2.simd.t;
    return tmp;
#endif
}

Mat4 TransformConcat(const Mat4& t1, const Mat4& t2)
{
    return t1 * t2;
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    return std::abs(t1.a - t2.a) < epsi && (t1.b - t2.b) < epsi && std::abs(t1.c - t2.c) < epsi && std::abs(t1.d - t2.d) < epsi &&
        std::abs(t1.tx - t2.tx) < epsi && std::abs(t1.ty - t2.ty) < epsi;
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
    auto const ab = t1.simd.r[0] == t2.simd.r[0];
    auto const cd = t1.simd.r[1] == t2.simd.r[1];
    auto const t = t1.simd.t == t2.simd.t;
#    pragma clang diagnostic pop
    return ab[0] == -1 && ab[1] == -1 && cd[0] == -1 && cd[1] == -1 && t[0] == -1 && t[1] == -1;
#endif
}

AffineTransform AffineTransformInvert(const AffineTransform& t)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    float const determinant = 1 / (t.a * t.d - t.b * t.c);
    return __CCAffineTransformMake(determinant * t.d, -determinant * t.b, -determinant * t.c, determinant * t.a, determinant * (t.c * t.ty - t.d * t.tx),
                                   determinant * (t.b * t.tx - t.a * t.ty));
#else
    AffineTransform tmp;
    auto const inv_determinant_vec = t.simd.r[0] * __builtin_shufflevector(t.simd.r[1], t.simd.r[1], 1, 0);
    float const determinant = 1.f / (inv_determinant_vec[0] - inv_determinant_vec[1]);
    tmp.simd.r[0] = AffineTransform::f32x2_t{t.simd.r[1][1], -t.simd.r[0][1]} * determinant;
    tmp.simd.r[1] = AffineTransform::f32x2_t{-t.simd.r[1][0], t.simd.r[0][0]} * determinant;
    tmp.simd.t = (AffineTransform::f32x2_t{t.simd.r[1][0], t.simd.r[0][1]} * __builtin_shufflevector(t.simd.t, t.simd.t, 1, 0) -
                  AffineTransform::f32x2_t{t.simd.r[1][1], t.simd.r[0][0]} * t.simd.t) *
        determinant;
    return tmp;
#endif
}

NS_CC_END
