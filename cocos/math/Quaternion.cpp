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

#include <cocos/math/Quaternion.h>

#include <cocos/base/ccMacros.h>

#include <cmath>
#include <limits>

NS_CC_MATH_BEGIN

const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

Quaternion::Quaternion(float* array)
{
    set(array);
}

Quaternion::Quaternion(const Mat4& m)
{
    set(m);
}

Quaternion::Quaternion(const Vec3& axis, float angle)
{
    set(axis, angle);
}

const Quaternion& Quaternion::identity()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 1.0f);
    return value;
}

const Quaternion& Quaternion::zero()
{
    static Quaternion value(0.0f, 0.0f, 0.0f, 0.0f);
    return value;
}

bool Quaternion::isIdentity() const
{
    static constexpr auto const identity = f32x4_t{0.f, 0.f, 0.f, 1.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == identity);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
}

bool Quaternion::isZero() const
{
    static constexpr auto const zero = f32x4_t{0.f, 0.f, 0.f, 0.f};
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == zero);
#pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
}

void Quaternion::createFromRotationMatrix(const Mat4& m, Quaternion& dst)
{
    m.getRotation(&dst);
}

void Quaternion::createFromAxisAngle(const Vec3& axis, float angle, Quaternion& dst)
{
    float const halfAngle = angle * 0.5f;

    Vec3 normal(axis);
    normal.normalize();
    normal *= std::sin(halfAngle);
    dst.v = {normal.x, normal.y, normal.z, std::cos(halfAngle)};
}

void Quaternion::conjugate()
{
    v = -v;
    v[3] = -v[3];
    // w =  w;
}

Quaternion Quaternion::getConjugated() const
{
    Quaternion q(*this);
    q.conjugate();
    return q;
}

bool Quaternion::inverse()
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    auto const mul = v * v;
    float n = mul[0] + mul[1] + mul[2] + mul[3];
    // Already normalized.
    if (std::abs(n - 1.f) < epsi)
    {
        v = -v;
        v[3] = -v[3];
        // w = w;
        return true;
    }

    // Too close to zero.
    if (std::abs(n) < epsi)
        return false;

    n = 1.0f / n;
    v = -v;
    v[3] = -v[3];
    v *= n;
    return true;
}

Quaternion Quaternion::getInversed() const
{
    Quaternion q(*this);
    q.inverse();
    return q;
}

void Quaternion::multiply(const Quaternion& q)
{
    multiply(*this, q, *this);
}

void Quaternion::multiply(const Quaternion& q1, const Quaternion& q2, Quaternion& dst)
{
    dst.v = q1.v[3] * q2.v + f32x4_t{1.f, -1.f, 1.f, -1.f} * q1.v[0] * __builtin_shufflevector(q2.v, q2.v, 3, 2, 1, 0) +
        f32x4_t{1.f, 1.f, -1.f, -1.f} * q1.v[1] * __builtin_shufflevector(q2.v, q2.v, 2, 3, 0, 1) +
        f32x4_t{-1.f, 1.f, 1.f, -1.f} * q1.v[2] * __builtin_shufflevector(q2.v, q2.v, 1, 0, 3, 2);
}

void Quaternion::normalize()
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    auto const mul = v * v;
    float n = mul[0] + mul[1] + mul[2] + mul[3];
    // Already normalized.
    if (std::abs(n - 1.f) < epsi)
        return;

    n = std::sqrt(n);
    // Too close to zero.
    if (std::abs(n) < epsi)
        return;

    n = 1.0f / n;
    v *= n;
}

Quaternion Quaternion::getNormalized() const
{
    Quaternion q(*this);
    q.normalize();
    return q;
}

void Quaternion::set(float xx, float yy, float zz, float ww)
{
    v = {xx, yy, zz, ww};
}

void Quaternion::set(float* array)
{
    GP_ASSERT(array);

    v = {array[0], array[1], array[2], array[3]};
}

void Quaternion::set(const Mat4& m)
{
    Quaternion::createFromRotationMatrix(m, *this);
}

void Quaternion::set(const Vec3& axis, float angle)
{
    Quaternion::createFromAxisAngle(axis, angle, *this);
}

void Quaternion::set(const Quaternion& q)
{
    v = q.v;
}

void Quaternion::setIdentity()
{
    v = {0.f, 0.f, 0.f, 1.f};
}

float Quaternion::toAxisAngle(Vec3& axis) const
{
    Quaternion q(x, y, z, w);
    q.normalize();
    axis.x = q.v[0];
    axis.y = q.v[1];
    axis.z = q.v[2];
    axis.normalize();

    return (2.0f * std::acos(q.w));
}

void Quaternion::lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& dst)
{
    GP_ASSERT(!(t < 0.0f || t > 1.0f));

    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    if (std::abs(t) < epsi)
    {
        dst.v = q1.v;
        return;
    }
    else if (std::abs(t - 1.0f) < epsi)
    {
        dst.v = q2.v;
        return;
    }

    dst.v = (1.0f - t) * q1.v + t * q2.v;
}

void Quaternion::slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& dst)
{
    slerp(q1.x, q1.y, q1.z, q1.w, q2.x, q2.y, q2.z, q2.w, t, &dst.x, &dst.y, &dst.z, &dst.w);
}

void Quaternion::squad(const Quaternion& q1, const Quaternion& q2, const Quaternion& s1, const Quaternion& s2, float t, Quaternion& dst)
{
    GP_ASSERT(!(t < 0.0f || t > 1.0f));

    Quaternion dstQ(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion dstS(0.0f, 0.0f, 0.0f, 1.0f);

    slerpForSquad(q1, q2, t, dstQ);
    slerpForSquad(s1, s2, t, dstS);
    slerpForSquad(dstQ, dstS, 2.0f * t * (1.0f - t), dst);
}

void Quaternion::slerp(float q1x, float q1y, float q1z, float q1w, float q2x, float q2y, float q2z, float q2w, float t, float* dstx, float* dsty, float* dstz,
                       float* dstw)
{
    // Fast slerp implementation by kwhatmough:
    // It contains no division operations, no trig, no inverse trig
    // and no sqrt. Not only does this code tolerate small constraint
    // errors in the input quaternions, it actually corrects for them.
    GP_ASSERT(dstx && dsty && dstz && dstw);
    GP_ASSERT(!(t < 0.0f || t > 1.0f));

    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    if (std::abs(t) < epsi)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }
    if (std::abs(t - 1.f) < epsi)
    {
        *dstx = q2x;
        *dsty = q2y;
        *dstz = q2z;
        *dstw = q2w;
        return;
    }

    if (std::abs(q1x - q2x) < epsi && std::abs(q1y - q2y) < epsi && std::abs(q1z - q2z) < epsi && std::abs(q1w - q2w) < epsi)
    {
        *dstx = q1x;
        *dsty = q1y;
        *dstz = q1z;
        *dstw = q1w;
        return;
    }

    float halfY, alpha, beta;
    float u, f1, f2a, f2b;
    float ratio1, ratio2;
    float halfSecHalfTheta, versHalfTheta;
    float sqNotU, sqU;

    float cosTheta = q1w * q2w + q1x * q2x + q1y * q2y + q1z * q2z;

    // As usual in all slerp implementations, we fold theta.
    alpha = cosTheta >= 0 ? 1.0f : -1.0f;
    halfY = 1.0f + alpha * cosTheta;

    // Here we bisect the interval, so we need to fold t as well.
    f2b = t - 0.5f;
    u = f2b >= 0 ? f2b : -f2b;
    f2a = u - f2b;
    f2b += u;
    u += u;
    f1 = 1.0f - u;

    // One iteration of Newton to get 1-cos(theta / 2) to good accuracy.
    halfSecHalfTheta = 1.09f - (0.476537f - 0.0903321f * halfY) * halfY;
    halfSecHalfTheta *= 1.5f - halfY * halfSecHalfTheta * halfSecHalfTheta;
    versHalfTheta = 1.0f - halfY * halfSecHalfTheta;

    // Evaluate series expansions of the coefficients.
    sqNotU = f1 * f1;
    ratio2 = 0.0000440917108f * versHalfTheta;
    ratio1 = -0.00158730159f + (sqNotU - 16.0f) * ratio2;
    ratio1 = 0.0333333333f + ratio1 * (sqNotU - 9.0f) * versHalfTheta;
    ratio1 = -0.333333333f + ratio1 * (sqNotU - 4.0f) * versHalfTheta;
    ratio1 = 1.0f + ratio1 * (sqNotU - 1.0f) * versHalfTheta;

    sqU = u * u;
    ratio2 = -0.00158730159f + (sqU - 16.0f) * ratio2;
    ratio2 = 0.0333333333f + ratio2 * (sqU - 9.0f) * versHalfTheta;
    ratio2 = -0.333333333f + ratio2 * (sqU - 4.0f) * versHalfTheta;
    ratio2 = 1.0f + ratio2 * (sqU - 1.0f) * versHalfTheta;

    // Perform the bisection and resolve the folding done earlier.
    f1 *= ratio1 * halfSecHalfTheta;
    f2a *= ratio2;
    f2b *= ratio2;
    alpha *= f1 + f2a;
    beta = f1 + f2b;

    // Apply final coefficients to a and b as usual.
    float w = alpha * q1w + beta * q2w;
    float x = alpha * q1x + beta * q2x;
    float y = alpha * q1y + beta * q2y;
    float z = alpha * q1z + beta * q2z;

    // This final adjustment to the quaternion's length corrects for
    // any small constraint error in the inputs q1 and q2 But as you
    // can see, it comes at the cost of 9 additional multiplication
    // operations. If this error-correcting feature is not required,
    // the following code may be removed.
    f1 = 1.5f - 0.5f * (w * w + x * x + y * y + z * z);
    *dstw = w * f1;
    *dstx = x * f1;
    *dsty = y * f1;
    *dstz = z * f1;
}

void Quaternion::slerpForSquad(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& dst)
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    // cos(omega) = q1 * q2;
    // slerp(q1, q2, t) = (q1*sin((1-t)*omega) + q2*sin(t*omega))/sin(omega);
    // q1 = +- q2, slerp(q1,q2,t) = q1.
    // This is a straight-forward implementation of the formula of slerp. It does not do any sign switching.

    auto const tmp = q1.v * q2.v;
    float const c = tmp[0] + tmp[1] + tmp[2] + tmp[3];

    if (std::abs(c) >= 1.0f)
    {
        dst.v = q1.v;
        return;
    }

    float const omega = std::acos(c);
    float const s = std::sqrt(1.0f - c * c);
    if (std::abs(s) < epsi)
    {
        dst.v = q1.v;
        return;
    }

    float const r1 = std::sin((1 - t) * omega) / s;
    float const r2 = std::sin(t * omega) / s;
    dst.v = q1.v * r1 + q2.v * r2;
}

NS_CC_MATH_END
