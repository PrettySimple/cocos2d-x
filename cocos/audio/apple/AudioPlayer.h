

#pragma once

#include <cocos/platform/CCPlatformMacros.h>

#include <OpenAL/al.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <string>

NS_CC_BEGIN
namespace experimental
{
    class AudioEngineImpl;

    class AudioPlayer
    {
    public:
        AudioPlayer();
        virtual ~AudioPlayer();

        virtual void destroy() {}

        // queue buffer related stuff
        virtual bool setTime(float time) = 0;
        virtual float getTime() = 0;
        virtual bool setLoop(bool loop) = 0;
        virtual void setVolume(float volume) = 0;

        virtual void stop() { destroy(); }
        virtual bool pause() = 0;
        virtual bool resume() = 0;

        virtual bool isStopped() = 0;
        virtual std::chrono::milliseconds getDuration() = 0;

        virtual bool play2d() = 0;

        virtual ALuint getAlSource() { return AL_INVALID; }
        virtual void setAlSource(ALuint p_source) {}
        bool isStreamingSource();
        bool isRemovedByEngine();
        virtual void wakeupRotateThread() {}

        void setRemovedByEngine(bool p_removed) { _removeByAudioEngine = p_removed; }
        bool isReady() { return _ready; }

        virtual const std::function<void(int, const std::string&)> getFinishCallback() { return nullptr; }
        virtual void setFinishCallback(const std::function<void(int, const std::string&)>& callback) {}

    protected:
        std::atomic_bool _isDestroyed;
        std::atomic_bool _removeByAudioEngine;
        std::atomic_bool _ready;

        float _volume;
        bool _loop;

        float _currTime;
        bool _streamingSource;
    };
} // namespace experimental

NS_CC_END
