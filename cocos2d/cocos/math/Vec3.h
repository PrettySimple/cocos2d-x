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

#include <cocos/base/ccMacros.h>
#include <cocos/math/CCMathBase.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>

#include <cmath>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
#    include <limits>
#endif

/**
 * @addtogroup base
 * @{
 */

NS_CC_MATH_BEGIN

/**
 * Defines a 3-element floating point vector.
 *
 * When using a vector to represent a surface normal,
 * the vector should typically be normalized.
 * Other uses of directional vectors may wish to leave
 * the magnitude of the vector intact. When used as a point,
 * the elements of the vector represent a position in 3D space.
 */
class CC_DLL Vec3 final
{
public:
#ifdef __ARM_NEON
    using f32x4_t = __attribute__((neon_vector_type(4))) float;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    using f32x4_t = float[4];
#else
    using f32x4_t = __attribute__((ext_vector_type(4))) float;
#endif

    float x = 0.f;
    float y = 0.f;
    float z = 0.f;

    /**
     * Constructs a new vector initialized to all zeros.
     */
    Vec3() = default;
    Vec3(Vec3 const&) = default;
    Vec3& operator=(Vec3 const&) = default;
    Vec3(Vec3&&) noexcept = default;
    Vec3& operator=(Vec3&&) noexcept = default;
    ~Vec3() = default;

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     * @param zz The z coordinate.
     */
    constexpr Vec3(float xx, float yy, float zz)
    : x(xx)
    , y(yy)
    , z(zz)
    {
    }

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    Vec3(const float* array);

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vec3(const Vec3& p1, const Vec3& p2);

    /**
     * Creates a new vector from an integer interpreted as an RGB value.
     * E.g. 0xff0000 represents red or the vector (1, 0, 0).
     *
     * @param color The integer to interpret as an RGB value.
     *
     * @return A vector corresponding to the interpreted RGB color.
     */
    static Vec3 fromColor(unsigned int color);

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    inline bool isZero() const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(x) < epsi && std::abs(y) < epsi && std::abs(z) < epsi;
#else
        static constexpr auto const zero = f32x4_t{0.f, 0.f, 0.f, 0.f};
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (f32x4_t{x, y, z, 0.f} == zero);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
#endif
    }

    /**
     * Indicates whether this vector contains all ones.
     *
     * @return true if this vector contains all ones, false otherwise.
     */
    inline bool isOne() const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(x - 1.f) < epsi && std::abs(y - 1.f) < epsi && std::abs(z - 1.f) < epsi;
#else
        static constexpr auto const one = f32x4_t{1.f, 1.f, 1.f, 1.f};
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (f32x4_t{x, y, z, 1.f} == one);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
#endif
    }

    /**
     * Returns the angle (in radians) between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The angle between the two vectors (in radians).
     */
    static float angle(const Vec3& v1, const Vec3& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    inline void add(const Vec3& other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    /**
     * Adds the elements of this vector to the specified values.
     *
     * @param xx The add x coordinate.
     * @param yy The add y coordinate.
     * @param zz The add z coordinate.
     */
    inline void add(float xx, float yy, float zz) noexcept
    {
        x += xx;
        y += yy;
        z += zz;
    }

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(const Vec3& v1, const Vec3& v2, Vec3& dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(const Vec3& min, const Vec3& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(const Vec3& v, const Vec3& min, const Vec3& max, Vec3& dst);

    /**
     * Sets this vector to the cross product between itself and the specified vector.
     *
     * @param v The vector to compute the cross product with.
     */
    void cross(const Vec3& v);

    /**
     * Computes the cross product of the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void cross(const Vec3& v1, const Vec3& v2, Vec3& dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     *
     * @return The distance between this vector and v.
     *
     * @see distanceSquared
     */
    float distance(const Vec3& v) const;

    /**
     * Returns the squared distance between this vector and v.
     *
     * When it is not necessary to get the exact distance between
     * two vectors (for example, when simply comparing the
     * distance between different vectors), it is advised to use
     * this method instead of distance.
     *
     * @param v The other vector.
     *
     * @return The squared distance between this vector and v.
     *
     * @see distance
     */
    float distanceSquared(const Vec3& v) const;

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     *
     * @return The dot product.
     */
    float dot(const Vec3& v) const;

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The dot product between the vectors.
     */
    static float dot(const Vec3& v1, const Vec3& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     *
     * @see lengthSquared
     */
    inline float length() const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return std::sqrt(x * x + y * y + z * z);
#else
        auto const mul = f32x4_t{x, y, z, 1.f} * f32x4_t{x, y, z, 1.f};
        return std::sqrt(mul[0] + mul[1] + mul[2]);
#endif
    }

    /**
     * Returns the squared length of this vector.
     *
     * When it is not necessary to get the exact length of a
     * vector (for example, when simply comparing the lengths of
     * different vectors), it is advised to use this method
     * instead of length.
     *
     * @return The squared length of the vector.
     *
     * @see length
     */
    inline float lengthSquared() const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x * x + y * y + z * z;
#else
        auto const mul = f32x4_t{x, y, z, 1.f} * f32x4_t{x, y, z, 1.f};
        return mul[0] + mul[1] + mul[2];
#endif
    }

    /**
     * Negates this vector.
     */
    inline void negate() noexcept
    {
        x = -x;
        y = -y;
        z = -z;
    }

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec3 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     *
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     * Get the normalized vector.
     */
    Vec3 getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    inline void scale(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     * @param zz The new z coordinate.
     */
    inline void set(float xx, float yy, float zz) noexcept
    {
        x = xx;
        y = yy;
        z = zz;
    }

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y, z.
     */
    inline void set(float const* array) noexcept
    {
        GP_ASSERT(array);

        x = array[0];
        y = array[1];
        z = array[2];
    }

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    inline void set(const Vec3& other) noexcept
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    /**
     * Sets this vector to the directional vector between the specified points.
     */
    inline void set(const Vec3& p1, const Vec3& p2) noexcept
    {
        x = p2.x - p1.x;
        y = p2.y - p1.y;
        z = p2.z - p1.z;
    }

    /**
     * Sets the elements of this vector to zero.
     */
    inline void setZero() noexcept
    {
        x = 0.f;
        y = 0.f;
        z = 0.f;
    }

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    inline void subtract(const Vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(const Vec3& v1, const Vec3& v2, Vec3& dst);

    /**
     * Updates this vector towards the given target using a smoothing function.
     * The given response time determines the amount of smoothing (lag). A longer
     * response time yields a smoother result and more lag. To force this vector to
     * follow the target closely, provide a response time that is very small relative
     * to the given elapsed time.
     *
     * @param target target value.
     * @param elapsedTime elapsed time between calls.
     * @param responseTime response time (in the same units as elapsedTime).
     */
    void smooth(const Vec3& target, float elapsedTime, float responseTime);

    /**
     * Linear interpolation between two vectors A and B by alpha which
     * is in the range [0,1]
     */
    inline Vec3 lerp(const Vec3& target, float alpha) const noexcept { return *this * (1.f - alpha) + target * alpha; }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline const Vec3 operator+(const Vec3& v) const noexcept
    {
        Vec3 result(*this);
        result.add(v);
        return result;
    }

    /**
     * Adds the given vector to this vector.
     *
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline Vec3& operator+=(const Vec3& v) noexcept
    {
        add(v);
        return *this;
    }

    /**
     * Calculates the difference of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to subtract.
     * @return The vector difference.
     */
    inline const Vec3 operator-(const Vec3& v) const noexcept
    {
        Vec3 result(*this);
        result.subtract(v);
        return result;
    }

    /**
     * Subtracts the given vector from this vector.
     *
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline Vec3& operator-=(const Vec3& v) noexcept
    {
        subtract(v);
        return *this;
    }

    /**
     * Calculates the negation of this vector.
     *
     * Note: this does not modify this vector.
     *
     * @return The negation of this vector.
     */
    inline const Vec3 operator-() const noexcept
    {
        Vec3 result(*this);
        result.negate();
        return result;
    }

    /**
     * Calculates the scalar product of this vector with the given value.
     *
     * Note: this does not modify this vector.
     *
     * @param s The value to scale by.
     * @return The scaled vector.
     */
    inline const Vec3 operator*(float s) const noexcept
    {
        Vec3 result(*this);
        result.scale(s);
        return result;
    }

    /**
     * Scales this vector by the given value.
     *
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline Vec3& operator*=(float s) noexcept
    {
        scale(s);
        return *this;
    }

    /**
     * Returns the components of this vector divided by the given constant
     *
     * Note: this does not modify this vector.
     *
     * @param s the constant to divide this vector with
     * @return a smaller vector
     */
    inline const Vec3 operator/(float s) const { return Vec3(x / s, y / s, z / s); }

    /** Returns true if the vector's scalar components are all greater
     that the ones of the vector it is compared against.
     */
    inline bool operator<(const Vec3& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x < other.x && y < other.y && z < other.z;
#else
        auto const lt = f32x4_t{x, y, z, 0.f} < f32x4_t{other.x, other.y, other.z, 0.f};
        return lt[0] == -1 && lt[1] == -1 && lt[2] == -1;
#endif
    }

    /** Returns true if the vector's scalar components are all smaller
     that the ones of the vector it is compared against.
     */
    inline bool operator>(const Vec3& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x > other.x && y > other.y && z > other.z;
#else
        auto const gt = f32x4_t{x, y, z, 0.f} < f32x4_t{other.x, other.y, other.z, 0.f};
        return gt[0] == -1 && gt[1] == -1 && gt[2] == -1;
#endif
    }

    /**
     * Determines if this vector is equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    inline bool operator==(const Vec3& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(x - other.x) < epsi && std::abs(y - other.y) < epsi && std::abs(z - other.z) < epsi;
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (f32x4_t{x, y, z, 0.f} == f32x4_t{other.x, other.y, other.z, 0.f});
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1 && eq[2] == -1;
#endif
    }

    /**
     * Determines if this vector is not equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    inline bool operator!=(const Vec3& v) const noexcept { return !operator==(v); }

    /** equals to Vec3(0,0,0) */
    static const Vec3 ZERO;
    /** equals to Vec3(1,1,1) */
    static const Vec3 ONE;
    /** equals to Vec3(1,0,0) */
    static const Vec3 UNIT_X;
    /** equals to Vec3(0,1,0) */
    static const Vec3 UNIT_Y;
    /** equals to Vec3(0,0,1) */
    static const Vec3 UNIT_Z;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 *
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline const Vec3 operator*(float x, const Vec3& v) noexcept
{
    Vec3 result(v);
    result.scale(x);
    return result;
}

// typedef Vec3 Point3;

NS_CC_MATH_END
/**
 end of base group
 @}
 */
