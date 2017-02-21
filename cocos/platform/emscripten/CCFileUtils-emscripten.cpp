
#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#include "platform/emscripten/CCFileUtils-emscripten.h"
#include <limits.h>
#include <unistd.h>

using namespace std;

NS_CC_BEGIN

FileUtils* FileUtils::getInstance()
{
    if (s_sharedFileUtils == nullptr)
    {
        s_sharedFileUtils = new FileUtilsEmscripten();
        if(!s_sharedFileUtils->init())
        {
          delete s_sharedFileUtils;
          s_sharedFileUtils = nullptr;
          CCLOG("ERROR: Could not init CCFileUtilsLinux");
        }
    }
    return s_sharedFileUtils;
}

FileUtilsEmscripten::FileUtilsEmscripten() : FileUtils()
{
}

FileUtilsEmscripten::~FileUtilsEmscripten()
{
}

bool FileUtilsEmscripten::init()
{
    _defaultResRootPath = "/bin/Resources/";
    char temp[PATH_MAX];
    temp[PATH_MAX-1] = '\0';
    _writablePath = (getcwd(temp, PATH_MAX) ? std::string(temp) : std::string(""));
    return FileUtils::init();
}

string FileUtilsEmscripten::getWritablePath() const
{
    return _writablePath;
}

bool FileUtilsEmscripten::isFileExistInternal(const std::string& strFilePath) const
{
    std::string strPath = strFilePath;
    if (strPath.front() != '/')
    {
        if (strPath.find(_defaultResRootPath) == std::string::npos)
        {// Didn't find "assets/" at the beginning of the path, adding it.
            strPath.insert(0, _defaultResRootPath);
        }
    }

    return access(strPath.c_str(), F_OK) != -1 ? true : false;
}

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_LINUX
