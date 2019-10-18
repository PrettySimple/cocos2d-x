#ifndef __CC_FILEUTILS_EMSCRIPTEN_H__
#define __CC_FILEUTILS_EMSCRIPTEN_H__

#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    include <cocos/platform/CCFileUtils.h>
#    include <string>

NS_CC_BEGIN

class CC_DLL FileUtilsEmscripten : public FileUtils
{
protected:
    FileUtilsEmscripten();
    ~FileUtilsEmscripten();

public:
    bool init() override;
    std::string getWritablePath() const override;

private:
    bool isFileExistInternal(const std::string& strFilePath) const override;

private:
    friend class FileUtils;
    std::string _writablePath;
};

NS_CC_END

#endif

#endif
