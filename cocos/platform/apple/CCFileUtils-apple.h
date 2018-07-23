/****************************************************************************
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
#ifndef CC_PLATFORM_APPLE_FILEUTILSAPPLE_H
#define CC_PLATFORM_APPLE_FILEUTILSAPPLE_H

#include <cocos/base/ccTypes.h>
#include <cocos/platform/CCFileUtils.h>
#include <cocos/platform/CCPlatformMacros.h>

#include <memory>
#include <string>
#include <vector>

#if CC_FILEUTILS_APPLE_ENABLE_OBJC
@class NSBundle;
#endif

NS_CC_BEGIN

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class CC_DLL FileUtilsApple : public FileUtils
{
public:
    FileUtilsApple();
    ~FileUtilsApple() override;
    /* override functions */
    std::string getWritablePath() const override;
    std::string getFullPathForDirectoryAndFilename(const std::string& directory, const std::string& filename) const override;

    ValueMap getValueMapFromFile(const std::string& filename) override;
    ValueMap getValueMapFromData(const char* filedata, int filesize) override;
    bool writeToFile(const ValueMap& dict, const std::string& fullPath) override;

    ValueVector getValueVectorFromFile(const std::string& filename) override;
    ValueVector getValueVectorFromData(const char* filedata, int filesize) override;
#if CC_FILEUTILS_APPLE_ENABLE_OBJC
    void setBundle(NSBundle* bundle);
#endif

    bool createDirectory(const std::string& path) override;

private:
    bool isFileExistInternal(const std::string& filePath) const override;
    bool removeDirectory(const std::string& dirPath) override;
    void valueMapCompact(ValueMap& valueMap) override;
    void valueVectorCompact(ValueVector& valueVector) override;

    struct IMPL;
    std::unique_ptr<IMPL> pimpl_;
};

// end of platform group
/// @}

NS_CC_END

#endif // CC_PLATFORM_APPLE_FILEUTILSAPPLE_H
