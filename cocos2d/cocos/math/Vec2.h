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
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>

#include <cmath>
#include <functional>
#include <type_traits>

#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
#    include <limits>
#endif

/**
 * @addtogroup base
 * @{
 */

NS_CC_MATH_BEGIN

/** Clamp a value between from and to.
 */

inline float clampf(float value, float min_inclusive, float max_inclusive)
{
    if (min_inclusive > max_inclusive)
    {
        std::swap(min_inclusive, max_inclusive);
    }
    return value < min_inclusive ? min_inclusive : value < max_inclusive ? value : max_inclusive;
}

class Mat4;

/**
 * Defines a 2-element floating point vector.
 */
class CC_DLL Vec2 final
{
public:
#if defined(__ARM_NEON)
    using f32x2_t = __attribute__((neon_vector_type(2))) float;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    using f32x2_t = float[2];
#else
    using f32x2_t = __attribute__((ext_vector_type(2))) float;
#endif

public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
    union
    {
        f32x2_t v = {0.f, 0.f};
        struct
        {
            float x;
            float y;
        };
    };
#pragma clang diagnostic pop

    /**
     * Constructs a new vector initialized to all zeros.
     */

    Vec2() = default;
    Vec2(Vec2 const&) = default;
    Vec2& operator=(Vec2 const&) = default;
    Vec2(Vec2&&) noexcept = default;
    Vec2& operator=(Vec2&&) noexcept = default;
    ~Vec2() = default;

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     */
    constexpr Vec2(float xx, float yy)
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    : x(xx)
    , y(yy)
#else
    : v
    {
        xx, yy
    }
#endif
    {
    }

#if CC_TARGET_PLATFORM != CC_PLATFORM_EMSCRIPTEN
    constexpr Vec2(f32x2_t&& other)
    : v(std::move(other))
    {
    }
#endif

    /**
     * Constructs a new vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    Vec2(float const* array);

    /**
     * Constructs a vector that describes the direction between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    Vec2(Vec2 const& p1, Vec2 const& p2);

    /**
     * Indicates whether this vector contains all zeros.
     *
     * @return true if this vector contains all zeros, false otherwise.
     */
    inline bool isZero() const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(x) < epsi && std::abs(y) < epsi;
#else
        static constexpr auto const zero = f32x2_t{0.f, 0.f};
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (v == zero);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1;
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
        return std::abs(x - 1.f) < epsi && std::abs(y - 1.f) < epsi;
#else
        static constexpr auto const one = f32x2_t{1.f, 1.f};
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (v == one);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1;
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
    static float angle(Vec2 const& v1, Vec2 const& v2);

    /**
     * Adds the elements of the specified vector to this one.
     *
     * @param v The vector to add.
     */
    inline void add(Vec2 const& other) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x += other.x;
        y += other.y;
#else
        v += other.v;
#endif
    }

    /**
     * Adds the specified vectors and stores the result in dst.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst A vector to store the result in.
     */
    static void add(Vec2 const& v1, Vec2 const& v2, Vec2* dst);

    /**
     * Clamps this vector within the specified range.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     */
    void clamp(Vec2 const& min, Vec2 const& max);

    /**
     * Clamps the specified vector within the specified range and returns it in dst.
     *
     * @param v The vector to clamp.
     * @param min The minimum value.
     * @param max The maximum value.
     * @param dst A vector to store the result in.
     */
    static void clamp(Vec2 const& v, Vec2 const& min, Vec2 const& max, Vec2* dst);

    /**
     * Returns the distance between this vector and v.
     *
     * @param v The other vector.
     *
     * @return The distance between this vector and v.
     *
     * @see distanceSquared
     */
    float distance(Vec2 const& v) const;

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
    inline float distanceSquared(Vec2 const& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        auto const a = other.x - x;
        auto const b = other.y - y;
        return a * a + b * b;
#else
        auto const diff = other.v - v;
        auto const mul = diff * diff;
        return mul[0] + mul[1];
#endif
    }

    /**
     * Returns the dot product of this vector and the specified vector.
     *
     * @param v The vector to compute the dot product with.
     *
     * @return The dot product.
     */
    inline float dot(Vec2 const& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x * other.x + y * other.y;
#else
        auto const mul = v * other.v;
        return mul[0] + mul[1];
#endif
    }

    /**
     * Returns the dot product between the specified vectors.
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     *
     * @return The dot product between the vectors.
     */
    static float dot(Vec2 const& v1, Vec2 const& v2);

    /**
     * Computes the length of this vector.
     *
     * @return The length of the vector.
     *
     * @see lengthSquared
     */
    float length() const;

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
    inline float lengthSquared() const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x * x + y * y;
#else
        auto const mul = v * v;
        return mul[0] + mul[1];
#endif
    }

    /**
     * Negates this vector.
     */
    inline void negate() noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = -x;
        y = -y;
#else
        v = -v;
#endif
    }

    /**
     * Normalizes this vector.
     *
     * This method normalizes this Vec2 so that it is of
     * unit length (in other words, the length of the vector
     * after calling this method will be 1.0f). If the vector
     * already has unit length or if the length of the vector
     * is zero, this method does nothing.
     *
     * @return This vector, after the normalization occurs.
     */
    void normalize();

    /**
     Get the normalized vector.
     */
    Vec2 getNormalized() const;

    /**
     * Scales all elements of this vector by the specified value.
     *
     * @param scalar The scalar value.
     */
    inline void scale(float scalar) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x *= scalar;
        y *= scalar;
#else
        v *= scalar;
#endif
    }

    /**
     * Scales each element of this vector by the matching component of scale.
     *
     * @param scale The vector to scale by.
     */
    inline void scale(Vec2 const& scale) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x *= scale.x;
        y *= scale.y;
#else
        v *= scale.v;
#endif
    }

    /**
     * Rotates this vector by angle (specified in radians) around the given point.
     *
     * @param point The point to rotate around.
     * @param angle The angle to rotate by (in radians).
     */
    void rotate(Vec2 const& point, float angle);

    /**
     * Sets the elements of this vector to the specified values.
     *
     * @param xx The new x coordinate.
     * @param yy The new y coordinate.
     */
    inline void set(float xx, float yy) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = xx;
        y = yy;
#else
        v = {xx, yy};
#endif
    }

    /**
     * Sets the elements of this vector from the values in the specified array.
     *
     * @param array An array containing the elements of the vector in the order x, y.
     */
    void set(float const* array);

    /**
     * Sets the elements of this vector to those in the specified vector.
     *
     * @param v The vector to copy.
     */
    inline void set(Vec2 const& other) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = other.x;
        y = other.y;
#else
        v = other.v;
#endif
    }

    /**
     * Sets this vector to the directional vector between the specified points.
     *
     * @param p1 The first point.
     * @param p2 The second point.
     */
    inline void set(Vec2 const& p1, Vec2 const& p2) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = p2.x - p1.x;
        y = p2.y - p1.y;
#else
        v = p2.v - p1.v;
#endif
    }

    /**
     * Sets the elements of this vector to zero.
     */
    inline void setZero() noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = 0.f;
        y = 0.f;
#else
        v = {0.0f, 0.f};
#endif
    }

    /**
     * Subtracts this vector and the specified vector as (this - v)
     * and stores the result in this vector.
     *
     * @param v The vector to subtract.
     */
    inline void subtract(Vec2 const& other) noexcept
    {
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
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = x - other.x;
        y = y - other.y;
#else
        v -= other.v;
#endif
    }

    /**
     * Subtracts the specified vectors and stores the result in dst.
     * The resulting vector is computed as (v1 - v2).
     *
     * @param v1 The first vector.
     * @param v2 The second vector.
     * @param dst The destination vector.
     */
    static void subtract(Vec2 const& v1, Vec2 const& v2, Vec2* dst);

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
    inline void smooth(Vec2 const& target, float elapsedTime, float responseTime) noexcept
    {
        if (elapsedTime > 0.f)
        {
            *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }
    }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline Vec2 const operator+(Vec2 const& v) const noexcept
    {
        Vec2 result(*this);
        result.add(v);
        return result;
    }

    /**
     * Adds the given vector to this vector.
     *
     * @param v The vector to add.
     * @return This vector, after the addition occurs.
     */
    inline Vec2& operator+=(Vec2 const& v) noexcept
    {
        add(v);
        return *this;
    }

    /**
     * Calculates the sum of this vector with the given vector.
     *
     * Note: this does not modify this vector.
     *
     * @param v The vector to add.
     * @return The vector sum.
     */
    inline Vec2 const operator-(Vec2 const& v) const noexcept
    {
        Vec2 result(*this);
        result.subtract(v);
        return result;
    }

    /**
     * Subtracts the given vector from this vector.
     *
     * @param v The vector to subtract.
     * @return This vector, after the subtraction occurs.
     */
    inline Vec2& operator-=(Vec2 const& v) noexcept
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
    inline Vec2 const operator-() const noexcept
    {
        Vec2 result(*this);
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
    inline Vec2 const operator*(float s) const noexcept
    {
        Vec2 result(*this);
        result.scale(s);
        return result;
    }

    /**
     * Scales this vector by the given value.
     *
     * @param s The value to scale by.
     * @return This vector, after the scale occurs.
     */
    inline Vec2& operator*=(float s) noexcept
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
    inline Vec2 const operator/(float s) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(x / s, y / s);
#else
        return Vec2(v / s);
#endif
    }

    /**
     * Determines if this vector is less than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is less than the given vector, false otherwise.
     */
    inline bool operator<(Vec2 const& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x < other.x && y < other.y;
#else
        auto const lt = v < other.v;
        return lt[0] == -1 && lt[1] == -1;
#endif
    }

    /**
     * Determines if this vector is greater than the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is greater than the given vector, false otherwise.
     */
    inline bool operator>(Vec2 const& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return x > other.x && y > other.y;
#else
        auto const gt = v > other.v;
        return gt[0] == -1 && gt[1] == -1;
#endif
    }

    /**
     * Determines if this vector is equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is equal to the given vector, false otherwise.
     */
    inline bool operator==(Vec2 const& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(x - other.x) < epsi && std::abs(y - other.y) < epsi;
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (v == other.v);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1;
#endif
    }

    /**
     * Determines if this vector is not equal to the given vector.
     *
     * @param v The vector to compare against.
     *
     * @return True if this vector is not equal to the given vector, false otherwise.
     */
    inline bool operator!=(Vec2 const& v) const noexcept { return !operator==(v); }

    // code added compatible for Point
public:
    /**
     * @js NA
     * @lua NA
     */
    inline void setPoint(float xx, float yy) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        x = xx;
        y = yy;
#else
        v = {xx, yy};
#endif
    }
    /**
     * @js NA
     */
    bool equals(Vec2 const& target) const noexcept;

    /** @returns if points have fuzzy equality which means equal with some degree of variance.
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    bool fuzzyEquals(Vec2 const& target, float variance) const;

    /** Calculates distance between point an origin
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float getLength() const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return std::sqrt(x * x + y * y);
#else
        auto const mul = v * v;
        return std::sqrt(mul[0] + mul[1]);
#endif
    }

    /** Calculates the square length of a Vec2 (not calling sqrt() )
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float getLengthSq() const
    {
        return dot(*this); // x*x + y*y;
    }

    /** Calculates the square distance between two points (not calling sqrt() )
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float getDistanceSq(Vec2 const& other) const { return (*this - other).getLengthSq(); }

    /** Calculates the distance between two points
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float getDistance(Vec2 const& other) const { return (*this - other).getLength(); }

    /** @returns the angle in radians between this vector and the x axis
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float getAngle() const { return std::atan2(v[1], v[0]); }

    /** @returns the angle in radians between two vector directions
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    float getAngle(Vec2 const& other) const;

    /** Calculates cross product of two points.
     @return float
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline float cross(Vec2 const& other) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return (x * other.y) - (y * other.x);
#else
        auto const mul = v * f32x2_t{other.v[1], other.v[0]};
        return mul[0] - mul[1];
#endif
    }

    /** Calculates perpendicular of v, rotated 90 degrees counter-clockwise -- cross(v, perp(v)) >= 0
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 getPerp() const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(-y, x);
#else
        return Vec2(f32x2_t{-v[1], v[0]});
#endif
    }

    /** Calculates midpoint between two points.
     @return Vec2
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2 getMidpoint(Vec2 const& other) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2((x + other.x) / 2.f, (y + other.y) / 2.f);
#else
        return Vec2((v + other.v) / 2.0f);
#endif
    }

    /** Clamp a point between from and to.
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2 getClampPoint(Vec2 const& min_inclusive, Vec2 const& max_inclusive) const
    {
        return Vec2(clampf(x, min_inclusive.x, max_inclusive.x), clampf(y, min_inclusive.y, max_inclusive.y));
    }

    /** Run a math operation function on each point component
     * absf, floorf, ceilf, roundf
     * any function that has the signature: float func(float);
     * For example: let's try to take the floor of x,y
     * p.compOp(floorf);
     @since v3.0
     * @js NA
     * @lua NA
     */
    inline Vec2 compOp(std::function<float(float)> function) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(function(x), function(y));
#else
        return Vec2(f32x2_t{function(v[0]), function(v[1])});
#endif
    }

    /** Calculates perpendicular of v, rotated 90 degrees clockwise -- cross(v, rperp(v)) <= 0
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 getRPerp() const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(y, -x);
#else
        return Vec2(f32x2_t{v[1], -v[0]});
#endif
    }

    /** Calculates the projection of this over other.
     @return Vec2
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 project(Vec2 const& other) const { return other * (dot(other) / other.dot(other)); }

    /** Complex multiplication of two points ("rotates" two points).
     @return Vec2 vector with an angle of this.getAngle() + other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 rotate(Vec2 const& other) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(x * other.x - y * other.y, x * other.y + y * other.x);
#else
        return Vec2(f32x2_t{v[0], v[0]} * other.v + f32x2_t{-v[1], v[1]} * f32x2_t{other.v[1], other.v[0]});
#endif
    }

    /** Unrotates two points.
     @return Vec2 vector with an angle of this.getAngle() - other.getAngle(),
     and a length of this.getLength() * other.getLength().
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 unrotate(Vec2 const& other) const
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(x * other.x + y * other.y, y * other.x - x * other.y);
#else
        return Vec2(v * f32x2_t{other.v[0], other.v[0]} + f32x2_t{v[1], -v[0]} * f32x2_t{other.v[1], other.v[1]});
#endif
    }

    /** Linear Interpolation between two points a and b
     @returns
        alpha == 0 ? a
        alpha == 1 ? b
        otherwise a value between a..b
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    inline Vec2 lerp(Vec2 const& other, float alpha) const { return *this * (1.f - alpha) + other * alpha; }

    /** Rotates a point counter clockwise by the angle around a pivot
     @param pivot is the pivot, naturally
     @param angle is the angle of rotation ccw in radians
     @returns the rotated point
     @since v2.1.4
     * @js NA
     * @lua NA
     */
    Vec2 rotateByAngle(Vec2 const& pivot, float angle) const;

    /**
     * @js NA
     * @lua NA
     */
    static inline Vec2 forAngle(float a)
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(std::cos(a), std::sin(a));
#else
        return Vec2(f32x2_t{std::cos(a), std::sin(a)});
#endif
    }

    /** A general line-line intersection test
     @param A   the startpoint for the first line L1 = (A - B)
     @param B   the endpoint for the first line L1 = (A - B)
     @param C   the startpoint for the second line L2 = (C - D)
     @param D   the endpoint for the second line L2 = (C - D)
     @param S   the range for a hitpoint in L1 (p = A + S*(B - A))
     @param T   the range for a hitpoint in L2 (p = C + T*(D - C))
     @return    whether these two lines intersects.

     Note that to truly test intersection for segments we have to make
     sure that S & T lie within [0..1] and for rays, make sure S & T > 0
     the hit point is        C + T * (D - C);
     the hit point also is   A + S * (B - A);
     @since 3.0
     * @js NA
     * @lua NA
     */
    static bool isLineIntersect(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D, float* S = nullptr, float* T = nullptr);

    /**
     returns true if Line A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineOverlap(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D);

    /**
     returns true if Line A-B parallel with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isLineParallel(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D);

    /**
     returns true if Segment A-B overlap with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentOverlap(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D, Vec2* S = nullptr, Vec2* E = nullptr);

    /**
     returns true if Segment A-B intersects with segment C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static bool isSegmentIntersect(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D);

    /**
     returns the intersection point of line A-B, C-D
     @since v3.0
     * @js NA
     * @lua NA
     */
    static Vec2 getIntersectPoint(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D);
    
    static Vec2 getIntersectSegmentPoint(Vec2 const& A, Vec2 const& B, Vec2 const& C, Vec2 const& D);

    /** equals to Vec2(0,0) */
    static Vec2 const ZERO;
    /** equals to Vec2(1,1) */
    static Vec2 const ONE;
    /** equals to Vec2(1,0) */
    static Vec2 const UNIT_X;
    /** equals to Vec2(0,1) */
    static Vec2 const UNIT_Y;
    /** equals to Vec2(0.5, 0.5) */
    static Vec2 const ANCHOR_MIDDLE;
    /** equals to Vec2(0, 0) */
    static Vec2 const ANCHOR_BOTTOM_LEFT;
    /** equals to Vec2(0, 1) */
    static Vec2 const ANCHOR_TOP_LEFT;
    /** equals to Vec2(1, 0) */
    static Vec2 const ANCHOR_BOTTOM_RIGHT;
    /** equals to Vec2(1, 1) */
    static Vec2 const ANCHOR_TOP_RIGHT;
    /** equals to Vec2(1, 0.5) */
    static Vec2 const ANCHOR_MIDDLE_RIGHT;
    /** equals to Vec2(0, 0.5) */
    static Vec2 const ANCHOR_MIDDLE_LEFT;
    /** equals to Vec2(0.5, 1) */
    static Vec2 const ANCHOR_MIDDLE_TOP;
    /** equals to Vec2(0.5, 0) */
    static Vec2 const ANCHOR_MIDDLE_BOTTOM;
};

/**
 * Calculates the scalar product of the given vector with the given value.
 *
 * @param x The value to scale by.
 * @param v The vector to scale.
 * @return The scaled vector.
 */
inline Vec2 const operator*(float x, Vec2 const& v) noexcept
{
    Vec2 result(v);
    result.scale(x);
    return result;
}

using Point = Vec2;

NS_CC_MATH_END

/**
 end of base group
 @}
 */
