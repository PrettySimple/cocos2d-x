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

#include <cocos/2d/CCFontFNT.h>

#include <cocos/2d/CCFontAtlas.h>
#include <cocos/base/CCConfiguration.h>
#include <cocos/base/CCDirector.h>
#include <cocos/base/CCRef.h>
#include <cocos/base/ccMacros.h>
#include <cocos/base/ccUTF8.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/platform/CCFileUtils.h>
#include <cocos/renderer/CCTextureCache.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <array>
#include <cmath>
#include <limits>
#include <new>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace cocos2d
{
    class Texture2D;
}

NS_CC_BEGIN

/**
@struct BMFontDef
BMFont definition
*/
struct BMFontDef
{
    //! ID of the character
    unsigned int charID = 0;
    //! origin and size of the font
    Rect rect = Rect::ZERO;
    //! The X amount the image should be offset when drawing the image (in pixels)
    short xOffset = 0;
    //! The Y amount the image should be offset when drawing the image (in pixels)
    short yOffset = 0;
    //! The amount to move the current position after drawing the character (in pixels)
    short xAdvance = 0;
};

/** @struct BMFontPadding
BMFont padding
@since v0.8.2
*/
struct BMFontPadding
{
    /// padding left
    int left = 0;
    /// padding top
    int top = 0;
    /// padding right
    int right = 0;
    /// padding bottom
    int bottom = 0;
};

/** @brief BMFontConfiguration has parsed configuration of the .fnt file
@since v0.8
*/
class CC_DLL BMFontConfiguration final : public Ref
{
    // BMFont definitions
    std::unordered_map<unsigned int, BMFontDef> _fontDefDictionary;

    //! FNTConfig: Common Height Should be signed (issue #1343)
    int _commonHeight = 0;
    //! Padding
    BMFontPadding _padding;
    //! atlas name
    std::string _atlasName;
    //! values for kerning
    std::unordered_map<unsigned int, int> _kerningDictionary;

    // Character Set defines the letters that actually exist in the font
    std::unordered_set<unsigned int> _characterSet;
    //! Font Size
    int _fontSize = 0;

public:
    BMFontConfiguration() = default;
    BMFontConfiguration(BMFontConfiguration const&) = delete;
    BMFontConfiguration& operator=(BMFontConfiguration const&) = delete;
    BMFontConfiguration(BMFontConfiguration&&) noexcept = delete;
    BMFontConfiguration& operator=(BMFontConfiguration&&) noexcept = delete;
    ~BMFontConfiguration() final = default;

    std::string description() const;

    /** allocates a BMFontConfiguration with a FNT file */
    static BMFontConfiguration* create(std::string const& FNTfile);

    /** initializes a BitmapFontConfiguration with a FNT file */
    bool initWithFNTfile(std::string const& FNTfile);

    inline const std::string& getAtlasName() const noexcept { return _atlasName; }
    inline void setAtlasName(std::string const& atlasName) { _atlasName = atlasName; }

    inline decltype(_characterSet) const& getCharacterSet() const noexcept { return _characterSet; }

    inline int getFontSize() const noexcept { return _fontSize; }
    inline int getCommonHeight() const noexcept { return _commonHeight; }

    inline decltype(_kerningDictionary) const& getKerningDictionary() const noexcept { return _kerningDictionary; }
    inline decltype(_fontDefDictionary) const& getKFontDefDictionary() const noexcept { return _fontDefDictionary; }

private:
    std::unordered_set<unsigned int> parseConfigFile(const std::string& controlFile);
    std::unordered_set<unsigned int> parseBinaryConfigFile(unsigned char* pData, unsigned long size, std::string const& controlFile);

    void parseCharacterDefinition(std::string const& line, BMFontDef& characterDefinition);
    void parseInfoArguments(std::string const& line);
    void parseCommonArguments(std::string const& line);
    void parseImageFileName(std::string const& line, std::string const& fntFile);
    void parseKerningEntry(std::string const& line);
};

//
// FNTConfig Cache - free functions
//
std::unordered_map<std::string, BMFontConfiguration*>& get_configurations()
{
    static std::unordered_map<std::string, BMFontConfiguration*> configurations;
    return configurations;
}

BMFontConfiguration* FNTConfigLoadFile(std::string const& fntFile)
{
    auto& configurations = get_configurations();

    if (auto search = configurations.find(fntFile); search != configurations.end())
    {
        return search->second;
    }
    else
    {
        BMFontConfiguration* ret = BMFontConfiguration::create(fntFile);
        ret->retain();
        configurations.emplace(fntFile, ret);
        return ret;
    }
}

//
// BitmapFontConfiguration
//

BMFontConfiguration* BMFontConfiguration::create(std::string const& FNTfile)
{
    BMFontConfiguration* ret = new (std::nothrow) BMFontConfiguration();
    if (ret->initWithFNTfile(FNTfile))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool BMFontConfiguration::initWithFNTfile(std::string const& FNTfile)
{
    _characterSet = this->parseConfigFile(FNTfile);

    if (_characterSet.empty())
    {
        return false;
    }

    return true;
}

std::string BMFontConfiguration::description() const
{
    return StringUtils::format("<BMFontConfiguration = " CC_FORMAT_PRINTF_SIZE_T " | Glphys:%d Kernings:%d | Image = %s>", (size_t)this,
                               _fontDefDictionary.size(), _kerningDictionary.size(), _atlasName.c_str());
}

std::unordered_set<unsigned int> BMFontConfiguration::parseConfigFile(std::string const& controlFile)
{
    std::unordered_set<unsigned int> validCharsString;
    std::string const data = FileUtils::getInstance()->getStringFromFile(controlFile);
    if (data.empty())
    {
        return validCharsString;
    }
    if (data.size() >= (sizeof("BMP") - 1) && memcmp("BMF", data.c_str(), sizeof("BMP") - 1) == 0)
    {
        // Handle fnt file of binary format
        return parseBinaryConfigFile(reinterpret_cast<unsigned char*>(const_cast<char*>(data.data())), data.size(), controlFile);
    }
    if (data.front() == '\0')
    {
        CCLOG("cocos2d: Error parsing FNTfile %s", controlFile.c_str());
        return validCharsString;
    }

    using namespace std::literals;
    static constexpr std::string_view const prefix_info_face = "info face"sv;
    static constexpr std::string_view const prefix_common_line_height = "common lineHeight"sv;
    static constexpr std::string_view const prefix_page_id = "page id"sv;
    static constexpr std::string_view const prefix_chars_c = "chars c"sv;
    static constexpr std::string_view const prefix_char = "char"sv;
    static constexpr std::string_view const prefix_kerning_first = "kerning first"sv;

    std::istringstream input(data);
    for (std::string line; std::getline(input, line);)
    {
        if (line.compare(0, prefix_info_face.size(), prefix_info_face) == 0)
        {
            parseInfoArguments(line);
        }
        else if (line.compare(0, prefix_common_line_height.size(), prefix_common_line_height) == 0)
        {
            parseCommonArguments(line);
        }
        else if (line.compare(0, prefix_page_id.size(), prefix_page_id) == 0)
        {
            parseImageFileName(line, controlFile);
        }
        else if (line.compare(0, prefix_chars_c.size(), prefix_chars_c) == 0)
        {
            // Ignore this line
        }
        else if (line.compare(0, prefix_char.size(), prefix_char) == 0)
        {
            // Parse the current line and create a new CharDef
            BMFontDef fontDef;
            parseCharacterDefinition(line, fontDef);

            _fontDefDictionary.emplace(fontDef.charID, fontDef);
            validCharsString.emplace(fontDef.charID);
        }
        else if (line.compare(0, prefix_kerning_first.size(), prefix_kerning_first) == 0)
        {
            parseKerningEntry(line);
        }
    }

    return validCharsString;
}

std::unordered_set<unsigned int> BMFontConfiguration::parseBinaryConfigFile(unsigned char* pData, unsigned long size, std::string const& controlFile)
{
    /* based on http://www.angelcode.com/products/bmfont/doc/file_format.html file format */

    std::unordered_set<unsigned int> validCharsString;

    unsigned long remains = size;

    CCASSERT(pData[3] == 3, "Only version 3 is supported");

    pData += 4;
    remains -= 4;

    while (remains > 0)
    {
        unsigned char blockId = pData[0];
        pData += 1;
        remains -= 1;
        uint32_t blockSize = 0;
        memcpy(&blockSize, pData, 4);

        pData += 4;
        remains -= 4;

        if (blockId == 1)
        {
            /*
             fontSize       2   int      0
             bitField       1   bits     2  bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeight, bits 5-7: reserved
             charSet        1   uint     3
             stretchH       2   uint     4
             aa             1   uint     6
             paddingUp      1   uint     7
             paddingRight   1   uint     8
             paddingDown    1   uint     9
             paddingLeft    1   uint     10
             spacingHoriz   1   uint     11
             spacingVert    1   uint     12
             outline        1   uint     13 added with version 2
             fontName       n+1 string   14 null terminated string with length n
             */

            memcpy(&_fontSize, pData, 2);
            _padding.top = (unsigned char)pData[7];
            _padding.right = (unsigned char)pData[8];
            _padding.bottom = (unsigned char)pData[9];
            _padding.left = (unsigned char)pData[10];
        }
        else if (blockId == 2)
        {
            /*
             lineHeight 2   uint    0
             base       2   uint    2
             scaleW     2   uint    4
             scaleH     2   uint    6
             pages      2   uint    8
             bitField   1   bits    10  bits 0-6: reserved, bit 7: packed
             alphaChnl  1   uint    11
             redChnl    1   uint    12
             greenChnl  1   uint    13
             blueChnl   1   uint    14
             */

            uint16_t lineHeight = 0;
            memcpy(&lineHeight, pData, 2);
            _commonHeight = lineHeight;

            uint16_t scaleW = 0;
            memcpy(&scaleW, pData + 4, 2);
            uint16_t scaleH = 0;
            memcpy(&scaleH, pData + 6, 2);

            CCASSERT(scaleW <= Configuration::getInstance()->getMaxTextureSize() && scaleH <= Configuration::getInstance()->getMaxTextureSize(),
                     "CCLabelBMFont: page can't be larger than supported");

            uint16_t pages = 0;
            memcpy(&pages, pData + 8, 2);
            CCASSERT(pages == 1, "CCBitfontAtlas: only supports 1 page");
        }
        else if (blockId == 3)
        {
            /*
             pageNames 	p*(n+1) 	strings 	0 	p null terminated strings, each with length n
             */

            const char* value = reinterpret_cast<char const*>(pData);
            CCASSERT(strlen(value) < blockSize, "Block size should be less then string");

            _atlasName = FileUtils::getInstance()->fullPathFromRelativeFile(value, controlFile);
        }
        else if (blockId == 4)
        {
            /*
             id         4   uint    0+c*20  These fields are repeated until all characters have been described
             x          2   uint    4+c*20
             y          2   uint    6+c*20
             width      2   uint    8+c*20
             height     2   uint    10+c*20
             xoffset    2   int     12+c*20
             yoffset    2   int     14+c*20
             xadvance   2   int     16+c*20
             page       1   uint    18+c*20
             chnl       1   uint    19+c*20
             */

            unsigned long count = blockSize / 20;

            for (unsigned long i = 0; i < count; i++)
            {
                BMFontDef fontDef;

                uint32_t charId = 0;
                memcpy(&charId, pData + (i * 20), 4);
                fontDef.charID = charId;

                uint16_t charX = 0;
                memcpy(&charX, pData + (i * 20) + 4, 2);
                fontDef.rect.origin.x = charX;

                uint16_t charY = 0;
                memcpy(&charY, pData + (i * 20) + 6, 2);
                fontDef.rect.origin.y = charY;

                uint16_t charWidth = 0;
                memcpy(&charWidth, pData + (i * 20) + 8, 2);
                fontDef.rect.size.width = charWidth;

                uint16_t charHeight = 0;
                memcpy(&charHeight, pData + (i * 20) + 10, 2);
                fontDef.rect.size.height = charHeight;

                int16_t xoffset = 0;
                memcpy(&xoffset, pData + (i * 20) + 12, 2);
                fontDef.xOffset = xoffset;

                int16_t yoffset = 0;
                memcpy(&yoffset, pData + (i * 20) + 14, 2);
                fontDef.yOffset = yoffset;

                int16_t xadvance = 0;
                memcpy(&xadvance, pData + (i * 20) + 16, 2);
                fontDef.xAdvance = xadvance;

                _fontDefDictionary.emplace(fontDef.charID, fontDef);
                validCharsString.emplace(fontDef.charID);
            }
        }
        else if (blockId == 5)
        {
            /*
             first  4   uint    0+c*10 	These fields are repeated until all kerning pairs have been described
             second 4   uint    4+c*10
             amount 2   int     8+c*10
             */

            unsigned long count = blockSize / 20;

            for (unsigned long i = 0; i < count; i++)
            {
                uint32_t first = 0;
                memcpy(&first, pData + (i * 10), 4);
                uint32_t second = 0;
                memcpy(&second, pData + (i * 10) + 4, 4);
                int16_t amount = 0;
                memcpy(&amount, pData + (i * 10) + 8, 2);

                unsigned int key = (first << 16) | (second & 0xffff);
                _kerningDictionary.emplace(key, amount);
            }
        }

        pData += blockSize;
        remains -= blockSize;
    }

    return validCharsString;
}

void BMFontConfiguration::parseImageFileName(std::string const& line, std::string const& fntFile)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // page id=0 file="bitmapFontTest.png"
    //////////////////////////////////////////////////////////////////////////
    auto line_c = line.c_str();

    // page ID. Sanity check
    int pageId = 0;
    sscanf(line_c, "page id=%d", &pageId);
    CCASSERT(pageId == 0, "LabelBMFont file could not be found");
    // file
    std::array<char, 255> fileName;
    fileName.fill('\0');
    sscanf(strchr(line_c, '"') + 1, "%[^\"]", fileName.data());
    _atlasName = FileUtils::getInstance()->fullPathFromRelativeFile(fileName.data(), fntFile);
}

void BMFontConfiguration::parseInfoArguments(std::string const& line)
{
    //////////////////////////////////////////////////////////////////////////
    // possible lines to parse:
    // info face="Script" size=32 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=1 padding=1,4,3,2 spacing=0,0 outline=0
    // info face="Cracked" size=36 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1
    //////////////////////////////////////////////////////////////////////////
    auto line_c = line.c_str();

    sscanf(strstr(line_c, "size=") + 5, "%d", &_fontSize);
    // padding
    sscanf(strstr(line_c, "padding=") + 8, "%d,%d,%d,%d", &_padding.top, &_padding.right, &_padding.bottom, &_padding.left);
    // CCLOG("cocos2d: padding: %d,%d,%d,%d", _padding.left, _padding.top, _padding.right, _padding.bottom);
}

void BMFontConfiguration::parseCommonArguments(std::string const& line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // common lineHeight=104 base=26 scaleW=1024 scaleH=512 pages=1 packed=0
    //////////////////////////////////////////////////////////////////////////

    auto line_c = line.c_str();

    // Height
    auto tmp = strstr(line_c, "lineHeight=") + 11;
    sscanf(tmp, "%d", &_commonHeight);

#if COCOS2D_DEBUG > 0
    // scaleW. sanity check
    int value;
    tmp = strstr(tmp, "scaleW=") + 7;
    sscanf(tmp, "%d", &value);

    int maxTextureSize = Configuration::getInstance()->getMaxTextureSize();
    CCASSERT(value <= maxTextureSize, "CCLabelBMFont: page can't be larger than supported");

    // scaleH. sanity check
    tmp = strstr(tmp, "scaleH=") + 7;
    sscanf(tmp, "%d", &value);
    CCASSERT(value <= maxTextureSize, "CCLabelBMFont: page can't be larger than supported");

    // pages. sanity check
    tmp = strstr(tmp, "pages=") + 6;
    sscanf(tmp, "%d", &value);
    CCASSERT(value == 1, "CCBitfontAtlas: only supports 1 page");
#endif
    // packed (ignore) What does this mean ??
}

void BMFontConfiguration::parseCharacterDefinition(std::string const& line, BMFontDef& characterDefinition)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // char id=32   x=0     y=0     width=0     height=0     xoffset=0     yoffset=44    xadvance=14     page=0  chnl=0
    //////////////////////////////////////////////////////////////////////////

    auto line_c = line.c_str();

    // Character ID
    auto tmp = strstr(line_c, "id=") + 3;
    sscanf(tmp, "%u", &characterDefinition.charID);

    // Character x
    tmp = strstr(tmp, "x=") + 2;
    sscanf(tmp, "%f", &characterDefinition.rect.origin.x);
    // Character y
    tmp = strstr(tmp, "y=") + 2;
    sscanf(tmp, "%f", &characterDefinition.rect.origin.y);
    // Character width
    tmp = strstr(tmp, "width=") + 6;
    sscanf(tmp, "%f", &characterDefinition.rect.size.width);
    // Character height
    tmp = strstr(tmp, "height=") + 7;
    sscanf(tmp, "%f", &characterDefinition.rect.size.height);
    // Character xoffset
    tmp = strstr(tmp, "xoffset=") + 8;
    sscanf(tmp, "%hd", &characterDefinition.xOffset);
    // Character yoffset
    tmp = strstr(tmp, "yoffset=") + 8;
    sscanf(tmp, "%hd", &characterDefinition.yOffset);
    // Character xadvance
    tmp = strstr(tmp, "xadvance=") + 9;
    sscanf(tmp, "%hd", &characterDefinition.xAdvance);
}

void BMFontConfiguration::parseKerningEntry(std::string const& line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // kerning first=121  second=44  amount=-7
    //////////////////////////////////////////////////////////////////////////

    auto line_c = line.c_str();

    int first, second, amount;
    auto tmp = strstr(line_c, "first=") + 6;
    sscanf(tmp, "%d", &first);

    tmp = strstr(tmp, "second=") + 7;
    sscanf(tmp, "%d", &second);

    tmp = strstr(tmp, "amount=") + 7;
    sscanf(tmp, "%d", &amount);

    unsigned int key = (first << 16) | (second & 0xffff);
    _kerningDictionary.emplace(key, amount);
}

FontFNT* FontFNT::create(std::string const& fntFilePath, Vec2 const& imageOffset /* = Vec2::ZERO */)
{
    BMFontConfiguration* newConf = FNTConfigLoadFile(fntFilePath);
    if (!newConf)
        return nullptr;

    // add the texture
    Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(newConf->getAtlasName());
    if (!tempTexture)
    {
        return nullptr;
    }

    FontFNT* tempFont = new FontFNT(newConf, imageOffset);
    tempFont->setFontSize(newConf->getFontSize());
    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->autorelease();
    return tempFont;
}

FontFNT::FontFNT(BMFontConfiguration* theContfig, const Vec2& imageOffset /* = Vec2::ZERO */)
: _configuration(theContfig)
, _imageOffset(CC_POINT_PIXELS_TO_POINTS(imageOffset))
{
    _configuration->retain();
}

FontFNT::~FontFNT()
{
    _configuration->release();
}

void FontFNT::purgeCachedData()
{
    auto& configurations = get_configurations();
    for (auto& conf : configurations)
    {
        conf.second->release();
    }
    configurations.clear();
}

int* FontFNT::getHorizontalKerningForTextUTF16(std::u16string const& text, int& outNumLetters) const
{
    outNumLetters = static_cast<int>(text.size());

    if (!outNumLetters)
        return nullptr;

    int* sizes = new (std::nothrow) int[outNumLetters];
    if (!sizes)
        return nullptr;

    for (int c = 0; c < outNumLetters; ++c)
    {
        if (c < (outNumLetters - 1))
            sizes[c] = getHorizontalKerningForChars(text[c], text[c + 1]);
        else
            sizes[c] = 0;
    }

    return sizes;
}

int FontFNT::getHorizontalKerningForChars(unsigned short firstChar, unsigned short secondChar) const noexcept
{
    unsigned int const key = (firstChar << 16) | (secondChar & 0xffff);

    auto const& kerningDic = _configuration->getKerningDictionary();
    auto search = kerningDic.find(key);
    if (search != kerningDic.end())
    {
        return search->second;
    }

    return 0;
}

int FontFNT::getOriginalFontSize() const noexcept
{
    return _configuration->getFontSize();
}

FontAtlas* FontFNT::createFontAtlas()
{
    // check that everything is fine with the BMFontCofniguration
    if (_configuration->getKFontDefDictionary().size() == 0)
        return nullptr;

    if (_configuration->getCharacterSet().size() == 0)
        return nullptr;

    if (_configuration->getCommonHeight() == 0)
        return nullptr;

    FontAtlas* tempAtlas = new (std::nothrow) FontAtlas(*this);
    if (tempAtlas == nullptr)
        return nullptr;

    // common height
    int originalFontSize = _configuration->getFontSize();
    float originalLineHeight = _configuration->getCommonHeight();
    float factor = 0.0f;
    if (std::abs(_fontSize - originalFontSize) < std::numeric_limits<float>::epsilon())
    {
        factor = 1.0f;
    }
    else
    {
        factor = _fontSize / originalFontSize;
    }

    tempAtlas->setLineHeight(originalLineHeight * factor);

    // Purge uniform hash
    for (auto const& [key, fontDef] : _configuration->getKFontDefDictionary())
    {
        FontLetterDefinition tempDefinition;
        Rect tempRect;

        tempRect = fontDef.rect;
        tempRect = CC_RECT_PIXELS_TO_POINTS(tempRect);

        tempDefinition.offsetX = fontDef.xOffset;
        tempDefinition.offsetY = fontDef.yOffset;

        tempDefinition.U = tempRect.origin.x + _imageOffset.x;
        tempDefinition.V = tempRect.origin.y + _imageOffset.y;

        tempDefinition.width = tempRect.size.width;
        tempDefinition.height = tempRect.size.height;

        // carloX: only one texture supported FOR NOW
        tempDefinition.textureID = 0;

        tempDefinition.validDefinition = true;
        tempDefinition.xAdvance = fontDef.xAdvance;
        // add the new definition
        if (65535 < fontDef.charID)
        {
            CCLOGWARN("Warning: 65535 < fontDef.charID (%u), ignored", fontDef.charID);
        }
        else
        {
            tempAtlas->addLetterDefinition(fontDef.charID, tempDefinition);
        }
    }

    // add the texture (only one texture for now)
    Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(_configuration->getAtlasName());
    if (tempTexture == nullptr)
    {
        CC_SAFE_RELEASE(tempAtlas);
        return nullptr;
    }
    // add the texture
    tempAtlas->addTexture(tempTexture, 0);
    // done
    return tempAtlas;
}

void FontFNT::reloadBMFontResource(const std::string& fntFilePath)
{
    auto& configurations = get_configurations();

    if (auto search = configurations.find(fntFilePath); search != configurations.end())
    {
        search->second->release();
        configurations.erase(search);
    }

    if (BMFontConfiguration* ret = BMFontConfiguration::create(fntFilePath); ret != nullptr)
    {
        configurations.emplace(fntFilePath, ret);
        Director::getInstance()->getTextureCache()->reloadTexture(ret->getAtlasName());
    }
}

NS_CC_END
