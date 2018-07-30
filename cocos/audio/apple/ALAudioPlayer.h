/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#pragma once

#include <cocos/platform/CCPlatformConfig.h>
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#    include <cocos/audio/apple/AudioCache.h>
#    include <cocos/audio/apple/AudioMacros.h>
#    include <cocos/audio/apple/AudioPlayer.h>
#    include <cocos/platform/CCPlatformMacros.h>

#    include <OpenAL/al.h>
#    include <chrono>
#    include <condition_variable>
#    include <mutex>
#    include <string>
#    include <thread>

NS_CC_BEGIN
namespace experimental
{
    class AudioEngineImpl;

    class ALAudioPlayer : public cocos2d::experimental::AudioPlayer
    {
    public:
        ALAudioPlayer(AudioCache& audioCache);
        ~ALAudioPlayer() override;

        void destroy() override;

        // queue buffer related stuff
        bool setTime(float time) override;
        float getTime() override;
        bool setLoop(bool loop) override;
        void setVolume(float volume) override;

        void stop() override { destroy(); }
        bool pause() override;
        bool resume() override;

        bool isStopped() override;
        std::chrono::milliseconds getDuration() override;

        ALuint getAlSource() override;
        void setAlSource(ALuint p_source) override;

    protected:
        bool play2d() override;
        void rotateBufferThread(int offsetFrame);
        void wakeupRotateThread() override;
        const std::function<void(int, const std::string&)> getFinishCallback() override;
        void setFinishCallback(const std::function<void(int, const std::string&)>& callback) override;

        AudioCache& _audioCache;

        std::function<void(int, const std::string&)> _finishCallbak;

        ALuint _alSource;

        // play by circular buffer
        ALuint _bufferIds[QUEUEBUFFER_NUM];
        std::thread* _rotateBufferThread;
        std::condition_variable _sleepCondition;
        std::mutex _sleepMutex;
        bool _timeDirty;
        std::atomic_bool _isRotateThreadExited;
        std::atomic_bool _needWakeupRotateThread;

        std::mutex _play2dMutex;

        unsigned int _id;
    };

} // namespace experimental
NS_CC_END

#endif
