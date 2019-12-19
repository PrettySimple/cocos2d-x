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

#pragma once

#include <cocos/math/CCMathBase.h>
#include <cocos/math/Mat4.h>
#include <cocos/math/Vec3.h>
#include <cocos/math/Vec4.h>
#include <cocos/platform/CCPlatformConfig.h>

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

inline void addMatrix(Mat4 const& other, float scalar, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = other.m[0] + scalar;
    dst.m[1] = other.m[1] + scalar;
    dst.m[2] = other.m[2] + scalar;
    dst.m[3] = other.m[3] + scalar;
    dst.m[4] = other.m[4] + scalar;
    dst.m[5] = other.m[5] + scalar;
    dst.m[6] = other.m[6] + scalar;
    dst.m[7] = other.m[7] + scalar;
    dst.m[8] = other.m[8] + scalar;
    dst.m[9] = other.m[9] + scalar;
    dst.m[10] = other.m[10] + scalar;
    dst.m[11] = other.m[11] + scalar;
    dst.m[12] = other.m[12] + scalar;
    dst.m[13] = other.m[13] + scalar;
    dst.m[14] = other.m[14] + scalar;
    dst.m[15] = other.m[15] + scalar;
#else
    dst.col[0] = other.col[0] + scalar;
    dst.col[1] = other.col[1] + scalar;
    dst.col[2] = other.col[2] + scalar;
    dst.col[3] = other.col[3] + scalar;
#endif
}

inline void addMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = m1.m[0] + m2.m[0];
    dst.m[1] = m1.m[1] + m2.m[1];
    dst.m[2] = m1.m[2] + m2.m[2];
    dst.m[3] = m1.m[3] + m2.m[3];
    dst.m[4] = m1.m[4] + m2.m[4];
    dst.m[5] = m1.m[5] + m2.m[5];
    dst.m[6] = m1.m[6] + m2.m[6];
    dst.m[7] = m1.m[7] + m2.m[7];
    dst.m[8] = m1.m[8] + m2.m[8];
    dst.m[9] = m1.m[9] + m2.m[9];
    dst.m[10] = m1.m[10] + m2.m[10];
    dst.m[11] = m1.m[11] + m2.m[11];
    dst.m[12] = m1.m[12] + m2.m[12];
    dst.m[13] = m1.m[13] + m2.m[13];
    dst.m[14] = m1.m[14] + m2.m[14];
    dst.m[15] = m1.m[15] + m2.m[15];
#else
    dst.col[0] = m1.col[0] + m2.col[0];
    dst.col[1] = m1.col[1] + m2.col[1];
    dst.col[2] = m1.col[2] + m2.col[2];
    dst.col[3] = m1.col[3] + m2.col[3];
#endif
}

inline void subtractMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = m1.m[0] - m2.m[0];
    dst.m[1] = m1.m[1] - m2.m[1];
    dst.m[2] = m1.m[2] - m2.m[2];
    dst.m[3] = m1.m[3] - m2.m[3];
    dst.m[4] = m1.m[4] - m2.m[4];
    dst.m[5] = m1.m[5] - m2.m[5];
    dst.m[6] = m1.m[6] - m2.m[6];
    dst.m[7] = m1.m[7] - m2.m[7];
    dst.m[8] = m1.m[8] - m2.m[8];
    dst.m[9] = m1.m[9] - m2.m[9];
    dst.m[10] = m1.m[10] - m2.m[10];
    dst.m[11] = m1.m[11] - m2.m[11];
    dst.m[12] = m1.m[12] - m2.m[12];
    dst.m[13] = m1.m[13] - m2.m[13];
    dst.m[14] = m1.m[14] - m2.m[14];
    dst.m[15] = m1.m[15] - m2.m[15];
#else
    dst.col[0] = m1.col[0] - m2.col[0];
    dst.col[1] = m1.col[1] - m2.col[1];
    dst.col[2] = m1.col[2] - m2.col[2];
    dst.col[3] = m1.col[3] - m2.col[3];
#endif
}

inline void multiplyMatrix(Mat4 const& other, float scalar, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = other.m[0] * scalar;
    dst.m[1] = other.m[1] * scalar;
    dst.m[2] = other.m[2] * scalar;
    dst.m[3] = other.m[3] * scalar;
    dst.m[4] = other.m[4] * scalar;
    dst.m[5] = other.m[5] * scalar;
    dst.m[6] = other.m[6] * scalar;
    dst.m[7] = other.m[7] * scalar;
    dst.m[8] = other.m[8] * scalar;
    dst.m[9] = other.m[9] * scalar;
    dst.m[10] = other.m[10] * scalar;
    dst.m[11] = other.m[11] * scalar;
    dst.m[12] = other.m[12] * scalar;
    dst.m[13] = other.m[13] * scalar;
    dst.m[14] = other.m[14] * scalar;
    dst.m[15] = other.m[15] * scalar;
#else
    dst.col[0] = other.col[0] * scalar;
    dst.col[1] = other.col[1] * scalar;
    dst.col[2] = other.col[2] * scalar;
    dst.col[3] = other.col[3] * scalar;
#endif
}

inline void multiplyMatrix(Mat4 const& m1, Mat4 const& m2, Mat4& dst)
{
    Mat4 tmp; // Support the case where m1 or m2 is the same matrix as dst.
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    tmp.m[0] = m1.m[0] * m2.m[0] + m1.m[4] * m2.m[1] + m1.m[8] * m2.m[2] + m1.m[12] * m2.m[3];
    tmp.m[1] = m1.m[1] * m2.m[0] + m1.m[5] * m2.m[1] + m1.m[9] * m2.m[2] + m1.m[13] * m2.m[3];
    tmp.m[2] = m1.m[2] * m2.m[0] + m1.m[6] * m2.m[1] + m1.m[10] * m2.m[2] + m1.m[14] * m2.m[3];
    tmp.m[3] = m1.m[3] * m2.m[0] + m1.m[7] * m2.m[1] + m1.m[11] * m2.m[2] + m1.m[15] * m2.m[3];

    tmp.m[4] = m1.m[0] * m2.m[4] + m1.m[4] * m2.m[5] + m1.m[8] * m2.m[6] + m1.m[12] * m2.m[7];
    tmp.m[5] = m1.m[1] * m2.m[4] + m1.m[5] * m2.m[5] + m1.m[9] * m2.m[6] + m1.m[13] * m2.m[7];
    tmp.m[6] = m1.m[2] * m2.m[4] + m1.m[6] * m2.m[5] + m1.m[10] * m2.m[6] + m1.m[14] * m2.m[7];
    tmp.m[7] = m1.m[3] * m2.m[4] + m1.m[7] * m2.m[5] + m1.m[11] * m2.m[6] + m1.m[15] * m2.m[7];

    tmp.m[8] = m1.m[0] * m2.m[8] + m1.m[4] * m2.m[9] + m1.m[8] * m2.m[10] + m1.m[12] * m2.m[11];
    tmp.m[9] = m1.m[1] * m2.m[8] + m1.m[5] * m2.m[9] + m1.m[9] * m2.m[10] + m1.m[13] * m2.m[11];
    tmp.m[10] = m1.m[2] * m2.m[8] + m1.m[6] * m2.m[9] + m1.m[10] * m2.m[10] + m1.m[14] * m2.m[11];
    tmp.m[11] = m1.m[3] * m2.m[8] + m1.m[7] * m2.m[9] + m1.m[11] * m2.m[10] + m1.m[15] * m2.m[11];

    tmp.m[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8] * m2.m[14] + m1.m[12] * m2.m[15];
    tmp.m[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9] * m2.m[14] + m1.m[13] * m2.m[15];
    tmp.m[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];
    tmp.m[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];
#else
    tmp.col[0] = m1.col[0] * m2.col[0][0] + m1.col[1] * m2.col[0][1] + m1.col[2] * m2.col[0][2] + m1.col[3] * m2.col[0][3];
    tmp.col[1] = m1.col[0] * m2.col[1][0] + m1.col[1] * m2.col[1][1] + m1.col[2] * m2.col[1][2] + m1.col[3] * m2.col[1][3];
    tmp.col[2] = m1.col[0] * m2.col[2][0] + m1.col[1] * m2.col[2][1] + m1.col[2] * m2.col[2][2] + m1.col[3] * m2.col[2][3];
    tmp.col[3] = m1.col[0] * m2.col[3][0] + m1.col[1] * m2.col[3][1] + m1.col[2] * m2.col[3][2] + m1.col[3] * m2.col[3][3];
#endif
    dst = tmp;
}

inline void negateMatrix(Mat4 const& other, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = -other.m[0];
    dst.m[1] = -other.m[1];
    dst.m[2] = -other.m[2];
    dst.m[3] = -other.m[3];
    dst.m[4] = -other.m[4];
    dst.m[5] = -other.m[5];
    dst.m[6] = -other.m[6];
    dst.m[7] = -other.m[7];
    dst.m[8] = -other.m[8];
    dst.m[9] = -other.m[9];
    dst.m[10] = -other.m[10];
    dst.m[11] = -other.m[11];
    dst.m[12] = -other.m[12];
    dst.m[13] = -other.m[13];
    dst.m[14] = -other.m[14];
    dst.m[15] = -other.m[15];
#else
    dst.col[0] = -other.col[0];
    dst.col[1] = -other.col[1];
    dst.col[2] = -other.col[2];
    dst.col[3] = -other.col[3];
#endif
}

inline void transposeMatrix(Mat4 const& other, Mat4& dst)
{
    Mat4 tmp; // Support the case where other is the same matrix as dst.
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    tmp.m[0] = other.m[0];
    tmp.m[1] = other.m[4];
    tmp.m[2] = other.m[8];
    tmp.m[3] = other.m[12];

    tmp.m[4] = other.m[1];
    tmp.m[5] = other.m[5];
    tmp.m[6] = other.m[9];
    tmp.m[7] = other.m[13];

    tmp.m[8] = other.m[2];
    tmp.m[9] = other.m[6];
    tmp.m[10] = other.m[10];
    tmp.m[11] = other.m[14];

    tmp.m[12] = other.m[3];
    tmp.m[13] = other.m[7];
    tmp.m[14] = other.m[11];
    tmp.m[15] = other.m[15];
#else
    tmp.col[0] = {other.m[0], other.m[4], other.m[8], other.m[12]};
    tmp.col[1] = {other.m[1], other.m[5], other.m[9], other.m[13]};
    tmp.col[2] = {other.m[2], other.m[6], other.m[10], other.m[14]};
    tmp.col[3] = {other.m[3], other.m[7], other.m[11], other.m[15]};
#endif
    dst = tmp;
}

inline void transformVec4(Mat4 const& other, float x, float y, float z, float w, Vec3& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.x = x * other.m[0] + y * other.m[4] + z * other.m[8] + w * other.m[12];
    dst.y = x * other.m[1] + y * other.m[5] + z * other.m[9] + w * other.m[13];
    dst.z = x * other.m[2] + y * other.m[6] + z * other.m[10] + w * other.m[14];
#else
    auto const tmp = x * other.col[0] + y * other.col[1] + z * other.col[2] + w * other.col[3];
    dst.x = tmp[0];
    dst.y = tmp[1];
    dst.z = tmp[2];
#endif
}

inline void transformVec4(Mat4 const& mat, Vec4 const& other, Vec4& dst)
{
    Vec4 tmp; // Support the case where other is the same as dst.
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    tmp.x = other.x * mat.m[0] + other.y * mat.m[4] + other.z * mat.m[8] + other.w * mat.m[12];
    tmp.y = other.x * mat.m[1] + other.y * mat.m[5] + other.z * mat.m[9] + other.w * mat.m[13];
    tmp.z = other.x * mat.m[2] + other.y * mat.m[6] + other.z * mat.m[10] + other.w * mat.m[14];
    tmp.w = other.x * mat.m[3] + other.y * mat.m[7] + other.z * mat.m[11] + other.w * mat.m[15];
#else
    tmp.v = other.v[0] * mat.col[0] + other.v[1] * mat.col[1] + other.v[2] * mat.col[2] + other.v[3] * mat.col[3];
#endif
    dst = tmp;
}

inline void crossVec3(Vec3 const& v1, Vec3 const& v2, Vec3& dst)
{
    Vec3 tmp; // Support the case where v1 or v2 is the same as dst.
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    tmp.x = (v1.y * v2.z) - (v1.z * v2.y);
    tmp.y = (v1.z * v2.x) - (v1.x * v2.z);
    tmp.z = (v1.x * v2.y) - (v1.y * v2.x);
#else
    auto const d1 = Vec4::f32x4_t{v1.x, v1.y, v1.z, 0.f};
    auto const d2 = Vec4::f32x4_t{v2.x, v2.y, v2.z, 0.f};
    auto const cross = (__builtin_shufflevector(d1, d1, 1, 2, 0, 3) * __builtin_shufflevector(d2, d2, 2, 0, 1, 3)) -
        (__builtin_shufflevector(d1, d1, 2, 0, 1, 3) * __builtin_shufflevector(d2, d2, 1, 2, 0, 3)); // (d1.yzxw * d2.zxyw) - (d1.zxyw * d2.yzxw);
    tmp.x = cross[0];
    tmp.y = cross[1];
    tmp.z = cross[2];
#endif
    dst = tmp;
}

NS_CC_MATH_END
