/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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

#ifndef CC_2D_FONTFNT_H
#define CC_2D_FONTFNT_H

#include <cocos/2d/CCFont.h>
#include <cocos/math/Vec2.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/platform/CCPlatformDefine.h>

#include <iosfwd>
#include <string>

NS_CC_BEGIN

class BMFontConfiguration;
class FontAtlas;

class CC_DLL FontFNT final : public Font
{
    BMFontConfiguration* _configuration = nullptr;
    Vec2 _imageOffset = Vec2::ZERO;
    // User defined font size
    float _fontSize;

public:
    static FontFNT* create(std::string const& fntFilePath, Vec2 const& imageOffset = Vec2::ZERO);
    /** Purges the cached data.
    Removes from memory the cached configurations and the atlas name dictionary.
    */
    static void purgeCachedData();
    int* getHorizontalKerningForTextUTF16(std::u16string const& text, int& outNumLetters) const final;
    FontAtlas* createFontAtlas() final;
    inline void setFontSize(float fontSize) noexcept { _fontSize = fontSize; }
    int getOriginalFontSize() const noexcept;

    static void reloadBMFontResource(std::string const& fntFilePath);

private:
    FontFNT() = default;
    FontFNT(BMFontConfiguration* theContfig, Vec2 const& imageOffset = Vec2::ZERO);
    FontFNT(FontFNT const&) = delete;
    FontFNT& operator=(FontFNT const&) = delete;
    FontFNT(FontFNT&&) noexcept = delete;
    FontFNT& operator=(FontFNT&&) noexcept = delete;
    ~FontFNT() final;

private:
    int getHorizontalKerningForChars(unsigned short firstChar, unsigned short secondChar) const noexcept;
};

NS_CC_END

#endif // CC_2D_FONTFNT_H
