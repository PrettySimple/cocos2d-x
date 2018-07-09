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

#include "math/Mat4.h"
#include "math/Vec3.h"

NS_CC_MATH_BEGIN

inline bool Vec3::isZero() const
{
    static constexpr auto const zero = f32x4_t{0.f, 0.f, 0.f, 0.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (f32x4_t{x, y, z, 0.f} == zero);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
}

inline bool Vec3::isOne() const
{
    static constexpr auto const one = f32x4_t{1.f, 1.f, 1.f, 1.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (f32x4_t{x, y, z, 1.f} == one);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
}

inline void Vec3::add(const Vec3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;
}

inline void Vec3::add(float xx, float yy, float zz)
{
    x += xx;
    y += yy;
    z += zz;
}

inline float Vec3::length() const
{
    auto const mul = f32x4_t{x, y, z, 1.f} * f32x4_t{x, y, z, 1.f};
    return std::sqrt(mul[0] + mul[1] + mul[2]);
}

inline float Vec3::lengthSquared() const
{
    auto const mul = f32x4_t{x, y, z, 1.f} * f32x4_t{x, y, z, 1.f};
    return mul[0] + mul[1] + mul[2];
}

inline void Vec3::negate()
{
    x = -x;
    y = -y;
    z = -z;
}

inline void Vec3::scale(float scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
}

inline Vec3 Vec3::lerp(const Vec3& target, float alpha) const
{
    return *this * (1.f - alpha) + target * alpha;
}

inline void Vec3::set(float xx, float yy, float zz)
{
    x = xx;
    y = yy;
    z = zz;
}

inline void Vec3::set(const float* array)
{
    GP_ASSERT(array);

    x = array[0];
    y = array[1];
    z = array[2];
}

inline void Vec3::set(const Vec3& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

inline void Vec3::set(const Vec3& p1, const Vec3& p2)
{
    x = p2.x - p1.x;
    y = p2.y - p1.y;
    z = p2.z - p1.z;
}

inline void Vec3::setZero()
{
    x = 0.f;
    y = 0.f;
    z = 0.f;
}

inline void Vec3::subtract(const Vec3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

inline const Vec3 Vec3::operator+(const Vec3& v) const
{
    Vec3 result(*this);
    result.add(v);
    return result;
}

inline Vec3& Vec3::operator+=(const Vec3& v)
{
    add(v);
    return *this;
}

inline const Vec3 Vec3::operator-(const Vec3& v) const
{
    Vec3 result(*this);
    result.subtract(v);
    return result;
}

inline Vec3& Vec3::operator-=(const Vec3& v)
{
    subtract(v);
    return *this;
}

inline const Vec3 Vec3::operator-() const
{
    Vec3 result(*this);
    result.negate();
    return result;
}

inline const Vec3 Vec3::operator*(float s) const
{
    Vec3 result(*this);
    result.scale(s);
    return result;
}

inline Vec3& Vec3::operator*=(float s)
{
    scale(s);
    return *this;
}

inline const Vec3 Vec3::operator/(float s) const
{
    return Vec3(x / s, y / s, z / s);
}

inline bool Vec3::operator==(const Vec3& other) const
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (f32x4_t{x, y, z, 0.f} == f32x4_t{other.x, other.y, other.z, 0.f});
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
}

inline bool Vec3::operator!=(const Vec3& v) const
{
    return !operator==(v);
}

inline const Vec3 operator*(float x, const Vec3& v)
{
    Vec3 result(v);
    result.scale(x);
    return result;
}

NS_CC_MATH_END
