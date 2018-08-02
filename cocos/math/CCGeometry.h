/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef CC_MATH_GEOMETRY_H
#define CC_MATH_GEOMETRY_H

#include <cocos/base/ccMacros.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCPlatformConfig.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <cmath>
#include <limits>
#include <type_traits>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

class CC_DLL Size final
{
public:
#ifdef __ARM_NEON
    using f32x2_t = __attribute__((neon_vector_type(2))) float;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    using f32x2_t = float[2];
#else
    using f32x2_t = __attribute__((ext_vector_type(2))) float;
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
    union
    {
        f32x2_t v = {0.f, 0.f};
        struct
        {
            float width;
            float height;
        };
    };
#pragma clang diagnostic pop

public:
    /**Conversion from Vec2 to Size.*/
    inline operator Vec2() const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Vec2(width, height);
#else
        return Vec2(f32x2_t{width, height});
#endif
    }

public:
    Size() = default;
    Size(Size const&) = default;
    Size& operator=(Size const&) = default;
    Size(Size&&) noexcept = default;
    Size& operator=(Size&&) noexcept = default;
    ~Size() = default;

    /**
     * Constructs a new vector initialized to the specified values.
     *
     * @param xx The x coordinate.
     * @param yy The y coordinate.
     */
    constexpr Size(float w, float h) noexcept
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    : width(w)
    , height(h)
#else
    : v
    {
        w, h
    }
#endif
    {
    }

    constexpr Size(Vec2 const& other) noexcept
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    : width(other.x)
    , height(other.y)
#else
    : v(other.v)
#endif
    {
    }

    constexpr Size(Vec2&& other) noexcept
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    : width(other.x)
    , height(other.y)
#else
    : v(std::move(other.v))
#endif
    {
    }

#if CC_TARGET_PLATFORM != CC_PLATFORM_EMSCRIPTEN
    constexpr Size(f32x2_t&& other) noexcept
    : v(std::move(other))
    {
    }
#endif

    /**
     * @js NA
     * @lua NA
     */
    inline Size operator+(const Size& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Size(width + other.width, height + other.height);
#else
        return Size(v + other.v);
#endif
    }
    /**
     * @js NA
     * @lua NA
     */
    inline Size operator-(const Size& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Size(width - other.width, height - other.height);
#else
        return Size(v - other.v);
#endif
    }
    /**
     * @js NA
     * @lua NA
     */
    inline Size operator*(float a) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Size(width * a, height * a);
#else
        return Size(v * a);
#endif
    }
    /**
     * @js NA
     * @lua NA
     */
    inline Size operator/(float a) const noexcept
    {
        CCASSERT(std::abs(a) >= std::numeric_limits<float>::epsilon(), "CCSize division by 0.");
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        return Size(width / a, height / a);
#else
        return Size(v / a);
#endif
    }
    /**
    Set the width and height of Size.
     * @js NA
     * @lua NA
     */
    inline void setSize(float w, float h) noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        width = w;
        height = h;
#else
        v = {w, h};
#endif
    }
    /**
    Check if two size is the same.
     * @js NA
     */
    inline bool equals(const Size& other) const noexcept
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
        static constexpr auto const epsi = std::numeric_limits<float>::epsilon();
        return std::abs(width - other.width) < epsi && std::abs(height - other.height) < epsi;
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
        auto const eq = (v == other.v);
#    pragma clang diagnostic pop
        return eq[0] == -1 && eq[1] == -1;
#endif
    }
    /**Size(0,0).*/
    static const Size ZERO;
};

/**Rectangle area.*/
class CC_DLL Rect final
{
public:
    /**Low left point of rect.*/
    Vec2 origin;
    /**Width and height of the rect.*/
    Size size;

public:
    /**
    Constructor an empty Rect.
     * @js NA
     */
    Rect() = default;
    Rect(Rect const&) = default;
    Rect& operator=(Rect const&) = default;
    Rect(Rect&&) noexcept = default;
    Rect& operator=(Rect&&) noexcept = default;
    ~Rect() = default;

    /**
    Constructor a rect.
     * @js NA
     */
    constexpr Rect(float x, float y, float width, float height)
    : origin(x, y)
    , size(width, height)
    {
    }
    /**
     Constructor a rect.
     * @js NA
     */
    Rect(const Vec2& pos, const Size& dimension);
    /**
    Set the x, y, width and height of Rect.
     * @js NA
     * @lua NA
     */
    void setRect(float x, float y, float width, float height);
    /**
    Get the left of the rect.
     * @js NA
     */
    float getMinX() const; /// return the leftmost x-value of current rect
    /**
    Get the X coordinate of center point.
     * @js NA
     */
    float getMidX() const; /// return the midpoint x-value of current rect
    /**
    Get the right of rect.
     * @js NA
     */
    float getMaxX() const; /// return the rightmost x-value of current rect
    /**
    Get the bottom of rect.
     * @js NA
     */
    float getMinY() const; /// return the bottommost y-value of current rect
    /**
    Get the Y coordinate of center point.
     * @js NA
     */
    float getMidY() const; /// return the midpoint y-value of current rect
    /**
    Get top of rect.
     * @js NA
     */
    float getMaxY() const; /// return the topmost y-value of current rect
    /**
    Compare two rects.
     * @js NA
     */
    bool equals(const Rect& rect) const;
    /**
    Check if the points is contained in the rect.
     * @js NA
     */
    bool containsPoint(const Vec2& point) const;
    /**
    Check the intersect status of two rects.
     * @js NA
     */
    bool intersectsRect(const Rect& rect) const;
    /**
    Check the intersect status of the rect and a circle.
     * @js NA
     */
    bool intersectsCircle(const Vec2& center, float radius) const;
    /**
    Get the min rect which can contain this and rect.
     * @js NA
     * @lua NA
     */
    Rect unionWithRect(const Rect& rect) const;
    /**Compute the min rect which can contain this and rect, assign it to this.*/
    void merge(const Rect& rect);
    /**An empty Rect.*/
    static const Rect ZERO;
};

NS_CC_END

// end of base group
/// @}

#endif // CC_MATH_GEOMETRY_H
