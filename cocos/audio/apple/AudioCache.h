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

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#    import <OpenAL/al.h>

#    include <chrono>
#    include <mutex>
#    include <string>
#    include <vector>

#    include "audio/apple/AudioMacros.h"
#    include "platform/CCPlatformMacros.h"

NS_CC_BEGIN
namespace experimental
{
    class AudioEngineImpl;
    class AudioPlayer;

    class AudioCache final
    {
    public:
        enum class State
        {
            INITIAL,
            LOADING,
            READY,
            FAILED
        };

        AudioCache();
        AudioCache(AudioCache const&) = delete;
        AudioCache& operator=(AudioCache const&) = delete;
        AudioCache(AudioCache&& other) noexcept = delete;
        AudioCache& operator=(AudioCache&&) noexcept = delete;
        ~AudioCache();

        void addPlayCallback(const std::function<void()>& callback);

        void addLoadCallback(const std::function<void(bool)>& callback);

        void setSkipReadDataTask(bool isSkip);
        void readDataTask(unsigned int selfId);
        
        State getState();
        void setState(const State& p_state);
        
        bool isAskedAsPreload();
        void setAskedAsPreload(bool p_askedAsPreload);
        
        void setFileFullPath(std::string p_fileFullPath);
        std::string getFileFullPath();
        
        void setLoadingFinished(bool p_finished);
        bool isLoadingFinished();
        
        std::shared_ptr<std::atomic_bool> isDestroyed();
        void setDestroyed(bool p_destroyed);
        
        int getId();
        
        ALuint getAlBufferId();
        
        uint32_t getQueBufferFrames();
        
        ALenum getFormat();
        
        char* getQueBuffer(int p_pos);
        uint32_t getQueBufferSize(int p_pos);
        
        ALsizei getSampleRate();
        std::chrono::milliseconds getDuration();
        
        uint32_t getFramesRead();
        uint32_t getTotalFrames();

    protected:

        void invokingPlayCallbacks();

        void invokingLoadCallbacks();


        // pcm data related stuff
        ALenum _format;
        ALsizei _sampleRate;
        std::chrono::milliseconds _duration;
        uint32_t _totalFrames;
        uint32_t _framesRead;

        /*Cache related stuff;
         * Cache pcm data when sizeInBytes less than PCMDATA_CACHEMAXSIZE
         */
        ALuint _alBufferId;
        char* _pcmData;

        /*Queue buffer related stuff
         *  Streaming in openal when sizeInBytes greater then PCMDATA_CACHEMAXSIZE
         */
        char* _queBuffers[QUEUEBUFFER_NUM];
        ALsizei _queBufferSize[QUEUEBUFFER_NUM];
        uint32_t _queBufferFrames;

        std::mutex _playCallbackMutex;
        std::vector<std::function<void()>> _playCallbacks;

        // loadCallbacks doesn't need mutex since it's invoked only in Cocos thread.
        std::vector<std::function<void(bool)>> _loadCallbacks;

        std::mutex _readDataTaskMutex;

        State _state;
        std::mutex _stateMutex;

        std::mutex _isDestroyedMutex;
        std::shared_ptr<std::atomic_bool> _isDestroyed;
        std::string _fileFullPath;
        unsigned int _id;
        std::atomic_bool _isLoadingFinished;
        std::atomic_bool _isSkipReadDataTask;
        std::atomic_bool _askedAsPreload;
    };

} // namespace experimental
NS_CC_END

#endif
