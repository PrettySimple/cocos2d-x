
#ifndef CCAPLICATION_H_
#define CCAPLICATION_H_

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    include "platform/CCApplicationProtocol.h"
#    include "platform/CCCommon.h"
#    include <string>

NS_CC_BEGIN
class Rect;

class Application : public ApplicationProtocol
{
public:
    /**
     * @js ctor
     */
    Application();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Application();

    /**
     @brief Callback by Director for limit FPS.
     @param interval    The time, which expressed in second in second, between current frame and next.
     */
    void setAnimationInterval(float interval) override;

    /**
     @brief Run the message loop.
     */
    int run();

    /**
     @brief Get current application instance.
     @return Current application instance pointer.
     */
    static Application* getInstance();

    /** @deprecated Use getInstance() instead */
    CC_DEPRECATED_ATTRIBUTE static Application* sharedApplication();

    /* override functions */
    [[noreturn]]
    virtual LanguageType getCurrentLanguage() override;

    /**
    @brief Get current language iso 639-1 code
    @return Current language iso 639-1 code
    */
    [[noreturn]]
    virtual const char* getCurrentLanguageCode() override;

    /**
    @brief Get application version
    */
    virtual std::string getVersion() override;

    /**
     @brief Open url in default browser
     @param String with url to open.
     @return true if the resource located by the URL was successfully opened; otherwise false.
     */
    virtual bool openURL(const std::string& url) override;

    /**
     *  Sets the Resource root path.
     *  @deprecated Please use FileUtils::getInstance()->setSearchPaths() instead.
     */
    CC_DEPRECATED_ATTRIBUTE void setResourceRootPath(const std::string& rootResDir);

    /**
     *  Gets the Resource root path.
     *  @deprecated Please use FileUtils::getInstance()->getSearchPaths() instead.
     */
    CC_DEPRECATED_ATTRIBUTE const std::string& getResourceRootPath(void);

    /**
     @brief Get target platform
     */
    virtual Platform getTargetPlatform() override;

protected:
    long _animationInterval; // micro second
    std::string _resourceRootPath;
    static Application* sm_pSharedApplication;


/* EMBIND - must be public */
public:

    static void js2cpp_onBackground();
    static void js2cpp_onInactive();
    static void js2cpp_onForeground();
    static void js2cpp_onActive();

};

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#endif /* CCAPLICATION_H_ */
