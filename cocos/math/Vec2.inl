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

#include "math/Vec2.h"

NS_CC_MATH_BEGIN

inline Vec2::Vec2(float const* array)
{
    set(array);
}

inline Vec2::Vec2(Vec2 const& p1, Vec2 const& p2)
{
    set(p1, p2);
}

inline bool Vec2::isZero() const
{
    static constexpr auto const zero = f32x2_t{0.f, 0.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == zero);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1;
}

bool Vec2::isOne() const
{
    static constexpr auto const one = f32x2_t{1.f, 1.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == one);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1;
}

inline void Vec2::add(Vec2 const& other)
{
    v += other.v;
}

inline float Vec2::distanceSquared(Vec2 const& other) const
{
    auto const diff = other.v - v;
    auto const mul = diff * diff;
    return mul[0] + mul[1];
}

inline float Vec2::dot(Vec2 const& other) const
{
    auto const mul = v * other.v;
    return mul[0] + mul[1];
}

inline float Vec2::lengthSquared() const
{
    auto const mul = v * v;
    return mul[0] + mul[1];
}

inline void Vec2::negate()
{
    v = -v;
}

inline void Vec2::scale(float scalar)
{
    v *= scalar;
}

inline void Vec2::scale(Vec2 const& scale)
{
    v *= scale.v;
}

inline void Vec2::set(float xx, float yy)
{
    v = {xx, yy};
}

inline void Vec2::set(Vec2 const& other)
{
    v = other.v;
}

inline void Vec2::set(Vec2 const& p1, Vec2 const& p2)
{
    v = p2.v - p1.v;
}

void Vec2::setZero()
{
    v = {0.0f, 0.f};
}

inline void Vec2::subtract(Vec2 const& other)
{
    //    auto const t1 = std::chrono::high_resolution_clock::now();
    //    auto const t2 = std::chrono::high_resolution_clock::now();
    //    std::cout << "exec= " << (t2 - t1).count() << " ns\n";

    /*
     Options #1: Plain
        x -= v.x;
        y -= v.y;

     Options #2: SSE
        auto const l = _mm_movelh_ps(_mm_load_ss(&x), _mm_load_ss(&y));
        auto const o = _mm_movelh_ps(_mm_load_ss(&other.x), _mm_load_ss(&other.y));
        auto const diff = _mm_sub_ps(l, o);
        x = _mm_cvtss_f32(_mm_shuffle_ps(diff, diff, _MM_SHUFFLE(0, 0, 0, 0)));
        y = _mm_cvtss_f32(_mm_shuffle_ps(diff, diff, _MM_SHUFFLE(0, 0, 0, 2)));

     Options #3: Vectors
        v -= other.v;

     Benchmark (in ns)
                    |     avg | min |  max |          σ |
         Vectors #3 |  90.440 |  40 |  606 |   4419.921 |
         SSE     #2 | 459.133 | 103 | 2869 | 295263.971 |
         Plain   #1 | 246.056 |  63 | 1824 |  79086.153 |
     */

    v -= other.v;
}

inline void Vec2::smooth(Vec2 const& target, float elapsedTime, float responseTime)
{
    if (elapsedTime > 0.f)
    {
        *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
    }
}

inline Vec2 const Vec2::operator+(Vec2 const& v) const
{
    Vec2 result(*this);
    result.add(v);
    return result;
}

inline Vec2& Vec2::operator+=(Vec2 const& v)
{
    add(v);
    return *this;
}

inline Vec2 const Vec2::operator-(Vec2 const& v) const
{
    Vec2 result(*this);
    result.subtract(v);
    return result;
}

inline Vec2& Vec2::operator-=(Vec2 const& v)
{
    subtract(v);
    return *this;
}

inline Vec2 const Vec2::operator-() const
{
    Vec2 result(*this);
    result.negate();
    return result;
}

inline Vec2 const Vec2::operator*(float s) const
{
    Vec2 result(*this);
    result.scale(s);
    return result;
}

inline Vec2& Vec2::operator*=(float s)
{
    scale(s);
    return *this;
}

inline Vec2 const Vec2::operator/(float s) const
{
    return Vec2(v / s);
}

inline bool Vec2::operator<(Vec2 const& other) const
{
    auto const lt = v < other.v;
    return lt[0] == -1 && lt[1] == -1;
}

inline bool Vec2::operator>(Vec2 const& other) const
{
    auto const gt = v > other.v;
    return gt[0] == -1 && gt[1] == -1;
}

inline bool Vec2::operator==(Vec2 const& other) const
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == other.v);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1;
}

inline bool Vec2::operator!=(Vec2 const& v) const
{
    return !operator==(v);
}

inline Vec2 const operator*(float x, Vec2 const& v)
{
    Vec2 result(v);
    result.scale(x);
    return result;
}

void Vec2::setPoint(float xx, float yy)
{
    v = {xx, yy};
}

NS_CC_MATH_END
