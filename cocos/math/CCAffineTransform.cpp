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

#include "math/CCAffineTransform.h"

#include "math/Vec3.h"

#include <algorithm>
#include <cmath>

using namespace std;

NS_CC_BEGIN

AffineTransform __CCAffineTransformMake(float a, float b, float c, float d, float tx, float ty)
{
    return {a, b, c, d, tx, ty};
}

Vec2 __CCPointApplyAffineTransform(const Vec2& point, const AffineTransform& t)
{
    return Vec2(t.simd.r[0] * point.v[0] + t.simd.r[1] * point.v[1] + t.simd.t);
}

Vec2 PointApplyTransform(const Vec2& point, const Mat4& transform)
{
    Vec3 vec(point.x, point.y, 0);
    transform.transformPoint(vec);
    return Vec2(vec.x, vec.y);
}

Size __CCSizeApplyAffineTransform(const Size& size, const AffineTransform& t)
{
    Size s;
    s.width =
        static_cast<float>(static_cast<double const>(t.a) * static_cast<double>(size.width) + static_cast<double const>(t.c) * static_cast<double>(size.height));
    s.height =
        static_cast<float>(static_cast<double const>(t.b) * static_cast<double>(size.width) + static_cast<double const>(t.d) * static_cast<double>(size.height));
    return s;
}

AffineTransform AffineTransformMakeIdentity()
{
    return {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};
}

const AffineTransform AffineTransform::IDENTITY = {1.f, 0.f, 0.f, 1.f, 0.f, 0.f};

Rect RectApplyAffineTransform(const Rect& rect, const AffineTransform& anAffineTransform)
{
    float top = rect.getMinY();
    float left = rect.getMinX();
    float right = rect.getMaxX();
    float bottom = rect.getMaxY();

    Vec2 topLeft = PointApplyAffineTransform(Vec2(left, top), anAffineTransform);
    Vec2 topRight = PointApplyAffineTransform(Vec2(right, top), anAffineTransform);
    Vec2 bottomLeft = PointApplyAffineTransform(Vec2(left, bottom), anAffineTransform);
    Vec2 bottomRight = PointApplyAffineTransform(Vec2(right, bottom), anAffineTransform);

    float minX = std::min(std::min(topLeft.x, topRight.x), std::min(bottomLeft.x, bottomRight.x));
    float maxX = std::max(std::max(topLeft.x, topRight.x), std::max(bottomLeft.x, bottomRight.x));
    float minY = std::min(std::min(topLeft.y, topRight.y), std::min(bottomLeft.y, bottomRight.y));
    float maxY = std::max(std::max(topLeft.y, topRight.y), std::max(bottomLeft.y, bottomRight.y));

    return Rect(minX, minY, (maxX - minX), (maxY - minY));
}

Rect RectApplyTransform(const Rect& rect, const Mat4& transform)
{
    float top = rect.getMinY();
    float left = rect.getMinX();
    float right = rect.getMaxX();
    float bottom = rect.getMaxY();

    Vec3 topLeft(left, top, 0);
    Vec3 topRight(right, top, 0);
    Vec3 bottomLeft(left, bottom, 0);
    Vec3 bottomRight(right, bottom, 0);
    transform.transformPoint(topLeft);
    transform.transformPoint(topRight);
    transform.transformPoint(bottomLeft);
    transform.transformPoint(bottomRight);

    float minX = std::min(std::min(topLeft.x, topRight.x), std::min(bottomLeft.x, bottomRight.x));
    float maxX = std::max(std::max(topLeft.x, topRight.x), std::max(bottomLeft.x, bottomRight.x));
    float minY = std::min(std::min(topLeft.y, topRight.y), std::min(bottomLeft.y, bottomRight.y));
    float maxY = std::max(std::max(topLeft.y, topRight.y), std::max(bottomLeft.y, bottomRight.y));

    return Rect(minX, minY, (maxX - minX), (maxY - minY));
}

AffineTransform AffineTransformTranslate(const AffineTransform& t, float tx, float ty)
{
    AffineTransform tmp;
    tmp.simd.r[0] = t.simd.r[0];
    tmp.simd.r[1] = t.simd.r[1];
    tmp.simd.t = t.simd.t + t.simd.r[0] * tx + t.simd.r[1] * ty;
    return tmp;
}

AffineTransform AffineTransformScale(const AffineTransform& t, float sx, float sy)
{
    AffineTransform tmp;
    tmp.simd.r[0] = t.simd.r[0] * sx;
    tmp.simd.r[1] = t.simd.r[1] * sy;
    tmp.simd.t = t.simd.t;
    return tmp;
}

AffineTransform AffineTransformRotate(const AffineTransform& t, float anAngle)
{
    AffineTransform tmp;
    float const sine = std::sin(anAngle);
    float const cosine = std::cos(anAngle);
    tmp.simd.r[0] = t.simd.r[0] * cosine + t.simd.r[1] * sine;
    tmp.simd.r[1] = t.simd.r[1] * cosine - t.simd.r[0] * sine;
    tmp.simd.t = t.simd.t;
    return tmp;
}

/* Concatenate `t2' to `t1' and return the result:
     t' = t1 * t2 */
AffineTransform AffineTransformConcat(const AffineTransform& t1, const AffineTransform& t2)
{
    AffineTransform tmp;
    tmp.simd.r[0] = t1.simd.r[0][0] * t2.simd.r[0] + t1.simd.r[0][1] * t2.simd.r[1];
    tmp.simd.r[1] = t1.simd.r[1][0] * t2.simd.r[0] + t1.simd.r[1][1] * t2.simd.r[1];
    tmp.simd.t = t1.simd.t[0] * t2.simd.r[0] + t1.simd.t[1] * t2.simd.r[1] + t2.simd.t;
    return tmp;
}

Mat4 TransformConcat(const Mat4& t1, const Mat4& t2)
{
    return t1 * t2;
}

/* Return true if `t1' and `t2' are equal, false otherwise. */
bool AffineTransformEqualToTransform(const AffineTransform& t1, const AffineTransform& t2)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const ab = t1.simd.r[0] == t2.simd.r[0];
    auto const cd = t1.simd.r[1] == t2.simd.r[1];
    auto const t = t1.simd.t == t2.simd.t;
#pragma clang diagnostic pop
    return ab[0] == -1 && ab[1] == -1 && cd[0] == -1 && cd[1] == -1 && t[0] == -1 && t[1] == -1;
}

AffineTransform AffineTransformInvert(const AffineTransform& t)
{
    AffineTransform tmp;
    auto const inv_determinant_vec = t.simd.r[0] * __builtin_shufflevector(t.simd.r[1], t.simd.r[1], 1, 0);
    float const determinant = 1.f / (inv_determinant_vec[0] - inv_determinant_vec[1]);
    tmp.simd.r[0] = AffineTransform::f32x2_t{t.simd.r[1][1], -t.simd.r[0][1]} * determinant;
    tmp.simd.r[1] = AffineTransform::f32x2_t{-t.simd.r[1][0], t.simd.r[0][0]} * determinant;
    tmp.simd.t = (AffineTransform::f32x2_t{t.simd.r[1][0], t.simd.r[0][1]} * __builtin_shufflevector(t.simd.t, t.simd.t, 1, 0) -
                  AffineTransform::f32x2_t{t.simd.r[1][1], t.simd.r[0][0]} * t.simd.t) *
        determinant;
    return tmp;
}

NS_CC_END
