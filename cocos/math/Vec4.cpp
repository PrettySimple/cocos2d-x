/**
 Copyright 2013 BlackBerry Inc.

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

#include <cocos/math/Vec4.h>

#include <cocos/base/ccMacros.h>
#include <cocos/math/CCMathBase.h>

#include <cmath>
#include <limits>

NS_CC_MATH_BEGIN

Vec4::Vec4(const float* src)
{
    set(src);
}

Vec4::Vec4(const Vec4& p1, const Vec4& p2)
{
    set(p1, p2);
}

Vec4 Vec4::fromColor(unsigned int color)
{
    f32x4_t components = {0.f, 0.f, 0.f, 0.f};
    int componentIndex = 0;
    for (int i = 3; i >= 0; --i)
    {
        int component = (color >> i * 8) & 0x000000ff;

        components[componentIndex++] = static_cast<float>(component) / 255.0f;
    }
    return Vec4(std::move(components));
    ;
}

bool Vec4::isZero() const
{
    static constexpr auto const zero = f32x4_t{0.f, 0.f, 0.f, 0.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == zero);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
}

bool Vec4::isOne() const
{
    static constexpr auto const one = f32x4_t{1.f, 1.f, 1.f, 1.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == one);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
}

float Vec4::angle(const Vec4& v1, const Vec4& v2)
{
    auto const tmp = v1.v[3] * v2.v - v1.v * v2.v[3] - __builtin_shufflevector(v1.v, v1.v, 1, 2, 0, 3) * __builtin_shufflevector(v2.v, v2.v, 2, 0, 1, 3) +
        __builtin_shufflevector(v1.v, v1.v, 2, 0, 1, 3) * __builtin_shufflevector(v2.v, v2.v, 1, 2, 0, 3);
    return std::atan2(std::sqrt(tmp[0] + tmp[1] + tmp[2]) + std::numeric_limits<float>::epsilon(), dot(v1, v2));
}

void Vec4::add(const Vec4& other)
{
    v += other.v;
}

void Vec4::add(const Vec4& v1, const Vec4& v2, Vec4* dst)
{
    GP_ASSERT(dst);

    dst->v = v1.v + v2.v;
}

void Vec4::clamp(const Vec4& min, const Vec4& max)
{
    GP_ASSERT(!(min > max));

    // Clamp the x value.
    if (v[0] < min.v[0])
        v[0] = min.v[0];
    if (v[1] < min.v[1])
        v[1] = min.v[1];
    if (v[2] < min.v[2])
        v[2] = min.v[2];
    if (v[3] < min.v[3])
        v[3] = min.v[3];

    if (v[0] > max.v[0])
        v[0] = max.v[0];
    if (v[1] > max.v[1])
        v[1] = max.v[1];
    if (v[2] > max.v[2])
        v[2] = max.v[2];
    if (v[3] > max.v[3])
        v[3] = max.v[3];
}

void Vec4::clamp(const Vec4& other, const Vec4& min, const Vec4& max, Vec4* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(!(min > max));

    // Clamp the x value.
    dst->v = other.v;
    if (dst->v[0] < min.v[0])
        dst->v[0] = min.v[0];
    if (dst->v[1] < min.v[1])
        dst->v[1] = min.v[1];
    if (dst->v[2] < min.v[2])
        dst->v[2] = min.v[2];
    if (dst->v[3] < min.v[3])
        dst->v[3] = min.v[3];

    if (dst->v[0] > max.v[0])
        dst->v[0] = max.v[0];
    if (dst->v[1] > max.v[1])
        dst->v[1] = max.v[1];
    if (dst->v[2] > max.v[2])
        dst->v[2] = max.v[2];
    if (dst->v[3] > max.v[3])
        dst->v[3] = max.v[3];
}

float Vec4::distance(const Vec4& other) const
{
    auto const sub = other.v - v;
    auto const mul = sub * sub;
    return std::sqrt(mul[0] + mul[1] + mul[2] + mul[3]);
}

float Vec4::distanceSquared(const Vec4& other) const
{
    auto const sub = other.v - v;
    auto const mul = sub * sub;
    return mul[0] + mul[1] + mul[2] + mul[3];
}

float Vec4::dot(const Vec4& other) const
{
    auto const mul = v * other.v;
    return mul[0] + mul[1] + mul[2] + mul[3];
}

float Vec4::dot(const Vec4& v1, const Vec4& v2)
{
    auto const mul = v1.v * v2.v;
    return mul[0] + mul[1] + mul[2] + mul[3];
}

float Vec4::length() const
{
    auto const mul = v * v;
    return std::sqrt(mul[0] + mul[1] + mul[2] + mul[3]);
}

float Vec4::lengthSquared() const
{
    auto const mul = v * v;
    return mul[0] + mul[1] + mul[2] + mul[3];
}

void Vec4::negate()
{
    v = -v;
}

void Vec4::normalize()
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    auto const mul = v * v;
    float n = mul[0] + mul[1] + mul[2] + mul[3];
    // Already normalized.
    if (std::abs(n - 1.f) < epsi)
        return;

    n = std::sqrt(n);
    // Too close to zero.
    if (n < epsi)
        return;

    n = 1.0f / n;
    v *= n;
}

Vec4 Vec4::getNormalized() const
{
    Vec4 v(*this);
    v.normalize();
    return v;
}

void Vec4::scale(float scalar)
{
    v *= scalar;
}

void Vec4::set(float xx, float yy, float zz, float ww)
{
    v = {xx, yy, zz, ww};
}

void Vec4::set(const float* array)
{
    GP_ASSERT(array);

    v = {array[0], array[1], array[2], array[3]};
}

void Vec4::set(const Vec4& other)
{
    v = other.v;
}

void Vec4::set(const Vec4& p1, const Vec4& p2)
{
    v = p2.v - p1.v;
}

void Vec4::subtract(const Vec4& other)
{
    v -= other.v;
}

void Vec4::subtract(const Vec4& v1, const Vec4& v2, Vec4* dst)
{
    GP_ASSERT(dst);
    dst->v = v1.v - v2.v;
}

const Vec4 Vec4::ZERO = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
const Vec4 Vec4::ONE = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
const Vec4 Vec4::UNIT_X = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
const Vec4 Vec4::UNIT_Y = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
const Vec4 Vec4::UNIT_Z = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
const Vec4 Vec4::UNIT_W = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

NS_CC_MATH_END
