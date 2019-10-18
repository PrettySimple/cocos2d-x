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

#include <cocos/2d/CCFontCharMap.h>

#include <cocos/2d/CCFontAtlas.h>
#include <cocos/base/CCDirector.h>
#include <cocos/base/CCValue.h>
#include <cocos/base/ccMacros.h>
#include <cocos/math/CCGeometry.h>
#include <cocos/platform/CCFileUtils.h>
#include <cocos/platform/CCPlatformMacros.h>
#include <cocos/renderer/CCTexture2D.h>
#include <cocos/renderer/CCTextureCache.h>

#include <new>

NS_CC_BEGIN

FontCharMap* FontCharMap::create(const std::string& plistFile)
{
    std::string pathStr = FileUtils::getInstance()->fullPathForFilename(plistFile);
    std::string relPathStr = pathStr.substr(0, pathStr.find_last_of("/")) + "/";

    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(pathStr);

    CCASSERT(dict["version"].asInt() == 1, "Unsupported version. Upgrade cocos2d version");

    std::string textureFilename = relPathStr + dict["textureFilename"].asString();

    unsigned int width = dict["itemWidth"].asInt();
    unsigned int height = dict["itemHeight"].asInt();
    unsigned int startChar = dict["firstChar"].asInt();

    Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(textureFilename);
    if (!tempTexture)
    {
        return nullptr;
    }

    FontCharMap* tempFont = new FontCharMap(tempTexture, width, height, startChar);

    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->autorelease();
    return tempFont;
}

FontCharMap* FontCharMap::create(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    Texture2D* tempTexture = Director::getInstance()->getTextureCache()->addImage(charMapFile);

    if (!tempTexture)
    {
        return nullptr;
    }

    FontCharMap* tempFont = new FontCharMap(tempTexture, itemWidth, itemHeight, startCharMap);

    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->autorelease();
    return tempFont;
}

FontCharMap* FontCharMap::create(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    FontCharMap* tempFont = new FontCharMap(texture, itemWidth, itemHeight, startCharMap);

    if (!tempFont)
    {
        return nullptr;
    }
    tempFont->autorelease();
    return tempFont;
}

FontCharMap::FontCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
: _texture(texture)
, _mapStartChar(startCharMap)
, _itemWidth(itemWidth)
, _itemHeight(itemHeight)
{
}

FontCharMap::~FontCharMap()
{
}

int* FontCharMap::getHorizontalKerningForTextUTF16(const std::u16string& text, int& outNumLetters) const
{
    return nullptr;
}

FontAtlas* FontCharMap::createFontAtlas()
{
    FontAtlas* tempAtlas = new (std::nothrow) FontAtlas(*this);
    if (!tempAtlas)
        return nullptr;

    Size s = _texture->getContentSizeInPixels();
    int itemsPerColumn = static_cast<int>(s.height / _itemHeight);
    int itemsPerRow = static_cast<int>(s.width / _itemWidth);

    tempAtlas->setLineHeight(_itemHeight);

    auto contentScaleFactor = CC_CONTENT_SCALE_FACTOR();

    FontLetterDefinition tempDefinition;
    tempDefinition.textureID = 0;
    tempDefinition.offsetX = 0.0f;
    tempDefinition.offsetY = 0.0f;
    tempDefinition.validDefinition = true;
    tempDefinition.width = _itemWidth / contentScaleFactor;
    tempDefinition.height = _itemHeight / contentScaleFactor;
    tempDefinition.xAdvance = _itemWidth;

    int charId = _mapStartChar;
    for (int row = 0; row < itemsPerColumn; ++row)
    {
        for (int col = 0; col < itemsPerRow; ++col)
        {
            tempDefinition.U = _itemWidth * col / contentScaleFactor;
            tempDefinition.V = _itemHeight * row / contentScaleFactor;

            tempAtlas->addLetterDefinition(charId, tempDefinition);
            charId++;
        }
    }

    tempAtlas->addTexture(_texture, 0);

    return tempAtlas;
}

NS_CC_END
