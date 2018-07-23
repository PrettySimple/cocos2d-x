/**
 Copyright 2013 BlackBerry Inc.
 Copyright (c) 2014-2015 Chukong Technologies

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 Original file from GamePlay3D: http://gameplay3d.org

 This file was modified to fit the cocos2d-x project
 */

#ifndef CC_MATH_MATHUTIL_H
#define CC_MATH_MATHUTIL_H

#include <cocos/math/CCMathBase.h>
#include <cocos/math/Mat4.h>
#include <cocos/math/Vec3.h>
#include <cocos/math/Vec4.h>

#include <cmath>
#include <limits>

NS_CC_MATH_BEGIN

/**
 * Updates the given scalar towards the given target using a smoothing function.
 * The given response time determines the amount of smoothing (lag). A longer
 * response time yields a smoother result and more lag. To force the scalar to
 * follow the target closely, provide a response time that is very small relative
 * to the given elapsed time.
 *
 * @param x the scalar to update.
 * @param target target value.
 * @param elapsedTime elapsed time between calls.
 * @param responseTime response time (in the same units as elapsedTime).
 */
inline void smooth(float& x, float target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0.f)
    {
        x += (target - x) * elapsedTime / (elapsedTime + responseTime);
    }
}

/**
 * Updates the given scalar towards the given target using a smoothing function.
 * The given rise and fall times determine the amount of smoothing (lag). Longer
 * rise and fall times yield a smoother result and more lag. To force the scalar to
 * follow the target closely, provide rise and fall times that are very small relative
 * to the given elapsed time.
 *
 * @param x the scalar to update.
 * @param target target value.
 * @param elapsedTime elapsed time between calls.
 * @param riseTime response time for rising slope (in the same units as elapsedTime).
 * @param fallTime response time for falling slope (in the same units as elapsedTime).
 */
inline void smooth(float& x, float target, float elapsedTime, float riseTime, float fallTime)
{
    if (elapsedTime > 0.f)
    {
        float const delta = target - x;
        x += delta * elapsedTime / (elapsedTime + (delta > 0 ? riseTime : fallTime));
    }
}

/**
 * Linearly interpolates between from value to to value by alpha which is in
 * the range [0,1]
 *
 * @param from the from value.
 * @param to the to value.
 * @param alpha the alpha value between [0,1]
 *
 * @return interpolated float value
 */
inline float lerp(float from, float to, float alpha)
{
    return from * (1.0f - alpha) + to * alpha;
}

inline void addMatrix(Mat4 const& m, float scalar, Mat4& dst)
{
    dst.col[0] = m.col[0] + scalar;
    dst.col[1] = m.col[1] + scalar;
    dst.col[2] = m.col[2] + scalar;
    dst.col[3] = m.col[3] + scalar;
}

inline void addMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
    dst.col[0] = m1.col[0] + m2.col[0];
    dst.col[1] = m1.col[1] + m2.col[1];
    dst.col[2] = m1.col[2] + m2.col[2];
    dst.col[3] = m1.col[3] + m2.col[3];
}

inline void subtractMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
    dst.col[0] = m1.col[0] - m2.col[0];
    dst.col[1] = m1.col[1] - m2.col[1];
    dst.col[2] = m1.col[2] - m2.col[2];
    dst.col[3] = m1.col[3] - m2.col[3];
}

inline void multiplyMatrix(Mat4 const& m, float scalar, Mat4& dst)
{
    dst.col[0] = m.col[0] * scalar;
    dst.col[1] = m.col[1] * scalar;
    dst.col[2] = m.col[2] * scalar;
    dst.col[3] = m.col[3] * scalar;
}

inline void multiplyMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
    // Support the case where m1 or m2 is the same matrix as dst.
    Mat4 tmp;

    tmp.col[0] = m1.col[0] * m2.col[0][0] + m1.col[1] * m2.col[0][1] + m1.col[2] * m2.col[0][2] + m1.col[3] * m2.col[0][3];
    tmp.col[1] = m1.col[0] * m2.col[1][0] + m1.col[1] * m2.col[1][1] + m1.col[2] * m2.col[1][2] + m1.col[3] * m2.col[1][3];
    tmp.col[2] = m1.col[0] * m2.col[2][0] + m1.col[1] * m2.col[2][1] + m1.col[2] * m2.col[2][2] + m1.col[3] * m2.col[2][3];
    tmp.col[3] = m1.col[0] * m2.col[3][0] + m1.col[1] * m2.col[3][1] + m1.col[2] * m2.col[3][2] + m1.col[3] * m2.col[3][3];

    dst = tmp;
}

inline void negateMatrix(Mat4 const& m, Mat4& dst)
{
    dst.col[0] = -m.col[0];
    dst.col[1] = -m.col[1];
    dst.col[2] = -m.col[2];
    dst.col[3] = -m.col[3];
}

inline void transposeMatrix(Mat4 const& other, Mat4& dst)
{
    dst.col[0] = {other.m[0], other.m[4], other.m[8], other.m[12]};
    dst.col[1] = {other.m[1], other.m[5], other.m[9], other.m[13]};
    dst.col[2] = {other.m[2], other.m[6], other.m[10], other.m[14]};
    dst.col[3] = {other.m[3], other.m[7], other.m[11], other.m[15]};
}

inline void transformVec4(Mat4 const& m, float x, float y, float z, float w, Vec3& dst)
{
    auto const tmp = x * m.col[0] + y * m.col[1] + z * m.col[2] + w * m.col[3];
    dst.x = tmp[0];
    dst.y = tmp[1];
    dst.z = tmp[2];
}

inline void transformVec4(Mat4 const& m, Vec4 const& other, Vec4& dst)
{
    dst.v = other.v[0] * m.col[0] + other.v[1] * m.col[1] + other.v[2] * m.col[2] + other.v[3] * m.col[3];
}

static void crossVec3(Vec3 const& v1, Vec3 const& v2, Vec3& dst)
{
    auto const d1 = Vec4::f32x4_t{v1.x, v1.y, v1.z, 0.f};
    auto const d2 = Vec4::f32x4_t{v2.x, v2.y, v2.z, 0.f};
    auto const tmp = (__builtin_shufflevector(d1, d1, 1, 2, 0, 3) * __builtin_shufflevector(d2, d2, 2, 0, 1, 3)) -
        (__builtin_shufflevector(d1, d1, 2, 0, 1, 3) * __builtin_shufflevector(d2, d2, 1, 2, 0, 3)); // (d1.yzxw * d2.zxyw) - (d1.zxyw * d2.yzxw);
    dst.x = tmp[0];
    dst.y = tmp[1];
    dst.z = tmp[2];
}

NS_CC_MATH_END

#endif // CC_MATH_MATHUTIL_H
