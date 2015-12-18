/****************************************************************************
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2009      Jason Booth
 Copyright (c) 2009      Robert J Payne
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
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

#include "2d/CCSpriteFrameCache.h"

#include <vector>


#include "2d/CCSprite.h"
#include "2d/CCAutoPolygon.h"
#include "platform/CCFileUtils.h"
#include "base/CCNS.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "renderer/CCTexture2D.h"
#include "renderer/CCTextureCache.h"
#include "base/CCNinePatchImageParser.h"

#include "deprecated/CCString.h"


using namespace std;

NS_CC_BEGIN

static SpriteFrameCache *_sharedSpriteFrameCache = nullptr;

SpriteFrameCache* SpriteFrameCache::getInstance()
{
    if (! _sharedSpriteFrameCache)
    {
        _sharedSpriteFrameCache = new (std::nothrow) SpriteFrameCache();
        _sharedSpriteFrameCache->init();
    }
    
    return _sharedSpriteFrameCache;
}

void SpriteFrameCache::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedSpriteFrameCache);
}

bool SpriteFrameCache::init()
{
    _spriteFrames.reserve(20);
    _spriteFramesAliases.reserve(20);
    _loadedFileNames = new std::set<std::string>();
    return true;
}

SpriteFrameCache::~SpriteFrameCache()
{
    CC_SAFE_DELETE(_loadedFileNames);
}

void SpriteFrameCache::parseIntegerList(const std::string string, std::vector<int> &res)
{
    std::string delim(" ");

    size_t n = std::count(string.begin(), string.end(), ' ');
    res.resize(n+1);
    
    size_t start  = 0U;
    size_t end = string.find(delim);
    
    int i=0;
    while (end != std::string::npos)
    {
        res[i++] = std::stoi(string.substr(start, end - start));
        start = end + delim.length();
        end = string.find(delim, start);
    }
    
    res[i] = std::stoi(string.substr(start, end));
}

void SpriteFrameCache::initializePolygonInfo(const Size &textureSize,
                                             const Size &spriteSize,
                                             const std::vector<int> &vertices,
                                             const std::vector<int> &verticesUV,
                                             const std::vector<int> &triangleIndices,
                                             PolygonInfo &info)
{
    size_t vertexCount = vertices.size();
    size_t indexCount = triangleIndices.size();
    
    float scaleFactor = CC_CONTENT_SCALE_FACTOR();

    V3F_C4B_T2F *vertexData = new V3F_C4B_T2F[vertexCount];
    for (size_t i = 0; i < vertexCount/2; i++)
    {
        vertexData[i].colors = Color4B::WHITE;
        vertexData[i].vertices = Vec3(vertices[i*2] / scaleFactor,
                                      (spriteSize.height - vertices[i*2+1]) / scaleFactor,
                                      0);
        vertexData[i].texCoords = Tex2F(verticesUV[i*2] / textureSize.width,
                                        verticesUV[i*2+1] / textureSize.height);
    }

    unsigned short *indexData = new unsigned short[indexCount];
    for (size_t i = 0; i < indexCount; i++)
    {
        indexData[i] = static_cast<unsigned short>(triangleIndices[i]);
    }

    info.triangles.vertCount = vertexCount;
    info.triangles.verts = vertexData;
    info.triangles.indexCount = indexCount;
    info.triangles.indices = indexData;
    info.rect = Rect(0, 0, spriteSize.width, spriteSize.height);
}

void SpriteFrameCache::addSpriteFramesWithDictionary(ValueMap& dictionary, Texture2D* texture)
{
    /*
    Supported Zwoptex Formats:

    ZWTCoordinatesFormatOptionXMLLegacy = 0, // Flash Version
    ZWTCoordinatesFormatOptionXML1_0 = 1, // Desktop Version 0.0 - 0.4b
    ZWTCoordinatesFormatOptionXML1_1 = 2, // Desktop Version 1.0.0 - 1.0.1
    ZWTCoordinatesFormatOptionXML1_2 = 3, // Desktop Version 1.0.2+

    Version 3 with TexturePacker 4.0 polygon mesh packing
    */

    
    ValueMap& framesDict = dictionary["frames"].asValueMap();
    int format = 0;

    Size textureSize;

    // get the format
    if (dictionary.find("metadata") != dictionary.end())
    {
        ValueMap& metadataDict = dictionary["metadata"].asValueMap();
        format = metadataDict["format"].asInt();

        if(metadataDict.find("size") != metadataDict.end())
        {
            textureSize = SizeFromString(metadataDict["size"].asString());
        }
    }
    
    // check the format
    CCASSERT(format >=0 && format <= 3, "format is not supported for SpriteFrameCache addSpriteFramesWithDictionary:textureFilename:");
    
    auto textureFileName = Director::getInstance()->getTextureCache()->getTextureFilePath(texture);
    Image* image = nullptr;
    NinePatchImageParser parser;
    for (auto iter = framesDict.begin(); iter != framesDict.end(); ++iter)
    {
        ValueMap& frameDict = iter->second.asValueMap();
        std::string spriteFrameName = iter->first;
        
        std::vector<SpriteFrame*>* spriteFrames;
        if (_spriteFrames.find(iter->first) != _spriteFrames.end())
        {
            spriteFrames = _spriteFrames.at(spriteFrameName);
        }
        else
        {
            spriteFrames = new std::vector<SpriteFrame*>();
            _spriteFrames.insert(std::make_pair(spriteFrameName, spriteFrames));
        }
        
        for (auto frame : *spriteFrames)
        {
            if (frame->getTexture() == texture)
                continue;
        }
        SpriteFrame* spriteFrame;
        
        if(format == 0)
        {
            float x = frameDict["x"].asFloat();
            float y = frameDict["y"].asFloat();
            float w = frameDict["width"].asFloat();
            float h = frameDict["height"].asFloat();
            float ox = frameDict["offsetX"].asFloat();
            float oy = frameDict["offsetY"].asFloat();
            int ow = frameDict["originalWidth"].asInt();
            int oh = frameDict["originalHeight"].asInt();
            // check ow/oh
            if(!ow || !oh)
            {
                CCLOGWARN("cocos2d: WARNING: originalWidth/Height not found on the SpriteFrame. AnchorPoint won't work as expected. Regenrate the .plist");
            }
            // abs ow/oh
            ow = abs(ow);
            oh = abs(oh);
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                                                         Rect(x, y, w, h),
                                                         false,
                                                         Vec2(ox, oy),
                                                         Size((float)ow, (float)oh)
                                                         );
        }
        else if(format == 1 || format == 2)
        {
            Rect frame = RectFromString(frameDict["frame"].asString());
            bool rotated = false;
            
            // rotation
            if (format == 2)
            {
                rotated = frameDict["rotated"].asBool();
            }
            
            Vec2 offset = PointFromString(frameDict["offset"].asString());
            Size sourceSize = SizeFromString(frameDict["sourceSize"].asString());
            
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                                                         frame,
                                                         rotated,
                                                         offset,
                                                         sourceSize
                                                         );
        }
        else if (format == 3)
        {
            // get values
            Size spriteSize = SizeFromString(frameDict["spriteSize"].asString());
            Vec2 spriteOffset = PointFromString(frameDict["spriteOffset"].asString());
            Size spriteSourceSize = SizeFromString(frameDict["spriteSourceSize"].asString());
            Rect textureRect = RectFromString(frameDict["textureRect"].asString());
            bool textureRotated = frameDict["textureRotated"].asBool();
            
            // get aliases
            ValueVector& aliases = frameDict["aliases"].asValueVector();
            
            for(const auto &value : aliases) {
                std::string oneAlias = value.asString();
                if (_spriteFramesAliases.find(oneAlias) != _spriteFramesAliases.end())
                {
                    CCLOGWARN("cocos2d: WARNING: an alias with name %s already exists", oneAlias.c_str());
                }
                
                _spriteFramesAliases[oneAlias] = Value(spriteFrameName);
            }
            
            // create frame
            spriteFrame = SpriteFrame::createWithTexture(texture,
                                                         Rect(textureRect.origin.x, textureRect.origin.y, spriteSize.width, spriteSize.height),
                                                         textureRotated,
                                                         spriteOffset,
                                                         spriteSourceSize);

            if(frameDict.find("vertices") != frameDict.end())
            {
                std::vector<int> vertices;
                parseIntegerList(frameDict["vertices"].asString(), vertices);
                std::vector<int> verticesUV;
                parseIntegerList(frameDict["verticesUV"].asString(), verticesUV);
                std::vector<int> indices;
                parseIntegerList(frameDict["triangles"].asString(), indices);

                PolygonInfo info;
                initializePolygonInfo(textureSize, spriteSourceSize, vertices, verticesUV, indices, info);
                spriteFrame->setPolygonInfo(info);
            }
        }
        
        bool flag = NinePatchImageParser::isNinePatchImage(spriteFrameName);
        if(flag)
        {
            if ( !image ) image = new Image();
            image->initWithImageFile(textureFileName);

            parser.setSpriteFrameInfo(image, spriteFrame->getRectInPixels(), spriteFrame->isRotated());
            texture->addSpriteFrameCapInset(spriteFrame, parser.parseCapInset());
        }
        // add sprite frame
        spriteFrames->push_back(spriteFrame);
        spriteFrame->retain();
    }
    CC_SAFE_DELETE(image);
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& plist, Texture2D *texture)
{
    if (_loadedFileNames->find(plist) != _loadedFileNames->end())
    {
        return; // We already added it
    }
    
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    
    addSpriteFramesWithDictionary(dict, texture);
    _loadedFileNames->insert(plist);
}

void SpriteFrameCache::addSpriteFramesWithFileContent(const std::string& plist_content, Texture2D *texture)
{
    ValueMap dict = FileUtils::getInstance()->getValueMapFromData(plist_content.c_str(), static_cast<int>(plist_content.size()));
    addSpriteFramesWithDictionary(dict, texture);
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& plist, const std::string& textureFileName)
{
    CCASSERT(textureFileName.size()>0, "texture name should not be null");
    Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(textureFileName);
    
    if (texture)
    {
        addSpriteFramesWithFile(plist, texture);
    }
    else
    {
        CCLOG("cocos2d: SpriteFrameCache: couldn't load texture file. File not found %s", textureFileName.c_str());
    }
}

void SpriteFrameCache::addSpriteFramesWithFile(const std::string& plist)
{
    CCASSERT(plist.size()>0, "plist filename should not be nullptr");
    
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    if (fullPath.size() == 0)
    {
        // return if plist file doesn't exist
        CCLOG("cocos2d: SpriteFrameCache: can not find %s", plist.c_str());
        return;
    }
    
    if (_loadedFileNames->find(plist) == _loadedFileNames->end())
    {
        
        ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
        
        string texturePath("");
        
        if (dict.find("metadata") != dict.end())
        {
            ValueMap& metadataDict = dict["metadata"].asValueMap();
            // try to read  texture file name from meta data
            texturePath = metadataDict["textureFileName"].asString();
        }
        
        if (!texturePath.empty())
        {
            // build texture path relative to plist file
            texturePath = FileUtils::getInstance()->fullPathFromRelativeFile(texturePath.c_str(), plist);
        }
        else
        {
            // build texture path by replacing file extension
            texturePath = plist;
            
            // remove .xxx
            size_t startPos = texturePath.find_last_of(".");
            texturePath = texturePath.erase(startPos);
            
            // append .png
            texturePath = texturePath.append(".png");
            
            CCLOG("cocos2d: SpriteFrameCache: Trying to use file %s as texture", texturePath.c_str());
        }
        
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(texturePath.c_str());
        
        if (texture)
        {
            addSpriteFramesWithDictionary(dict, texture);
            _loadedFileNames->insert(plist);
        }
        else
        {
            CCLOG("cocos2d: SpriteFrameCache: Couldn't load texture");
        }
    }
}

bool SpriteFrameCache::isSpriteFramesWithFileLoaded(const std::string& plist) const
{
    bool result = false;
    
    if (_loadedFileNames->find(plist) != _loadedFileNames->end())
    {
        result = true;
    }
    
    return result;
}

void SpriteFrameCache::addSpriteFrame(SpriteFrame* frame, const std::string& frameName)
{
    if (_spriteFrames.find(frameName) != _spriteFrames.end())
    {
        std::vector<SpriteFrame*>* frames = _spriteFrames.at(frameName);
        frames->push_back(frame);
    }
    else
    {
        std::vector<SpriteFrame*>* newFrames = new std::vector<SpriteFrame*>();
        newFrames->push_back(frame);
        _spriteFrames.insert(std::make_pair(frameName, newFrames));
    }
    frame->retain();
}

void SpriteFrameCache::removeSpriteFrames()
{
    for (auto sprites : _spriteFrames)
    {
        delete sprites.second;
    }
    _spriteFrames.clear();
    _spriteFramesAliases.clear();
    _loadedFileNames->clear();
}

void SpriteFrameCache::removeUnusedSpriteFrames()
{
    bool removed = false;
    std::vector<std::string> toRemoveFrames;
    std::string frameName;
    
    for (auto iter = _spriteFrames.begin(); iter != _spriteFrames.end(); )
    {
        frameName = iter->first;
        std::vector<SpriteFrame*>* spriteFrames = _spriteFrames.at(frameName);
        if (spriteFrames->size() > 0)
        {
            std::vector<SpriteFrame*>::iterator frameIter;
            for (frameIter = spriteFrames->begin(); frameIter != spriteFrames->end(); )
            {
                SpriteFrame* spriteFrame = *frameIter;
                if(spriteFrame->getReferenceCount() <= 1 && spriteFrame->getTexture()->getReferenceCount() <= 1)
                {
                    frameIter = spriteFrames->erase(frameIter);
                    spriteFrame->release();
                    CCLOG("cocos2d: SpriteFrameCache: removing unused frame: %s", iter->first.c_str());
                    removed = true;
                }
                else
                {
                    frameIter++;
                }
            }
        }
        
        if (spriteFrames->size() == 0)
        {
            delete _spriteFrames.at(iter->first);
            iter = _spriteFrames.erase(iter);
        }
        else
        {
            iter++;
        }
    }
    
    // FIXME:. Since we don't know the .plist file that originated the frame, we must remove all .plist from the cache
    if( removed )
    {
        _loadedFileNames->clear();
    }
}


void SpriteFrameCache::removeSpriteFrameByName(const std::string& name)
{
    // explicit nil handling
    if( !(name.size()>0) )
        return;
    
    // Is this an alias ?
    std::string key = _spriteFramesAliases[name].asString();
    
    if (!key.empty())
    {
        for (auto spriteFrame : *_spriteFrames.at(key))
        {
            spriteFrame->release();
        }
        delete _spriteFrames.at(key);
        _spriteFrames.erase(key);
        _spriteFramesAliases.erase(key);
    }
    else
    {
        for (auto spriteFrame : *_spriteFrames.at(name))
        {
            spriteFrame->release();
        }
        delete _spriteFrames.at(name);
        _spriteFrames.erase(name);
    }
    
    // FIXME:. Since we don't know the .plist file that originated the frame, we must remove all .plist from the cache
    _loadedFileNames->clear();
}

void SpriteFrameCache::removeSpriteFramesFromFile(const std::string& plist)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(plist);
    ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(fullPath);
    Texture2D *texture = nullptr;
    if (dict.empty())
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFile: create dict by %s fail.", plist.c_str());
        return;
    }
    
    string texturePath("");
    
    if (dict.find("metadata") != dict.end())
    {
        ValueMap& metadataDict = dict["metadata"].asValueMap();
        // try to read  texture file name from meta data
        texturePath = metadataDict["textureFileName"].asString();
    }
    
    if (!texturePath.empty())
    {
        texturePath = FileUtils::getInstance()->fullPathFromRelativeFile(texturePath.c_str(), plist);
        texture = Director::getInstance()->getTextureCache()->getTextureForKey(texturePath.c_str());
    }
    else
    {
        // build texture path by replacing file extension
        texturePath = plist;
        
        // remove .xxx
        size_t startPos = texturePath.find_last_of(".");
        texturePath = texturePath.erase(startPos);
        
        // append .png
        texturePath = texturePath.append(".png");
        
        CCLOG("cocos2d: SpriteFrameCache: Trying to use file %s as texture", texturePath.c_str());
        
        texture = Director::getInstance()->getTextureCache()->getTextureForKey(texturePath.c_str());
    }
    
    if (!texture)
    {
        CCLOG("cocos2d: SpriteFrameCache: cannot find texture from plist %s", plist.c_str());
        return;
    }
    
    removeSpriteFramesFromDictionary(dict, texture);
    
    // remove it from the cache
    set<string>::iterator ret = _loadedFileNames->find(plist);
    if (ret != _loadedFileNames->end())
    {
        _loadedFileNames->erase(ret);
    }
}

void SpriteFrameCache::removeSpriteFramesFromFileContent(const std::string& plist_content)
{
    ValueMap dict = FileUtils::getInstance()->getValueMapFromData(plist_content.data(), static_cast<int>(plist_content.size()));
    if (dict.empty())
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFileContent: create dict by fail.");
        return;
    }
    
    Texture2D *texture = nullptr;
    string texturePath("");
    
    if (dict.find("metadata") != dict.end())
    {
        ValueMap& metadataDict = dict["metadata"].asValueMap();
        // try to read  texture file name from meta data
        texturePath = metadataDict["textureFileName"].asString();
    }
    
    if (texturePath.empty())
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromFileContent: cannot remove sprite frames with no associated texture.");
        return;
    }
    
    // build texture path relative to plist file
    texture = Director::getInstance()->getTextureCache()->getTextureForKey(texturePath.c_str());
    
    removeSpriteFramesFromDictionary(dict, texture);
}

void SpriteFrameCache::removeSpriteFramesFromDictionary(ValueMap& dictionary, Texture2D* texture)
{
    if (!texture)
    {
        CCLOG("cocos2d:SpriteFrameCache:removeSpriteFramesFromDictionary: cannot remove frames from null texture.");
        return;
    }
    
    ValueMap framesDict = dictionary["frames"].asValueMap();
    
    for (auto iter = framesDict.cbegin(); iter != framesDict.cend(); ++iter)
    {
        std::vector<SpriteFrame*>* spriteFrames = _spriteFrames.at(iter->first);
        if (spriteFrames->size() > 0)
        {
            std::vector<SpriteFrame*>::iterator frameIter;
            for (frameIter = spriteFrames->begin(); frameIter != spriteFrames->end(); )
            {
                if ((*frameIter)->getTexture() == texture)
                {
                    (*frameIter)->release();
                    frameIter = spriteFrames->erase(frameIter);
                }
                else
                {
                    frameIter++;
                }
            }
        }
        if (_spriteFrames.size() == 0)
        {
            delete _spriteFrames.at(iter->first);
            _spriteFrames.erase(iter->first);
        }
    }
}

void SpriteFrameCache::removeSpriteFramesFromTexture(Texture2D* texture)
{
    std::unordered_map<std::string, std::vector<SpriteFrame*>*>::iterator spriteFramesIterator;
    for (spriteFramesIterator = _spriteFrames.begin(); spriteFramesIterator != _spriteFrames.end(); )
    {
        std::string key = spriteFramesIterator->first;
        std::vector<SpriteFrame*>* spriteFrames = _spriteFrames.at(key);
        
        for (auto frameIter = spriteFrames->begin(); frameIter != spriteFrames->end(); )
        {
            if ((*frameIter)->getTexture() == texture)
            {
                (*frameIter)->release();
                frameIter = spriteFrames->erase(frameIter);
            }
            else
            {
                frameIter++;
            }
        }
        
        if (spriteFrames->size() == 0)
        {
            delete _spriteFrames.at(spriteFramesIterator->first);
            spriteFramesIterator = _spriteFrames.erase(spriteFramesIterator);
        }
        else
        {
            spriteFramesIterator++;
        }
    }
}

SpriteFrame* SpriteFrameCache::getSpriteFrameByName(const std::string& name, Texture2D* texture)
{
    SpriteFrame* frame = nullptr;
    if (_spriteFrames.find(name) != _spriteFrames.end())
    {
        std::vector<SpriteFrame*>* spriteFrames = _spriteFrames.at(name);
        if (!texture || (spriteFrames->size() == 1))
        {
            frame = spriteFrames->at(0);
        }
        else
        {
            for (auto sprFrame : *spriteFrames)
            {
                if (sprFrame->getTexture() == texture)
                {
                    frame = sprFrame;
                }
            }
        }
        if (!frame)
        {
            // try alias dictionary
            std::string key = _spriteFramesAliases[name].asString();
            if (!key.empty())
            {
                spriteFrames = _spriteFrames.at(key);
                if (!texture || spriteFrames->size() == 1)
                {
                    frame = spriteFrames->at(0);
                }
                else
                {
                    for (auto sprFrame : *spriteFrames)
                    {
                        if (sprFrame->getTexture() == texture)
                        {
                            frame = sprFrame;
                        }
                    }
                }
                if (!frame)
                {
                    CCLOG("cocos2d: SpriteFrameCache: Frame '%s' not found", name.c_str());
                }
            }
        }
    }
    return frame;
}

NS_CC_END
