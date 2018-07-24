#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    include <cocos/platform/emscripten/CCApplication-emscripten.h>

#    include <cocos/base/CCDirector.h>
#    include <cocos/platform/CCFileUtils.h>

#    include <emscripten/emscripten.h>
#    include <emscripten/bind.h>

#    include <cstdlib>

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

    // Handle application background/foreground change
    emscripten_run_script(
        #include "CCApplication-emscripten.cpp.js"
    );

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


// The following two are unused (using NY's NativeLocale instead)

[[noreturn]]
const char* Application::getCurrentLanguageCode()
{
    std::abort();
}

[[noreturn]]
LanguageType Application::getCurrentLanguage()
{
    std::abort();
}



//////////////////////////////////////////////////////////////////////////
// EMBIND
//////////////////////////////////////////////////////////////////////////


void    Application::js2cpp_onBackground()
{
    getInstance()->applicationDidEnterBackground();
}

void    Application::js2cpp_onInactive()
{
    getInstance()->applicationDidBecomeInactive();
}

void    Application::js2cpp_onForeground()
{
    getInstance()->applicationWillEnterForeground();
}

void    Application::js2cpp_onActive()
{
    getInstance()->applicationDidBecomeActive();
}


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"

EMSCRIPTEN_BINDINGS(cocos_Application_binding)
{
    emscripten::class_<Application>("cocos_Application_binding")
        .class_function("onBackground", &Application::js2cpp_onBackground)
        .class_function("onInactive", &Application::js2cpp_onInactive)
        .class_function("onForeground", &Application::js2cpp_onForeground)
        .class_function("onActive", &Application::js2cpp_onActive)
    ;
}

#pragma clang diagnostic pop



NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN
