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
#include "math/Vec4.h"

NS_CC_MATH_BEGIN

inline const Vec4 Vec4::operator+(const Vec4& v) const
{
    Vec4 result(*this);
    result.add(v);
    return result;
}

inline Vec4& Vec4::operator+=(const Vec4& v)
{
    add(v);
    return *this;
}

inline const Vec4 Vec4::operator-(const Vec4& v) const
{
    Vec4 result(*this);
    result.subtract(v);
    return result;
}

inline Vec4& Vec4::operator-=(const Vec4& v)
{
    subtract(v);
    return *this;
}

inline const Vec4 Vec4::operator-() const
{
    Vec4 result(*this);
    result.negate();
    return result;
}

inline const Vec4 Vec4::operator*(float s) const
{
    Vec4 result(*this);
    result.scale(s);
    return result;
}

inline Vec4& Vec4::operator*=(float s)
{
    scale(s);
    return *this;
}

inline const Vec4 Vec4::operator/(float s) const
{
    return Vec4(v / s);
}

inline bool Vec4::operator<(const Vec4& other) const
{
    auto const lt = v < other.v;
    return lt[0] == -1 && lt[1] == -1 && lt[2] == -1 && lt[3] == -1;
}

inline bool Vec4::operator==(const Vec4& other) const
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == other.v);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
}

inline bool Vec4::operator!=(const Vec4& v) const
{
    return !operator==(v);
}

inline const Vec4 operator*(float x, const Vec4& v)
{
    Vec4 result(v);
    result.scale(x);
    return result;
}

NS_CC_MATH_END
