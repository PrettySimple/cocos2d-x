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

#include <cocos/math/Vec3.h>

#include <cocos/base/ccMacros.h>
#include <cocos/math/CCMathBase.h>
#include <cocos/math/MathUtil.h>

#include <cstddef>
#include <limits>

NS_CC_MATH_BEGIN

Vec3::Vec3(const float* array)
{
    set(array);
}

Vec3::Vec3(const Vec3& p1, const Vec3& p2)
{
    set(p1, p2);
}

Vec3 Vec3::fromColor(unsigned int color)
{
    float components[3];
    std::size_t componentIndex = 0;
    for (int i = 2; i >= 0; --i)
    {
        int const component = (color >> i * 8) & 0x0000ff;

        components[componentIndex++] = static_cast<float>(component) / 255.0f;
    }

    return Vec3(components[0], components[1], components[2]);
}

float Vec3::angle(const Vec3& v1, const Vec3& v2)
{
    f32x4_t const p1 = {v1.x, v1.y, v1.z, 0.f};
    f32x4_t const p2 = {v2.x, v2.y, v2.z, 0.f};
    auto const tmp = __builtin_shufflevector(p1, p1, 1, 2, 0, 3) * __builtin_shufflevector(p2, p2, 2, 0, 1, 3) -
        __builtin_shufflevector(p1, p1, 2, 0, 1, 3) * __builtin_shufflevector(p2, p2, 1, 2, 0, 3);
    return std::atan2(std::sqrt(tmp[0] + tmp[1] + tmp[2]) + std::numeric_limits<float>::epsilon(), dot(v1, v2));
}

void Vec3::add(const Vec3& v1, const Vec3& v2, Vec3& dst)
{
    dst.x = v1.x + v2.x;
    dst.y = v1.y + v2.y;
    dst.z = v1.z + v2.z;
}

void Vec3::clamp(const Vec3& min, const Vec3& max)
{
    GP_ASSERT(!(min > max));

    if (x < min.x)
        x = min.x;
    if (y < min.y)
        y = min.y;
    if (z < min.z)
        z = min.z;

    if (x > max.x)
        x = max.x;
    if (y > max.y)
        y = max.y;
    if (z > max.z)
        z = max.z;
}

void Vec3::clamp(const Vec3& other, const Vec3& min, const Vec3& max, Vec3& dst)
{
    GP_ASSERT(!(min > max));

    // Clamp the x value.
    dst.x = other.x;
    dst.y = other.y;
    dst.z = other.z;

    if (dst.x < min.x)
        dst.x = min.x;
    if (dst.y < min.y)
        dst.y = min.y;
    if (dst.z < min.z)
        dst.z = min.z;

    if (dst.x > max.x)
        dst.x = max.x;
    if (dst.y > max.y)
        dst.y = max.y;
    if (dst.z > max.z)
        dst.z = max.z;
}

void Vec3::cross(const Vec3& v)
{
    cross(*this, v, *this);
}

void Vec3::cross(const Vec3& v1, const Vec3& v2, Vec3& dst)
{
    // NOTE: This code assumes Vec3 struct members are contiguous floats in memory.
    // We might want to revisit this (and other areas of code that make this assumption)
    // later to guarantee 100% safety/compatibility.
    crossVec3(v1, v2, dst);
}

float Vec3::distance(const Vec3& other) const
{
    auto const sub = f32x4_t{other.x, other.y, other.z, 0.f} - f32x4_t{x, y, z, 0.f};
    auto const mul = sub * sub;
    return std::sqrt(mul[0] + mul[1] + mul[2]);
}

float Vec3::distanceSquared(const Vec3& other) const
{
    auto const sub = f32x4_t{other.x, other.y, other.z, 0.f} - f32x4_t{x, y, z, 0.f};
    auto const mul = sub * sub;
    return mul[0] + mul[1] + mul[2];
}

float Vec3::dot(const Vec3& other) const
{
    auto const mul = f32x4_t{x, y, z, 0.f} * f32x4_t{other.x, other.y, other.z, 0.f};
    return mul[0] + mul[1] + mul[2];
}

float Vec3::dot(const Vec3& v1, const Vec3& v2)
{
    auto const mul = f32x4_t{v1.x, v1.y, v1.z, 0.f} * f32x4_t{v2.x, v2.y, v2.z, 0.f};
    return mul[0] + mul[1] + mul[2];
}

void Vec3::normalize()
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    auto const mul = f32x4_t{x, y, z, 0.f} * f32x4_t{x, y, z, 0.f};
    float n = mul[0] + mul[1] + mul[2];
    // Already normalized.
    if (std::abs(n - 1.f) < epsi)
        return;

    n = std::sqrt(n);
    // Too close to zero.
    if (n < epsi)
        return;

    n = 1.0f / n;
    x *= n;
    y *= n;
    z *= n;
}

Vec3 Vec3::getNormalized() const
{
    Vec3 v(*this);
    v.normalize();
    return v;
}

void Vec3::subtract(const Vec3& v1, const Vec3& v2, Vec3& dst)
{
    dst.x = v1.x - v2.x;
    dst.y = v1.y - v2.y;
    dst.z = v1.z - v2.z;
}

void Vec3::smooth(const Vec3& target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

const Vec3 Vec3::ZERO(0.0f, 0.0f, 0.0f);
const Vec3 Vec3::ONE(1.0f, 1.0f, 1.0f);
const Vec3 Vec3::UNIT_X(1.0f, 0.0f, 0.0f);
const Vec3 Vec3::UNIT_Y(0.0f, 1.0f, 0.0f);
const Vec3 Vec3::UNIT_Z(0.0f, 0.0f, 1.0f);

NS_CC_MATH_END
