/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
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

#include <cocos/base/ccTypes.h>
#include <cocos/renderer/backend/Types.h>

NS_CC_BEGIN

const std::string STD_STRING_EMPTY("");
static constexpr auto const epsi = std::numeric_limits<GLfloat>::epsilon();
const std::size_t CC_INVALID_INDEX = std::numeric_limits<std::size_t>::max();

/**
 * Color3B
 */

Color3B::Color3B(const Color4B& color)
: r(color.r)
, g(color.g)
, b(color.b)
{
}

Color3B::Color3B(const Color4F& color)
: r(color.r * 255.0f)
, g(color.g * 255.0f)
, b(color.b * 255.0f)
{
}

bool Color3B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color4F& right) const
{
    return (right.a == 1.0f && Color4F(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4B
 */

Color4B::Color4B(const Color3B& color, GLubyte _a)
: r(color.r)
, g(color.g)
, b(color.b)
, a(_a)
{
}

Color4B::Color4B(const Color4F& color)
: r(color.r * 255)
, g(color.g * 255)
, b(color.b * 255)
, a(color.a * 255)
{
}

bool Color4B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color4B::operator==(const Color4F& right) const
{
    return (*this == Color4B(right));
}

bool Color4B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4F
 */

Color4F::Color4F(const Color3B& color, GLfloat _a)
: r(static_cast<GLfloat>(color.r) / 255.0f)
, g(static_cast<GLfloat>(color.g) / 255.0f)
, b(static_cast<GLfloat>(color.b) / 255.0f)
, a(_a)
{
}

Color4F::Color4F(const Color4B& color)
: r(static_cast<GLfloat>(color.r) / 255.0f)
, g(static_cast<GLfloat>(color.g) / 255.0f)
, b(static_cast<GLfloat>(color.b) / 255.0f)
, a(static_cast<GLfloat>(color.a) / 255.0f)
{
}

bool Color4F::operator==(const Color4F& other) const
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
    return (std::abs(r - other.r) < epsi && std::abs(g - other.g) < epsi && std::abs(b - other.b) < epsi && std::abs(a - other.a) < epsi);
#else
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wfloat-equal"
    auto const eq = (v == other.v);
#    pragma clang diagnostic pop
    return eq[0] == -1 && eq[1] == -1 && eq[2] == -1 && eq[3] == -1;
#endif
}

bool Color4F::operator==(const Color3B& other) const
{
    return (std::abs(a - 1.0f) < epsi && Color3B(*this) == other);
}

bool Color4F::operator==(const Color4B& other) const
{
    return (*this == Color4F(other));
}

bool Color4F::operator!=(const Color4F& other) const
{
    return !(*this == other);
}

bool Color4F::operator!=(const Color3B& other) const
{
    return !(*this == other);
}

bool Color4F::operator!=(const Color4B& other) const
{
    return !(*this == other);
}

/**
 * Color constants
 */

const Color3B Color3B::WHITE  (255, 255, 255);
const Color3B Color3B::YELLOW (255, 255,   0);
const Color3B Color3B::GREEN  (  0, 255,   0);
const Color3B Color3B::BLUE   (  0,   0, 255);
const Color3B Color3B::RED    (255,   0,   0);
const Color3B Color3B::MAGENTA(255,   0, 255);
const Color3B Color3B::BLACK  (  0,   0,   0);
const Color3B Color3B::ORANGE (255, 127,   0);
const Color3B Color3B::GRAY   (166, 166, 166);

const Color4B Color4B::WHITE  (255, 255, 255, 255);
const Color4B Color4B::YELLOW (255, 255,   0, 255);
const Color4B Color4B::GREEN  (  0, 255,   0, 255);
const Color4B Color4B::BLUE   (  0,   0, 255, 255);
const Color4B Color4B::RED    (255,   0,   0, 255);
const Color4B Color4B::MAGENTA(255,   0, 255, 255);
const Color4B Color4B::BLACK  (  0,   0,   0, 255);
const Color4B Color4B::ORANGE (255, 127,   0, 255);
const Color4B Color4B::GRAY   (166, 166, 166, 255);

const Color4F Color4F::WHITE  (    1,     1,     1, 1);
const Color4F Color4F::YELLOW (    1,     1,     0, 1);
const Color4F Color4F::GREEN  (    0,     1,     0, 1);
const Color4F Color4F::BLUE   (    0,     0,     1, 1);
const Color4F Color4F::RED    (    1,     0,     0, 1);
const Color4F Color4F::MAGENTA(    1,     0,     1, 1);
const Color4F Color4F::BLACK  (    0,     0,     0, 1);
const Color4F Color4F::ORANGE (    1,  0.5f,     0, 1);
const Color4F Color4F::GRAY   (0.65f, 0.65f, 0.65f, 1);

const BlendFunc BlendFunc::DISABLE = {backend::BlendFactor::ONE, backend::BlendFactor::ZERO};
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED = { backend::BlendFactor::ONE, backend::BlendFactor::ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED = { backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ADDITIVE = { backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE};

NS_CC_END
