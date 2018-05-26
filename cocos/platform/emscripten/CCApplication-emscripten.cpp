#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    include "base/CCDirector.h"
#    include "platform/CCFileUtils.h"
#    include "platform/emscripten/CCApplication-emscripten.h"
#    include <emscripten/emscripten.h>

NS_CC_BEGIN

// sharedApplication pointer
Application* Application::sm_pSharedApplication = nullptr;

Application::Application()
: _animationInterval(1.0f / 60.0f * 1000.0f)
{
    CC_ASSERT(!sm_pSharedApplication);
    sm_pSharedApplication = this;
}

Application::~Application()
{
    CC_ASSERT(this == sm_pSharedApplication);
    sm_pSharedApplication = nullptr;
}

extern "C" void mainLoopIter()
{
    auto director = Director::getInstance();
    director->mainLoop();
}

int Application::run()
{
    initGLContextAttrs();
    // Initialize instance and cocos2d.
    if (!applicationDidFinishLaunching())
    {
        return 0;
    }

    emscripten_set_main_loop(&mainLoopIter, 0, 1);

    return EXIT_SUCCESS;
}

void Application::setAnimationInterval(float interval)
{
    _animationInterval = interval * 1000.0f;
}

void Application::setResourceRootPath(const std::string& rootResDir)
{
    _resourceRootPath = rootResDir;
    if (_resourceRootPath[_resourceRootPath.length() - 1] != '/')
    {
        _resourceRootPath += '/';
    }
    FileUtils* pFileUtils = FileUtils::getInstance();
    std::vector<std::string> searchPaths = pFileUtils->getSearchPaths();
    searchPaths.insert(searchPaths.begin(), _resourceRootPath);
    pFileUtils->setSearchPaths(searchPaths);
}

const std::string& Application::getResourceRootPath()
{
    return _resourceRootPath;
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::OS_EMSCRIPTEN;
}

std::string Application::getVersion()
{
    return "";
}

bool Application::openURL(const std::string& url)
{
    // TODO EMSCRIPTEN: Implement openURL
    return false;
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    CC_ASSERT(sm_pSharedApplication);
    return sm_pSharedApplication;
}

// @deprecated Use getInstance() instead
Application* Application::sharedApplication()
{
    return Application::getInstance();
}

const char* Application::getCurrentLanguageCode()
{
    // TODO EMSCRIPTEN: Implement language detection
    return "en";
}

LanguageType Application::getCurrentLanguage()
{
    // TODO EMSCRIPTEN: Implement language detection
    return LanguageType::ENGLISH;
}

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
