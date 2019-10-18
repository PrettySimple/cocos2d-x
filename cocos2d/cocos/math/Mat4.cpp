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

#include <cocos/math/Mat4.h>

#include <cocos/base/ccMacros.h>
#include <cocos/math/CCMathBase.h>
#include <cocos/math/MathUtil.h>
#include <cocos/math/Quaternion.h>
#include <cocos/math/Vec3.h>
#include <cocos/math/Vec4.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <cmath>
#include <cstring>
#include <limits>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
#    include <algorithm>
#endif

NS_CC_MATH_BEGIN

Mat4::Mat4(const float* mat)
{
    set(mat);
}

void Mat4::createLookAt(const Vec3& eyePosition, const Vec3& targetPosition, const Vec3& up, Mat4& dst)
{
    createLookAt(eyePosition.x, eyePosition.y, eyePosition.z, targetPosition.x, targetPosition.y, targetPosition.z, up.x, up.y, up.z, dst);
}

void Mat4::createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ, float targetPositionX, float targetPositionY, float targetPositionZ,
                        float upX, float upY, float upZ, Mat4& dst)
{
    Vec3 eye(eyePositionX, eyePositionY, eyePositionZ);
    Vec3 target(targetPositionX, targetPositionY, targetPositionZ);
    Vec3 up(upX, upY, upZ);
    up.normalize();

    Vec3 zaxis;
    Vec3::subtract(eye, target, zaxis);
    zaxis.normalize();

    Vec3 xaxis;
    Vec3::cross(up, zaxis, xaxis);
    xaxis.normalize();

    Vec3 yaxis;
    Vec3::cross(zaxis, xaxis, yaxis);
    yaxis.normalize();

#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[0] = xaxis.x;
    dst.m[1] = yaxis.x;
    dst.m[2] = zaxis.x;
    dst.m[3] = 0.0f;

    dst.m[4] = xaxis.y;
    dst.m[5] = yaxis.y;
    dst.m[6] = zaxis.y;
    dst.m[7] = 0.0f;

    dst.m[8] = xaxis.z;
    dst.m[9] = yaxis.z;
    dst.m[10] = zaxis.z;
    dst.m[11] = 0.0f;

    dst.m[12] = -Vec3::dot(xaxis, eye);
    dst.m[13] = -Vec3::dot(yaxis, eye);
    dst.m[14] = -Vec3::dot(zaxis, eye);
    dst.m[15] = 1.0f;
#else
    dst.col[0] = {xaxis.x, yaxis.x, zaxis.x, 0.0f};
    dst.col[1] = {xaxis.y, yaxis.y, zaxis.y, 0.0f};
    dst.col[2] = {xaxis.z, yaxis.z, zaxis.z, 0.0f};
    dst.col[3] = {-Vec3::dot(xaxis, eye), -Vec3::dot(yaxis, eye), -Vec3::dot(zaxis, eye), 1.0f};
#endif
}

void Mat4::createPerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, Mat4& dst)
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    GP_ASSERT(std::abs(zFarPlane - zNearPlane) > epsi);

    float const f_n = 1.0f / (zFarPlane - zNearPlane);
    float const theta = MATH_DEG_TO_RAD(fieldOfView) * 0.5f;
    if (std::abs(std::fmod(theta, MATH_PIOVER2)) < epsi)
    {
        CCLOGERROR("Invalid field of view value (%f) causes attempted calculation tan(%f), which is undefined.", fieldOfView, theta);
        return;
    }
    float const divisor = std::tan(theta);
    GP_ASSERT(divisor);
    float const factor = 1.0f / divisor;

    std::fill_n(dst.m, 16, 0.f);

    GP_ASSERT(aspectRatio);
    dst.m[0] = (1.0f / aspectRatio) * factor;
    dst.m[5] = factor;
    dst.m[10] = (-(zFarPlane + zNearPlane)) * f_n;
    dst.m[11] = -1.0f;
    dst.m[14] = -2.0f * zFarPlane * zNearPlane * f_n;
}

void Mat4::createOrthographic(float width, float height, float zNearPlane, float zFarPlane, Mat4& dst)
{
    float const halfWidth = width / 2.0f;
    float const halfHeight = height / 2.0f;
    createOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
}

void Mat4::createOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, Mat4& dst)
{
    GP_ASSERT(std::abs(right - left) >= std::numeric_limits<float>::epsilon());
    GP_ASSERT(std::abs(top - bottom) >= std::numeric_limits<float>::epsilon());
    GP_ASSERT(std::abs(zFarPlane - zNearPlane) >= std::numeric_limits<float>::epsilon());

    std::fill_n(dst.m, 16, 0.f);

    dst.m[0] = 2.f / (right - left);
    dst.m[5] = 2.f / (top - bottom);
    dst.m[10] = 2.f / (zNearPlane - zFarPlane);

    dst.m[12] = (left + right) / (left - right);
    dst.m[13] = (top + bottom) / (bottom - top);
    dst.m[14] = (zNearPlane + zFarPlane) / (zNearPlane - zFarPlane);
    dst.m[15] = 1.f;
}

void Mat4::createBillboard(const Vec3& objectPosition, const Vec3& cameraPosition, const Vec3& cameraUpVector, Mat4& dst)
{
    createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, nullptr, dst);
}

void Mat4::createBillboard(const Vec3& objectPosition, const Vec3& cameraPosition, const Vec3& cameraUpVector, const Vec3& cameraForwardVector, Mat4& dst)
{
    createBillboardHelper(objectPosition, cameraPosition, cameraUpVector, &cameraForwardVector, dst);
}

void Mat4::createBillboardHelper(const Vec3& objectPosition, const Vec3& cameraPosition, const Vec3& cameraUpVector, const Vec3* cameraForwardVector, Mat4& dst)
{
    Vec3 delta(objectPosition, cameraPosition);
    bool const isSufficientDelta = delta.lengthSquared() > std::numeric_limits<float>::epsilon();

    dst.setIdentity();
    dst.m[3] = objectPosition.x;
    dst.m[7] = objectPosition.y;
    dst.m[11] = objectPosition.z;

    // As per the contracts for the 2 variants of createBillboard, we need
    // either a safe default or a sufficient distance between object and camera.
    if (cameraForwardVector || isSufficientDelta)
    {
        Vec3 target = isSufficientDelta ? cameraPosition : (objectPosition - *cameraForwardVector);

        // A billboard is the inverse of a lookAt rotation
        Mat4 lookAt;
        createLookAt(objectPosition, target, cameraUpVector, lookAt);
        dst.m[0] = lookAt.m[0];
        dst.m[1] = lookAt.m[4];
        dst.m[2] = lookAt.m[8];
        dst.m[4] = lookAt.m[1];
        dst.m[5] = lookAt.m[5];
        dst.m[6] = lookAt.m[9];
        dst.m[8] = lookAt.m[2];
        dst.m[9] = lookAt.m[6];
        dst.m[10] = lookAt.m[10];
    }
}

// void Mat4::createReflection(const Plane& plane, Mat4* dst)
// {
//     Vec3 normal(plane.getNormal());
//     float k = -2.0f * plane.getDistance();

//     dst->setIdentity();

//     dst->m[0] -= 2.0f * normal.x * normal.x;
//     dst->m[5] -= 2.0f * normal.y * normal.y;
//     dst->m[10] -= 2.0f * normal.z * normal.z;
//     dst->m[1] = dst->m[4] = -2.0f * normal.x * normal.y;
//     dst->m[2] = dst->m[8] = -2.0f * normal.x * normal.z;
//     dst->m[6] = dst->m[9] = -2.0f * normal.y * normal.z;

//     dst->m[3] = k * normal.x;
//     dst->m[7] = k * normal.y;
//     dst->m[11] = k * normal.z;
// }

void Mat4::createScale(const Vec3& scale, Mat4& dst)
{
    dst.setIdentity();

    dst.m[0] = scale.x;
    dst.m[5] = scale.y;
    dst.m[10] = scale.z;
}

void Mat4::createScale(float xScale, float yScale, float zScale, Mat4& dst)
{
    dst.setIdentity();

    dst.m[0] = xScale;
    dst.m[5] = yScale;
    dst.m[10] = zScale;
}

void Mat4::createRotation(const Quaternion& q, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    float const x2 = q.x + q.x;
    float const y2 = q.y + q.y;
    float const z2 = q.z + q.z;

    float const xx2 = q.x * x2;
    float const yy2 = q.y * y2;
    float const zz2 = q.z * z2;
    float const xy2 = q.x * y2;
    float const xz2 = q.x * z2;
    float const yz2 = q.y * z2;
    float const wx2 = q.w * x2;
    float const wy2 = q.w * y2;
    float const wz2 = q.w * z2;

    dst.m[0] = 1.0f - yy2 - zz2;
    dst.m[1] = xy2 + wz2;
    dst.m[2] = xz2 - wy2;
    dst.m[3] = 0.0f;

    dst.m[4] = xy2 - wz2;
    dst.m[5] = 1.0f - xx2 - zz2;
    dst.m[6] = yz2 + wx2;
    dst.m[7] = 0.0f;

    dst.m[8] = xz2 + wy2;
    dst.m[9] = yz2 - wx2;
    dst.m[10] = 1.0f - xx2 - yy2;
    dst.m[11] = 0.0f;

    dst.m[12] = 0.0f;
    dst.m[13] = 0.0f;
    dst.m[14] = 0.0f;
    dst.m[15] = 1.0f;
#else
    dst.col[0] = f32x4_t{1.f, 0.f, 0.f, 0.f} +
        f32x4_t{-2.f, 2.f, 2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 1, 0, 0, 0) * __builtin_shufflevector(q.v, q.v, 1, 1, 2, 2) +
        f32x4_t{-2.f, 2.f, -2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 2, 3, 3, 3) * __builtin_shufflevector(q.v, q.v, 2, 2, 1, 1);
    dst.col[0][3] = 0.f;

    dst.col[1] = f32x4_t{0.f, 1.f, 0.f, 0.f} +
        f32x4_t{2.f, -2.f, 2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 0, 0, 1, 1) * __builtin_shufflevector(q.v, q.v, 1, 0, 2, 2) +
        f32x4_t{-2.f, -2.f, 2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 3, 2, 3, 3) * __builtin_shufflevector(q.v, q.v, 2, 2, 0, 0);
    dst.col[1][3] = 0.f;

    dst.col[2] = f32x4_t{0.f, 0.f, 1.f, 0.f} +
        f32x4_t{2.f, 2.f, -2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 0, 1, 0, 0) * __builtin_shufflevector(q.v, q.v, 2, 2, 0, 0) +
        f32x4_t{2.f, -2.f, -2.f, 0.f} * __builtin_shufflevector(q.v, q.v, 3, 3, 1, 1) * __builtin_shufflevector(q.v, q.v, 1, 0, 1, 1);
    dst.col[2][3] = 0.f;

    dst.col[3] = {0.f, 0.f, 0.f, 1.f};
#endif
}

void Mat4::createRotation(const Vec3& axis, float angle, Mat4& dst)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    // Make sure the input axis is normalized.
    float n = x * x + y * y + z * z;
    if (std::abs(n - 1.0f) > epsi)
    {
        // Not normalized.
        n = std::sqrt(n);
        // Prevent divide too close to zero.
        if (std::abs(n) > epsi)
        {
            n = 1.0f / n;
            x *= n;
            y *= n;
            z *= n;
        }
    }

    float const c = std::cos(angle);
    float const s = std::sin(angle);

    float const t = 1.0f - c;
    float const tx = t * x;
    float const ty = t * y;
    float const tz = t * z;
    float const txy = tx * y;
    float const txz = tx * z;
    float const tyz = ty * z;
    float const sx = s * x;
    float const sy = s * y;
    float const sz = s * z;

    dst.m[0] = c + tx * x;
    dst.m[1] = txy + sz;
    dst.m[2] = txz - sy;
    dst.m[3] = 0.0f;

    dst.m[4] = txy - sz;
    dst.m[5] = c + ty * y;
    dst.m[6] = tyz + sx;
    dst.m[7] = 0.0f;

    dst.m[8] = txz + sy;
    dst.m[9] = tyz - sx;
    dst.m[10] = c + tz * z;
    dst.m[11] = 0.0f;

    dst.m[12] = 0.0f;
    dst.m[13] = 0.0f;
    dst.m[14] = 0.0f;
    dst.m[15] = 1.0f;
#else
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    auto a = f32x4_t{axis.x, axis.y, axis.z, 0.f};
    auto const square_axis = a * a;

    // Make sure the input axis is normalized.
    float n = square_axis[0] + square_axis[1] + square_axis[2];
    if (std::abs(n - 1.0f) > epsi)
    {
        // Not normalized.
        n = std::sqrt(n);
        // Prevent divide too close to zero.
        if (std::abs(n) > epsi)
        {
            n = 1.0f / n;
            a *= n;
        }
    }

    float const c = std::cos(angle);
    float const s = std::sin(angle);
    float const t = 1.0f - c;

    dst.col[0] = f32x4_t{c, 0.f, 0.f, 0.f} + t * __builtin_shufflevector(a, a, 0, 0, 0, 0) * a + f32x4_t{0.f, s * a[2], -s * a[1], 0.f};
    dst.col[0][3] = 0.f;

    dst.col[1] = f32x4_t{0.f, c, 0.f, 0.f} + t * __builtin_shufflevector(a, a, 0, 1, 1, 1) * __builtin_shufflevector(a, a, 1, 1, 3, 4) +
        f32x4_t{-s * a[2], 0.f, s * a[0], 0.f};
    dst.col[1][3] = 0.f;

    dst.col[1] = f32x4_t{0.f, 0.f, c, 0.f} + t * __builtin_shufflevector(a, a, 0, 1, 2, 2) * __builtin_shufflevector(a, a, 2, 2, 2, 2) +
        f32x4_t{s * a[1], -s * a[0], 0.f, 0.f};
    dst.col[1][3] = 0.f;

    dst.col[3] = {0.f, 0.f, 0.f, 1.f};
#endif
}

void Mat4::createRotationX(float angle, Mat4& dst)
{
    dst.setIdentity();

    float const c = std::cos(angle);
    float const s = std::sin(angle);

    dst.col[1][1] = c;
    dst.col[1][2] = s;
    dst.col[2][1] = -s;
    dst.col[2][2] = c;
}

void Mat4::createRotationY(float angle, Mat4& dst)
{
    dst.setIdentity();

    float const c = std::cos(angle);
    float const s = std::sin(angle);

    dst.col[0][0] = c;
    dst.col[0][2] = -s;
    dst.col[2][0] = c;
    dst.col[2][2] = -s;

    dst.m[0] = c;
    dst.m[2] = -s;
    dst.m[8] = s;
    dst.m[10] = c;
}

void Mat4::createRotationZ(float angle, Mat4& dst)
{
    dst.setIdentity();

    float c = std::cos(angle);
    float s = std::sin(angle);

    dst.col[0][0] = c;
    dst.col[0][1] = s;
    dst.col[1][0] = -s;
    dst.col[1][1] = c;
}

void Mat4::createTranslation(const Vec3& translation, Mat4& dst)
{
    dst.setIdentity();

#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[12] = translation.x;
    dst.m[13] = translation.y;
    dst.m[14] = translation.z;
#else
    dst.col[3] = {translation.x, translation.y, translation.z, 1.f};
#endif
}

void Mat4::createTranslation(float xTranslation, float yTranslation, float zTranslation, Mat4& dst)
{
    dst.setIdentity();

#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    dst.m[12] = xTranslation;
    dst.m[13] = yTranslation;
    dst.m[14] = zTranslation;
#else
    dst.col[3] = {xTranslation, yTranslation, zTranslation, 1.f};
#endif
}

void Mat4::add(float scalar)
{
    add(scalar, *this);
}

void Mat4::add(float scalar, Mat4& dst)
{
    addMatrix(*this, scalar, dst);
}

void Mat4::add(const Mat4& mat)
{
    add(*this, mat, *this);
}

void Mat4::add(const Mat4& m1, const Mat4& m2, Mat4& dst)
{
    addMatrix(m1, m2, dst);
}

bool Mat4::decompose(Vec3* scale, Quaternion* rotation, Vec3* translation) const
{
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
    if (translation != nullptr)
    {
        // Extract the translation.
        translation->x = col[3][0];
        translation->y = col[3][1];
        translation->z = col[3][2];
    }

    // Nothing left to do.
    if (scale == nullptr && rotation == nullptr)
        return true;

    // Extract the scale.
    // This is simply the length of each axis (row/column) in the matrix.
    Vec3 xaxis(m[0], m[1], m[2]);
    float const scaleX = xaxis.length();

    Vec3 yaxis(m[4], m[5], m[6]);
    float const scaleY = yaxis.length();

    Vec3 zaxis(m[8], m[9], m[10]);
    float scaleZ = zaxis.length();

    // Determine if we have a negative scale (true if determinant is less than zero).
    // In this case, we simply negate a single axis of the scale.
    float const det = determinant();
    if (det < 0.f)
        scaleZ = -scaleZ;

    if (scale != nullptr)
    {
        scale->x = scaleX;
        scale->y = scaleY;
        scale->z = scaleZ;
    }

    // Nothing left to do.
    if (rotation == nullptr)
        return true;

    // Scale too close to zero, can't decompose rotation.
    if (std::abs(scaleX) < epsi || std::abs(scaleY) < epsi || std::abs(scaleZ) < epsi)
        return false;

    // Factor the scale out of the matrix axes.
    float rn = 1.0f / scaleX;
    xaxis.x *= rn;
    xaxis.y *= rn;
    xaxis.z *= rn;

    rn = 1.0f / scaleY;
    yaxis.x *= rn;
    yaxis.y *= rn;
    yaxis.z *= rn;

    rn = 1.0f / scaleZ;
    zaxis.x *= rn;
    zaxis.y *= rn;
    zaxis.z *= rn;

    // Now calculate the rotation from the resulting matrix (axes).
    float const trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    if (std::abs(trace) > epsi)
    {
        float s = 0.5f / std::sqrt(trace);
        rotation->w = 0.25f / s;
        rotation->x = (yaxis.z - zaxis.y) * s;
        rotation->y = (zaxis.x - xaxis.z) * s;
        rotation->z = (xaxis.y - yaxis.x) * s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized,
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
        {
            float s = 0.5f / std::sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
            rotation->w = (yaxis.z - zaxis.y) * s;
            rotation->x = 0.25f / s;
            rotation->y = (yaxis.x + xaxis.y) * s;
            rotation->z = (zaxis.x + xaxis.z) * s;
        }
        else if (yaxis.y > zaxis.z)
        {
            float s = 0.5f / std::sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
            rotation->w = (zaxis.x - xaxis.z) * s;
            rotation->x = (yaxis.x + xaxis.y) * s;
            rotation->y = 0.25f / s;
            rotation->z = (zaxis.y + yaxis.z) * s;
        }
        else
        {
            float s = 0.5f / std::sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
            rotation->w = (xaxis.y - yaxis.x) * s;
            rotation->x = (zaxis.x + xaxis.z) * s;
            rotation->y = (zaxis.y + yaxis.z) * s;
            rotation->z = 0.25f / s;
        }
    }
#else
    if (std::abs(trace) > epsi)
    {
        float const s = 0.5f / std::sqrt(trace);
        rotation->v = (f32x4_t{yaxis.z, zaxis.x, xaxis.y, 0.f} - f32x4_t{zaxis.y, xaxis.z, yaxis.x, 0.f}) * s;
        rotation->v[3] = 0.25f / s;
    }
    else
    {
        // Note: since xaxis, yaxis, and zaxis are normalized,
        // we will never divide by zero in the code below.
        if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
        {
            float const s = 0.5f / std::sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
            rotation->v = (f32x4_t{0.f, yaxis.x, zaxis.x, yaxis.z} + f32x4_t{0.f, xaxis.y, xaxis.z, -zaxis.y}) * s;
            rotation->v[0] = 0.25f / s;
        }
        else if (yaxis.y > zaxis.z)
        {
            float const s = 0.5f / std::sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
            rotation->v = (f32x4_t{yaxis.x, 0.f, zaxis.y, zaxis.x} + f32x4_t{xaxis.y, 0.f, yaxis.z, -xaxis.z}) * s;
            rotation->v[1] = 0.25f / s;
        }
        else
        {
            float const s = 0.5f / std::sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
            rotation->v = (f32x4_t{zaxis.x, zaxis.y, 0.f, xaxis.y} + f32x4_t{xaxis.z, yaxis.z, 0.f, -yaxis.x}) * s;
            rotation->v[2] = 0.25f / s;
        }
    }
#endif

    return true;
}

float Mat4::determinant() const
{
    float a0 = m[0] * m[5] - m[1] * m[4];
    float a1 = m[0] * m[6] - m[2] * m[4];
    float a2 = m[0] * m[7] - m[3] * m[4];
    float a3 = m[1] * m[6] - m[2] * m[5];
    float a4 = m[1] * m[7] - m[3] * m[5];
    float a5 = m[2] * m[7] - m[3] * m[6];
    float b0 = m[8] * m[13] - m[9] * m[12];
    float b1 = m[8] * m[14] - m[10] * m[12];
    float b2 = m[8] * m[15] - m[11] * m[12];
    float b3 = m[9] * m[14] - m[10] * m[13];
    float b4 = m[9] * m[15] - m[11] * m[13];
    float b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the determinant.
    return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
}

void Mat4::getScale(Vec3* scale) const
{
    decompose(scale, nullptr, nullptr);
}

bool Mat4::getRotation(Quaternion* rotation) const
{
    return decompose(nullptr, rotation, nullptr);
}

void Mat4::getTranslation(Vec3* translation) const
{
    decompose(nullptr, nullptr, translation);
}

void Mat4::getUpVector(Vec3& dst) const
{
    dst.x = col[1][0];
    dst.y = col[1][1];
    dst.z = col[1][2];
}

void Mat4::getDownVector(Vec3& dst) const
{
    dst.x = -col[1][0];
    dst.y = -col[1][1];
    dst.z = -col[1][2];
}

void Mat4::getLeftVector(Vec3& dst) const
{
    dst.x = -col[0][0];
    dst.y = -col[0][1];
    dst.z = -col[0][2];
}

void Mat4::getRightVector(Vec3& dst) const
{
    dst.x = col[0][0];
    dst.y = col[0][1];
    dst.z = col[0][2];
}

void Mat4::getForwardVector(Vec3& dst) const
{
    dst.x = -col[2][0];
    dst.y = -col[2][1];
    dst.z = -col[2][2];
}

void Mat4::getBackVector(Vec3& dst) const
{
    dst.x = col[2][0];
    dst.y = col[2][1];
    dst.z = col[2][2];
}

Mat4 Mat4::getInversed() const
{
    Mat4 mat(*this);
    mat.inverse();
    return mat;
}

bool Mat4::inverse()
{
    float const a0 = m[0] * m[5] - m[1] * m[4];
    float const a1 = m[0] * m[6] - m[2] * m[4];
    float const a2 = m[0] * m[7] - m[3] * m[4];
    float const a3 = m[1] * m[6] - m[2] * m[5];
    float const a4 = m[1] * m[7] - m[3] * m[5];
    float const a5 = m[2] * m[7] - m[3] * m[6];
    float const b0 = m[8] * m[13] - m[9] * m[12];
    float const b1 = m[8] * m[14] - m[10] * m[12];
    float const b2 = m[8] * m[15] - m[11] * m[12];
    float const b3 = m[9] * m[14] - m[10] * m[13];
    float const b4 = m[9] * m[15] - m[11] * m[13];
    float const b5 = m[10] * m[15] - m[11] * m[14];

    // Calculate the determinant.
    float const det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

    // Close to zero, can't invert.
    if (std::abs(det) < std::numeric_limits<float>::epsilon())
        return false;

    // Support the case where m == dst.
    Mat4 inverse;
    inverse.m[0] = m[5] * b5 - m[6] * b4 + m[7] * b3;
    inverse.m[1] = -m[1] * b5 + m[2] * b4 - m[3] * b3;
    inverse.m[2] = m[13] * a5 - m[14] * a4 + m[15] * a3;
    inverse.m[3] = -m[9] * a5 + m[10] * a4 - m[11] * a3;

    inverse.m[4] = -m[4] * b5 + m[6] * b2 - m[7] * b1;
    inverse.m[5] = m[0] * b5 - m[2] * b2 + m[3] * b1;
    inverse.m[6] = -m[12] * a5 + m[14] * a2 - m[15] * a1;
    inverse.m[7] = m[8] * a5 - m[10] * a2 + m[11] * a1;

    inverse.m[8] = m[4] * b4 - m[5] * b2 + m[7] * b0;
    inverse.m[9] = -m[0] * b4 + m[1] * b2 - m[3] * b0;
    inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
    inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

    inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
    inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
    inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
    inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

    multiply(inverse, 1.0f / det, *this);

    return true;
}

bool Mat4::isIdentity() const
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    static constexpr auto const epsi = std::numeric_limits<float>::epsilon();

    return std::abs(m[0] - 1.f) < epsi && std::abs(m[1]) < epsi && std::abs(m[2]) < epsi && std::abs(m[3]) < epsi && std::abs(m[4]) < epsi &&
        std::abs(m[5] - 1.f) < epsi && std::abs(m[6]) < epsi && std::abs(m[7]) < epsi && std::abs(m[8]) < epsi && std::abs(m[9]) < epsi &&
        std::abs(m[10] - 1.f) < epsi && std::abs(m[11]) < epsi && std::abs(m[12]) < epsi && std::abs(m[13]) < epsi && std::abs(m[14]) < epsi &&
        std::abs(m[15] - 1.f) < epsi;
#else
    static constexpr f32x4x4_t const identity = {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}};
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
    i32x4x4_t const eq = {identity[0] == col[0], identity[1] == col[1], identity[2] == col[2], identity[3] == col[3]};
#    pragma clang diagnostic pop

    return eq[0][0] == -1 && eq[0][1] == -1 && eq[0][2] == -1 && eq[0][3] == -1 && eq[1][0] == -1 && eq[1][1] == -1 && eq[1][2] == -1 && eq[1][3] == -1 &&
        eq[2][0] == -1 && eq[2][1] == -1 && eq[2][2] == -1 && eq[2][3] == -1 && eq[3][0] == -1 && eq[3][1] == -1 && eq[3][2] == -1 && eq[3][3] == -1;
#endif
}

void Mat4::multiply(float scalar)
{
    multiply(scalar, *this);
}

void Mat4::multiply(float scalar, Mat4& dst) const
{
    multiply(*this, scalar, dst);
}

void Mat4::multiply(const Mat4& m, float scalar, Mat4& dst)
{
    multiplyMatrix(m.m, scalar, dst);
}

void Mat4::multiply(const Mat4& mat)
{
    multiply(*this, mat, *this);
}

void Mat4::multiply(const Mat4& m1, const Mat4& m2, Mat4& dst)
{
    multiplyMatrix(m1, m2, dst);
}

void Mat4::negate()
{
    negateMatrix(*this, *this);
}

Mat4 Mat4::getNegated() const
{
    Mat4 mat(*this);
    mat.negate();
    return mat;
}

void Mat4::rotate(const Quaternion& q)
{
    rotate(q, *this);
}

void Mat4::rotate(const Quaternion& q, Mat4& dst) const
{
    Mat4 r;
    createRotation(q, r);
    multiply(*this, r, dst);
}

void Mat4::rotate(const Vec3& axis, float angle)
{
    rotate(axis, angle, *this);
}

void Mat4::rotate(const Vec3& axis, float angle, Mat4& dst) const
{
    Mat4 r;
    createRotation(axis, angle, r);
    multiply(*this, r, dst);
}

void Mat4::rotateX(float angle)
{
    rotateX(angle, *this);
}

void Mat4::rotateX(float angle, Mat4& dst) const
{
    Mat4 r;
    createRotationX(angle, r);
    multiply(*this, r, dst);
}

void Mat4::rotateY(float angle)
{
    rotateY(angle, *this);
}

void Mat4::rotateY(float angle, Mat4& dst) const
{
    Mat4 r;
    createRotationY(angle, r);
    multiply(*this, r, dst);
}

void Mat4::rotateZ(float angle)
{
    rotateZ(angle, *this);
}

void Mat4::rotateZ(float angle, Mat4& dst) const
{
    Mat4 r;
    createRotationZ(angle, r);
    multiply(*this, r, dst);
}

void Mat4::scale(float value)
{
    scale(value, *this);
}

void Mat4::scale(float value, Mat4& dst) const
{
    scale(value, value, value, dst);
}

void Mat4::scale(float xScale, float yScale, float zScale)
{
    scale(xScale, yScale, zScale, *this);
}

void Mat4::scale(float xScale, float yScale, float zScale, Mat4& dst) const
{
    Mat4 s;
    createScale(xScale, yScale, zScale, s);
    multiply(*this, s, dst);
}

void Mat4::scale(const Vec3& s)
{
    scale(s.x, s.y, s.z, *this);
}

void Mat4::scale(const Vec3& s, Mat4& dst) const
{
    scale(s.x, s.y, s.z, dst);
}

void Mat4::set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41,
               float m42, float m43, float m44)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    m[0] = m11;
    m[1] = m21;
    m[2] = m31;
    m[3] = m41;
    m[4] = m12;
    m[5] = m22;
    m[6] = m32;
    m[7] = m42;
    m[8] = m13;
    m[9] = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
#else
    col[0] = {m11, m21, m31, m41};
    col[1] = {m12, m22, m32, m42};
    col[2] = {m13, m23, m33, m43};
    col[3] = {m14, m24, m34, m44};
#endif
}

void Mat4::set(const float* mat)
{
    GP_ASSERT(mat);
    std::memcpy(m, mat, sizeof(float[16]));
}

void Mat4::set(const Mat4& mat)
{
    std::memcpy(m, mat.m, sizeof(float[16]));
}

void Mat4::setIdentity()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    std::fill_n(m, 16, 0.f);
    m[0] = 1.0f;
    m[5] = 1.f;
    m[10] = 1.f;
    m[15] = 1.f;
#else
    col[0] = {1.0f, 0.0f, 0.0f, 0.0f};
    col[1] = {0.0f, 1.0f, 0.0f, 0.0f};
    col[2] = {0.0f, 0.0f, 1.0f, 0.0f};
    col[3] = {0.0f, 0.0f, 0.0f, 1.0f};
#endif
}

void Mat4::setZero()
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    std::fill_n(m, 16, 0.f);
#else
    static constexpr auto const zero = Mat4::f32x4_t{0.f, 0.f, 0.f, 0.f};
    col[0] = zero;
    col[1] = zero;
    col[2] = zero;
    col[3] = zero;
#endif
}

void Mat4::subtract(const Mat4& mat)
{
    subtract(*this, mat, *this);
}

void Mat4::subtract(const Mat4& m1, const Mat4& m2, Mat4& dst)
{
    subtractMatrix(m1, m2, dst);
}

void Mat4::transformVector(Vec3& vector) const
{
    transformVector(vector.x, vector.y, vector.z, 0.0f, vector);
}

void Mat4::transformVector(const Vec3& vector, Vec3& dst) const
{
    transformVector(vector.x, vector.y, vector.z, 0.0f, dst);
}

void Mat4::transformVector(float x, float y, float z, float w, Vec3& dst) const
{
    transformVec4(*this, x, y, z, w, dst);
}

void Mat4::transformVector(Vec4& vector) const
{
    transformVector(vector, vector);
}

void Mat4::transformVector(const Vec4& vector, Vec4& dst) const
{
    transformVec4(*this, vector, dst);
}

void Mat4::translate(float x, float y, float z)
{
    translate(x, y, z, *this);
}

void Mat4::translate(float x, float y, float z, Mat4& dst) const
{
    Mat4 t;
    createTranslation(x, y, z, t);
    multiply(*this, t, dst);
}

void Mat4::translate(const Vec3& t)
{
    translate(t.x, t.y, t.z, *this);
}

void Mat4::translate(const Vec3& t, Mat4& dst) const
{
    translate(t.x, t.y, t.z, dst);
}

void Mat4::transpose()
{
    transposeMatrix(*this, *this);
}

Mat4 Mat4::getTransposed() const
{
    Mat4 mat(*this);
    mat.transpose();
    return mat;
}

const Mat4 Mat4::IDENTITY = Mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

const Mat4 Mat4::ZERO = Mat4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

NS_CC_MATH_END
