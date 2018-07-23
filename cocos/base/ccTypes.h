/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.

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

#ifndef CC_BASE_TYPES_H
#define CC_BASE_TYPES_H

#include <cocos/base/CCRef.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/math/Vec2.h>
#include <cocos/math/Vec3.h>
#include <cocos/platform/CCGL.h>
#include <cocos/platform/CCPlatformDefine.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <cstddef>
#include <iosfwd>

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

struct Color4B;
struct Color4F;

/**
 * RGB color composed of bytes 3 bytes.
 * @since v3.0
 */
struct CC_DLL Color3B final
{
    GLubyte r;
    GLubyte g;
    GLubyte b;

    Color3B() = default;
    Color3B(Color3B const&) = default;
    Color3B& operator=(Color3B const&) = default;
    Color3B(Color3B&&) noexcept = default;
    Color3B& operator=(Color3B&&) noexcept = default;
    ~Color3B() = default;

    constexpr Color3B(GLubyte _r, GLubyte _g, GLubyte _b)
    : r(_r)
    , g(_g)
    , b(_b)
    {
    }
    explicit Color3B(const Color4B& color);
    explicit Color3B(const Color4F& color);

    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;

    bool equals(const Color3B& other) const { return (*this == other); }

    static const Color3B WHITE;
    static const Color3B YELLOW;
    static const Color3B BLUE;
    static const Color3B GREEN;
    static const Color3B RED;
    static const Color3B MAGENTA;
    static const Color3B BLACK;
    static const Color3B ORANGE;
    static const Color3B GRAY;
};

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct CC_DLL Color4B final
{
public:
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;

    Color4B() = default;
    Color4B(Color4B const&) = default;
    Color4B& operator=(Color4B const&) = default;
    Color4B(Color4B&&) noexcept = default;
    Color4B& operator=(Color4B&&) noexcept = default;
    ~Color4B() = default;

    constexpr Color4B(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
    : r(_r)
    , g(_g)
    , b(_b)
    , a(_a)
    {
    }
    explicit Color4B(const Color3B& color, GLubyte _a = 255);
    explicit Color4B(const Color4F& color);

    inline void set(GLubyte _r, GLubyte _g, GLubyte _b, GLubyte _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    bool operator==(const Color4B& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4F& right) const;
    bool operator!=(const Color4B& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4F& right) const;

    static const Color4B WHITE;
    static const Color4B YELLOW;
    static const Color4B BLUE;
    static const Color4B GREEN;
    static const Color4B RED;
    static const Color4B MAGENTA;
    static const Color4B BLACK;
    static const Color4B ORANGE;
    static const Color4B GRAY;
};

/**
 * RGBA color composed of 4 floats.
 * @since v3.0
 */
struct CC_DLL Color4F final
{
#ifdef __ARM_NEON
    using f32x4_t = __attribute__((neon_vector_type(4))) GLfloat;
#else
    using f32x4_t = __attribute__((ext_vector_type(4))) GLfloat;
#endif

public:
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
    union
    {
        f32x4_t v = {0.f, 0.f, 0.f, 0.f};
        struct
        {
            GLfloat r;
            GLfloat g;
            GLfloat b;
            GLfloat a;
        };
    };
#pragma clang diagnostic pop

    Color4F() = default;
    Color4F(Color4F const&) = default;
    Color4F& operator=(Color4F const&) = default;
    Color4F(Color4F&&) noexcept = default;
    Color4F& operator=(Color4F&&) noexcept = default;
    ~Color4F() = default;

    constexpr Color4F(float _r, float _g, float _b, float _a)
    : v{_r, _g, _b, _a}
    {
    }
    explicit Color4F(const Color3B& color, float _a = 1.0f);
    explicit Color4F(const Color4B& color);

    bool operator==(const Color4F& right) const;
    bool operator==(const Color3B& right) const;
    bool operator==(const Color4B& right) const;
    bool operator!=(const Color4F& right) const;
    bool operator!=(const Color3B& right) const;
    bool operator!=(const Color4B& right) const;

    bool equals(const Color4F& other) const { return (*this == other); }

    static const Color4F WHITE;
    static const Color4F YELLOW;
    static const Color4F BLUE;
    static const Color4F GREEN;
    static const Color4F RED;
    static const Color4F MAGENTA;
    static const Color4F BLACK;
    static const Color4F ORANGE;
    static const Color4F GRAY;
};

/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex2F
// {
//     Vertex2F(float _x, float _y) :x(_x), y(_y) {}

//     Vertex2F(): x(0.f), y(0.f) {}

//     GLfloat x;
//     GLfloat y;
// };

/** A vertex composed of 2 floats: x, y
 @since v3.0
 */
// struct Vertex3F
// {
//     Vertex3F(float _x, float _y, float _z)
//         : x(_x)
//         , y(_y)
//         , z(_z)
//     {}

//     Vertex3F(): x(0.f), y(0.f), z(0.f) {}

//     GLfloat x;
//     GLfloat y;
//     GLfloat z;
// };

/** @struct Tex2F
 * A TEXCOORD composed of 2 floats: u, y
 * @since v3.0
 */
struct CC_DLL Tex2F final
{
    GLfloat u = 0.f;
    GLfloat v = 0.f;

    Tex2F() = default;
    Tex2F(Tex2F const&) = default;
    Tex2F& operator=(Tex2F const&) = default;
    Tex2F(Tex2F&&) noexcept = default;
    Tex2F& operator=(Tex2F&&) noexcept = default;
    ~Tex2F() = default;

    constexpr Tex2F(float _u, float _v)
    : u(_u)
    , v(_v)
    {
    }
};

/** @struct PointSprite
 * Vec2 Sprite component.
 */
struct CC_DLL PointSprite final
{
    Vec2 pos; // 8 bytes
    Color4B color; // 4 bytes
    GLfloat size = 0.f; // 4 bytes
};

/** @struct Quad2
 * A 2D Quad. 4 * 2 floats.
 */
struct CC_DLL Quad2 final
{
    Vec2 tl;
    Vec2 tr;
    Vec2 bl;
    Vec2 br;
};

/** @struct Quad3
 * A 3D Quad. 4 * 3 floats.
 */
struct CC_DLL Quad3 final
{
    Vec3 bl;
    Vec3 br;
    Vec3 tl;
    Vec3 tr;
};

/** @struct V2F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct V2F_C4B_T2F final
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4B)
    Color4B colors;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V2F_C4B_PF
 *
 */
struct V2F_C4B_PF final
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4B)
    Color4B colors;
    /// pointsize
    float pointSize = 0.f;
};

/** @struct V2F_C4F_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4F.
 */
struct CC_DLL V2F_C4F_T2F final
{
    /// vertices (2F)
    Vec2 vertices;
    /// colors (4F)
    Color4F colors;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V3F_C4B_T2F
 * A Vec2 with a vertex point, a tex coord point and a color 4B.
 */
struct CC_DLL V3F_C4B_T2F final
{
    /// vertices (3F)
    Vec3 vertices; // 12 bytes

    /// colors (4B)
    Color4B colors; // 4 bytes

    // tex coords (2F)
    Tex2F texCoords; // 8 bytes
};

/** @struct V3F_T2F
 * A Vec2 with a vertex point, a tex coord point.
 */
struct CC_DLL V3F_T2F final
{
    /// vertices (2F)
    Vec3 vertices;
    /// tex coords (2F)
    Tex2F texCoords;
};

/** @struct V2F_C4B_T2F_Triangle
 * A Triangle of V2F_C4B_T2F.
 */
struct CC_DLL V2F_C4B_T2F_Triangle final
{
    V2F_C4B_T2F a;
    V2F_C4B_T2F b;
    V2F_C4B_T2F c;
};

/** @struct V2F_C4B_T2F_Quad
 * A Quad of V2F_C4B_T2F.
 */
struct CC_DLL V2F_C4B_T2F_Quad final
{
    /// bottom left
    V2F_C4B_T2F bl;
    /// bottom right
    V2F_C4B_T2F br;
    /// top left
    V2F_C4B_T2F tl;
    /// top right
    V2F_C4B_T2F tr;
};

/** @struct V3F_C4B_T2F_Quad
 * 4 Vertex3FTex2FColor4B.
 */
struct CC_DLL V3F_C4B_T2F_Quad final
{
    /// top left
    V3F_C4B_T2F tl;
    /// bottom left
    V3F_C4B_T2F bl;
    /// top right
    V3F_C4B_T2F tr;
    /// bottom right
    V3F_C4B_T2F br;
};

/** @struct V2F_C4F_T2F_Quad
 * 4 Vertex2FTex2FColor4F Quad.
 */
struct CC_DLL V2F_C4F_T2F_Quad final
{
    /// bottom left
    V2F_C4F_T2F bl;
    /// bottom right
    V2F_C4F_T2F br;
    /// top left
    V2F_C4F_T2F tl;
    /// top right
    V2F_C4F_T2F tr;
};

/** @struct V3F_T2F_Quad
 *
 */
struct CC_DLL V3F_T2F_Quad final
{
    /// bottom left
    V3F_T2F bl;
    /// bottom right
    V3F_T2F br;
    /// top left
    V3F_T2F tl;
    /// top right
    V3F_T2F tr;
};

/** @struct BlendFunc
 * Blend Function used for textures.
 */
struct CC_DLL BlendFunc final
{
    /** source blend function */
    GLenum src;
    /** destination blend function */
    GLenum dst;

    /** Blending disabled. Uses {GL_ONE, GL_ZERO} */
    static const BlendFunc DISABLE;
    /** Blending enabled for textures with Alpha premultiplied. Uses {GL_ONE, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_PREMULTIPLIED;
    /** Blending enabled for textures with Alpha NON premultiplied. Uses {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_NON_PREMULTIPLIED;
    /** Enables Additive blending. Uses {GL_SRC_ALPHA, GL_ONE} */
    static const BlendFunc ADDITIVE;

    inline bool operator==(const BlendFunc& a) const { return src == a.src && dst == a.dst; }

    inline bool operator!=(const BlendFunc& a) const { return src != a.src || dst != a.dst; }

    inline bool operator<(const BlendFunc& a) const { return src < a.src || (src == a.src && dst < a.dst); }
};

/** @enum TextVAlignment
 * Vertical text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum struct CC_DLL TextVAlignment : std::uint8_t
{
    TOP,
    CENTER,
    BOTTOM
};

/** @enum TextHAlignment
 * Horizontal text alignment type.
 *
 * @note If any of these enums are edited and/or reordered, update Texture2D.m.
 */
enum struct CC_DLL TextHAlignment : std::uint8_t
{
    LEFT,
    CENTER,
    RIGHT
};

/**
 * @brief Possible GlyphCollection used by Label.
 *
 * Specify a collections of characters to be load when Label created.
 * Consider using DYNAMIC.
 */
enum struct GlyphCollection : std::uint8_t
{
    DYNAMIC,
    NEHE,
    ASCII,
    CUSTOM
};

// Types for animation in particle systems

/** @struct T2F_Quad
 * Texture coordinates for a quad.
 */
struct CC_DLL T2F_Quad final
{
    /// bottom left
    Tex2F bl;
    /// bottom right
    Tex2F br;
    /// top left
    Tex2F tl;
    /// top right
    Tex2F tr;
};

/** @struct AnimationFrameData
 * Struct that holds the size in pixels, texture coordinates and delays for animated ParticleSystemQuad.
 */
struct CC_DLL AnimationFrameData final
{
    T2F_Quad texCoords;
    float delay = 0.f;
    Size size;
};

/**
 types used for defining fonts properties (i.e. font name, size, stroke or shadow)
 */

/** @struct FontShadow
 * Shadow attributes.
 */
struct CC_DLL FontShadow final
{
    /// true if shadow enabled
    bool _shadowEnabled = false;
    /// shadow x and y offset
    Size _shadowOffset;
    /// shadow blurriness
    float _shadowBlur = 0.f;
    /// shadow opacity
    float _shadowOpacity = 0.f;
};

/** @struct FontStroke
 * Stroke attributes.
 */
struct CC_DLL FontStroke final
{
    /// true if stroke enabled
    bool _strokeEnabled = false;
    /// stroke color
    Color3B _strokeColor = Color3B::BLACK;
    /// stroke alpha
    GLubyte _strokeAlpha = 255;
    /// stroke size
    float _strokeSize = 0.f;
};

/** @struct FontDefinition
 * Font attributes.
 */
struct CC_DLL FontDefinition final
{
    /// font name
    std::string _fontName;
    /// font size
    int _fontSize = 0;
    /// horizontal alignment
    TextHAlignment _alignment = TextHAlignment::CENTER;
    /// vertical alignment
    TextVAlignment _vertAlignment = TextVAlignment::TOP;
    /// rendering box
    Size _dimensions;
    /// font color
    Color3B _fontFillColor = Color3B::WHITE;
    /// font alpha
    GLubyte _fontAlpha = 255;
    /// font shadow
    FontShadow _shadow;
    /// font stroke
    FontStroke _stroke;
    /// enable text wrap
    bool _enableWrap = true;
    /** There are 4 overflows: none, clamp, shrink and resize_height.
     *  The corresponding integer values are 0, 1, 2, 3 respectively
     * For more information, please refer to Label::Overflow enum class.
     */
    int _overflow = 0;
};

/**
 * @brief Effects used by `Label`
 *
 */
enum struct LabelEffect : std::uint8_t
{
    // FIXME: Covert them to bitwise. More than one effect should be supported
    NORMAL,
    OUTLINE,
    SHADOW,
    GLOW,
    ITALICS,
    BOLD,
    UNDERLINE,
    STRIKETHROUGH,
    ALL
};

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
struct CC_DLL Acceleration final : public Ref
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    double timestamp = 0.0;

    Acceleration() = default;
    Acceleration(Acceleration const&) = default;
    Acceleration& operator=(Acceleration const&) = default;
    Acceleration(Acceleration&&) noexcept = default;
    Acceleration& operator=(Acceleration&&) noexcept = default;
    ~Acceleration() final;
};

extern const std::size_t CC_DLL CC_INVALID_INDEX;

NS_CC_END
// end group
/// @}
#endif // CC_BASE_TYPES_H
