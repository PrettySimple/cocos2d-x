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

#include <cocos/math/Vec2.h>

#include <cocos/base/ccMacros.h>
#include <cocos/math/CCMathBase.h>

#include <algorithm>
#include <limits>

NS_CC_MATH_BEGIN

Vec2::Vec2(float const* array)
{
    set(array);
}

Vec2::Vec2(Vec2 const& p1, Vec2 const& p2)
{
    set(p1, p2);
}

// returns true if segment A-B intersects with segment C-D. S->E is the overlap part
bool isOneDimensionSegmentOverlap(float A, float B, float C, float D, float* S, float* E)
{
    float ABmin = std::min(A, B);
    float ABmax = std::max(A, B);
    float CDmin = std::min(C, D);
    float CDmax = std::max(C, D);

    if (ABmax < CDmin || CDmax < ABmin)
    {
        // ABmin->ABmax->CDmin->CDmax or CDmin->CDmax->ABmin->ABmax
        return false;
    }
    else
    {
        if (ABmin >= CDmin && ABmin <= CDmax)
        {
            // CDmin->ABmin->CDmax->ABmax or CDmin->ABmin->ABmax->CDmax
            if (S != nullptr)
                *S = ABmin;
            if (E != nullptr)
                *E = CDmax < ABmax ? CDmax : ABmax;
        }
        else if (ABmax >= CDmin && ABmax <= CDmax)
        {
            // ABmin->CDmin->ABmax->CDmax
            if (S != nullptr)
                *S = CDmin;
            if (E != nullptr)
                *E = ABmax;
        }
        else
        {
            // ABmin->CDmin->CDmax->ABmax
            if (S != nullptr)
                *S = CDmin;
            if (E != nullptr)
                *E = CDmax;
        }
        return true;
    }
}

// cross product of 2 vector. A->B X C->D
float crossProduct2Vector(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
    auto const DC = D.v - C.v;
    auto const BA = B.v - A.v;
    auto const temp = __builtin_shufflevector(DC, DC, 1, 0) * BA;
    return temp[0] - temp[1];
}

float Vec2::angle(Vec2 const& v1, Vec2 const& v2)
{
    auto const mul = v1.v * __builtin_shufflevector(v2.v, v2.v, 1, 0);
    return std::atan2(std::abs(mul[0] - mul[1]) + std::numeric_limits<float>::epsilon(), dot(v1, v2));
}

void Vec2::add(const Vec2& v1, const Vec2& v2, Vec2* dst)
{
    GP_ASSERT(dst);

    dst->v = v1.v + v2.v;
}

void Vec2::clamp(const Vec2& min, const Vec2& max)
{
    GP_ASSERT(!(min > max));

    // Clamp the x value.
    if (v[0] < min.v[0])
        v[0] = min.v[0];
    if (v[1] < min.v[1])
        v[1] = min.v[1];
    if (v[0] > max.v[0])
        v[0] = max.v[0];
    if (v[1] > max.v[1])
        v[1] = max.v[1];
}

void Vec2::clamp(const Vec2& other, const Vec2& min, const Vec2& max, Vec2* dst)
{
    GP_ASSERT(dst);
    GP_ASSERT(!(min > max));

    // Clamp the x value.
    dst->v = other.v;

    if (other.v[0] < min.v[0])
        dst->v[0] = min.v[0];
    if (other.v[1] < min.v[1])
        dst->v[1] = min.v[1];
    if (other.v[0] > max.v[0])
        dst->v[0] = max.v[0];
    if (other.v[1] > max.v[1])
        dst->v[1] = max.v[1];
}

float Vec2::distance(Vec2 const& other) const
{
    auto const diff = other.v - v;
    auto const mul = diff * diff;
    return std::sqrt(mul[0] + mul[1]);
}

float Vec2::dot(const Vec2& v1, const Vec2& v2)
{
    auto const mul = v1.v * v2.v;
    return mul[0] + mul[1];
}

float Vec2::length() const
{
    auto const mul = v * v;
    return std::sqrt(mul[0] + mul[1]);
}

void Vec2::normalize()
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    auto const mul = v * v;
    float n = mul[0] + mul[1];
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

Vec2 Vec2::getNormalized() const
{
    Vec2 v(*this);
    v.normalize();
    return v;
}

void Vec2::rotate(const Vec2& point, float angle)
{
    auto const sinAngle = std::sin(angle);
    auto const cosAngle = std::cos(angle);
    auto const tmp = v - point.v;
    v = tmp * cosAngle + __builtin_shufflevector(tmp, tmp, 1, 0) * f32x2_t{-1.f, 0.f} * sinAngle + point.v;
}

void Vec2::set(const float* array)
{
    GP_ASSERT(array);

    v = {array[0], array[1]};
}

void Vec2::subtract(const Vec2& v1, const Vec2& v2, Vec2* dst)
{
    GP_ASSERT(dst);

    dst->v = v1.v - v2.v;
}

bool Vec2::equals(Vec2 const& other) const noexcept
{
    /*
     Options #1: Plain
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        auto const result = (std::abs(this->x - target.x) < epsi) && (std::abs(this->y - target.y) < epsi);

     Options #2: SSE
        __m128 l = _mm_movelh_ps(_mm_load_ss(&x), _mm_load_ss(&y));
        __m128 o = _mm_movelh_ps(_mm_load_ss(&target.x), _mm_load_ss(&target.y));
        auto const result = (_mm_movemask_ps(_mm_cmpeq_ps(l, o)) & 0b0101) == 5;

     Options #3: Vectors
        auto const result = (reinterpret_cast<std::int64_t>(v == target.v) & 0b0101) == 5;

     Benchmark (in ns)
                   |     avg | min |  max |         σ |
        Vectors #3 |  45.687 |  35 |   99 |   122.162 |
        SSE     #2 |  48.808 |  36 |  125 |   189.094 |
        Plain   #1 | 100.616 |  37 | 1498 | 12195.935 |
     */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == other.v);
#pragma pop
    return eq[0] == -1 && eq[1] == -1;
}

bool Vec2::fuzzyEquals(const Vec2& b, float var) const
{
    auto const sub_lteq = (v - var) <= b.v;
    auto const add_lteq = b.v <= (v + var);
    if (sub_lteq[0] == -1 && sub_lteq[1] == -1 && add_lteq[0] == -1 && add_lteq[1] == -1)
    {
        return true;
    }
    return false;
}

float Vec2::getAngle(const Vec2& other) const
{
    Vec2 const a2 = getNormalized();
    Vec2 const b2 = other.getNormalized();
    return std::atan2(a2.cross(b2), a2.dot(b2));
}

Vec2 Vec2::rotateByAngle(const Vec2& pivot, float angle) const
{
    return pivot + (*this - pivot).rotate(Vec2::forAngle(angle));
}

bool Vec2::isLineIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D, float* S, float* T)
{
    // FAIL: Line undefined
    if (A == B || C == D)
    {
        return false;
    }

    float const denom = crossProduct2Vector(A, B, C, D);

    if (std::abs(denom) < std::numeric_limits<float>::epsilon())
    {
        // Lines parallel or overlap
        return false;
    }

    if (S != nullptr)
        *S = crossProduct2Vector(C, D, C, A) / denom;
    if (T != nullptr)
        *T = crossProduct2Vector(A, B, C, A) / denom;

    return true;
}

bool Vec2::isLineParallel(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
    // FAIL: Line undefined
    if (A == B || C == D)
    {
        return false;
    }

    if (crossProduct2Vector(A, B, C, D) == 0.f)
    {
        // line overlap
        if (crossProduct2Vector(C, D, C, A) == 0.f || crossProduct2Vector(A, B, C, A) == 0.f)
        {
            return false;
        }

        return true;
    }

    return false;
}

bool Vec2::isLineOverlap(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
    // FAIL: Line undefined
    if (A == B || C == D)
    {
        return false;
    }

    if (crossProduct2Vector(A, B, C, D) == 0.f && (crossProduct2Vector(C, D, C, A) == 0.f || crossProduct2Vector(A, B, C, A) == 0.f))
    {
        return true;
    }

    return false;
}

bool Vec2::isSegmentOverlap(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D, Vec2* S, Vec2* E)
{
    if (isLineOverlap(A, B, C, D))
    {
        return isOneDimensionSegmentOverlap(A.x, B.x, C.x, D.x, &S->x, &E->x) && isOneDimensionSegmentOverlap(A.y, B.y, C.y, D.y, &S->y, &E->y);
    }

    return false;
}

bool Vec2::isSegmentIntersect(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
    float S, T;

    if (isLineIntersect(A, B, C, D, &S, &T) && (S >= 0.0f && S <= 1.0f && T >= 0.0f && T <= 1.0f))
    {
        return true;
    }

    return false;
}

Vec2 Vec2::getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D)
{
    float S, T;

    if (isLineIntersect(A, B, C, D, &S, &T))
    {
        // Vec2 of intersection
        Vec2 P;
        P.v = A.v + S * (B.v - B.v);
        return P;
    }

    return Vec2::ZERO;
}

const Vec2 Vec2::ZERO(0.0f, 0.0f);
const Vec2 Vec2::ONE(1.0f, 1.0f);
const Vec2 Vec2::UNIT_X(1.0f, 0.0f);
const Vec2 Vec2::UNIT_Y(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE(0.5f, 0.5f);
const Vec2 Vec2::ANCHOR_BOTTOM_LEFT(0.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_LEFT(0.0f, 1.0f);
const Vec2 Vec2::ANCHOR_BOTTOM_RIGHT(1.0f, 0.0f);
const Vec2 Vec2::ANCHOR_TOP_RIGHT(1.0f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_RIGHT(1.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_LEFT(0.0f, 0.5f);
const Vec2 Vec2::ANCHOR_MIDDLE_TOP(0.5f, 1.0f);
const Vec2 Vec2::ANCHOR_MIDDLE_BOTTOM(0.5f, 0.0f);

NS_CC_MATH_END
