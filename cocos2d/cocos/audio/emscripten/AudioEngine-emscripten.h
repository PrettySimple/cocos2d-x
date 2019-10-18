
#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#    ifndef __AUDIO_ENGINE_EMPSCRIPTEN_H_
#        define __AUDIO_ENGINE_EMPSCRIPTEN_H_

#        include "base/CCRef.h"
#        include <chrono>
#        include <functional>
#        include <string>
#        include <unordered_map>
#        include <vector>

NS_CC_BEGIN

namespace experimental
{
#        define MAX_AUDIOINSTANCES 32

    class CC_DLL AudioEngineImpl : public cocos2d::Ref
    {
    public:
        using preload_callback_t = std::function<void(bool)>;
        using finish_callback_t = std::function<void(int, const std::string&)>;

    private:
        /*
        // Required as JS calls us back using the path (I prefered that over passing the callback ptr to JS)
        // We're supporting multiple preload requests per path (hence the std::vector) although it is unlikely
        // to be used (if the application is using the PSAssetManager)
        std::unordered_map<std::string, std::vector<preload_callback_t>>	_preloadCallbacks;
        */

        // Finally only supporting one callback per filePath. We'll dumbly override the callback should
        // we receive a subsequent request on a same pending filePath.

        std::unordered_map<std::string, preload_callback_t> _pendingPreloads;

        std::unordered_map<int, finish_callback_t> _finishCallbacks;

    public:
        AudioEngineImpl();
        ~AudioEngineImpl();

        bool init();

        ////////////////////////////////////////////
        // PRELOADING

        void preload(const std::string& filePath, const preload_callback_t& callback);
        void uncache(const std::string& filePath);
        void uncacheAll(); // /!\ MUST NOT BE INVOKED. SHALL ABORT. /!\ //

        ////////////////////////////////////////////
        // PLAYING

        int play2d(const std::string& fileFullPath, bool loop, float volume, bool isMusic);
        void setVolume(int audioID, float volume);
        void setLoop(int audioID, bool loop);
        void pause(int audioID);
        void resume(int audioID);
        void stop(int audioID);
        void stopAll(); // /!\ MUST NOT BE INVOKED. SHALL ABORT. /!\ //

        std::chrono::milliseconds getDuration(int audioID);
        float getCurrentTime(int audioID);
        bool setCurrentTime(int audioID, float time);

        void setFinishCallback(int audioID, const finish_callback_t& callback);

    public:
        // Must be public, by design...
        static void js2cpp_preloadCallback(uintptr_t class_ptr, const std::string& filePath, bool success);
        static void js2cpp_finishCallback(uintptr_t class_ptr, int audioID, const std::string& filePath);
    };
} // namespace experimental

NS_CC_END

#    endif // __AUDIO_ENGINE_EMPSCRIPTEN_H_

#endif
