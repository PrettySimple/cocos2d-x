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

#define LOG_TAG "ALAudioPlayer"

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#import <Foundation/Foundation.h>

#include "audio/apple/ALAudioPlayer.h"
#include "audio/apple/AudioCache.h"
#include "platform/CCFileUtils.h"
#include "audio/apple/AudioDecoder.h"

#ifdef VERY_VERY_VERBOSE_LOGGING
#define ALOGVV ALOGV
#else
#define ALOGVV(...) do{} while(false)
#endif

using namespace cocos2d;
using namespace cocos2d::experimental;

namespace {
    unsigned int __idIndex = 0;
}

ALAudioPlayer::ALAudioPlayer() : AudioPlayer()
, _audioCache(nullptr)
, _finishCallbak(nullptr)
, _rotateBufferThread(nullptr)
, _timeDirty(false)
, _isRotateThreadExited(false)
, _needWakeupRotateThread(false)
, _id(++__idIndex)
{
    ALOGVV("ALAudioPlayer() created (%p), id=%u", this, _id);
    memset(_bufferIds, 0, sizeof(_bufferIds));
}

ALAudioPlayer::~ALAudioPlayer()
{
    ALOGVV("~ALAudioPlayer() (%p), id=%u", this, _id);
    destroy();
    
    if (_streamingSource)
    {
        alDeleteBuffers(QUEUEBUFFER_NUM, _bufferIds);
    }
}

void ALAudioPlayer::destroy()
{
    if (_isDestroyed)
        return;
    
    ALOGVV("ALAudioPlayer::destroy begin, id=%u", _id);
    
    _isDestroyed = true;
    
    do
    {
        if (_audioCache != nullptr)
        {
            if (_audioCache->_state == AudioCache::State::INITIAL)
            {
                ALOGV("ALAudioPlayer::destroy, id=%u, cache isn't ready!", _id);
                break;
            }
            
            while (!_audioCache->_isLoadingFinished)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
        
        // Wait for play2d to be finished.
        _play2dMutex.lock();
        _play2dMutex.unlock();
        
        if (_streamingSource)
        {
            if (_rotateBufferThread != nullptr)
            {
                while (!_isRotateThreadExited)
                {
                    _sleepCondition.notify_one();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                
                if (_rotateBufferThread->joinable()) {
                    _rotateBufferThread->join();
                }
                
                delete _rotateBufferThread;
                _rotateBufferThread = nullptr;
                ALOGVV("rotateBufferThread exited!");
            }
        }
    } while(false);
    
    ALOGVV("Before alSourceStop");
    alSourceStop(_alSource); CHECK_AL_ERROR_DEBUG();
    ALOGVV("Before alSourcei");
    alSourcei(_alSource, AL_BUFFER, 0); CHECK_AL_ERROR_DEBUG();
    ALOGVV("unbinding buffer %u from cache id %d", _audioCache->_alBufferId, _audioCache->_id);
    
    _removeByAudioEngine = true;
    
    _ready = false;
    ALOGVV("ALAudioPlayer::destroy end, id=%u", _id);
}

void ALAudioPlayer::setCache(AudioCache* cache)
{
    _audioCache = cache;
}

bool ALAudioPlayer::play2d()
{
    _play2dMutex.lock();
    ALOGVV("ALAudioPlayer::play2d, _alSource: %u", _alSource);
    
    /*********************************************************************/
    /*       Note that it may be in sub thread or in main thread.       **/
    /*********************************************************************/
    bool ret = false;
    do
    {
        if (_audioCache->_state != AudioCache::State::READY)
        {
            ALOGE("alBuffer isn't ready for play!");
            break;
        }
        
        alSourcei(_alSource, AL_BUFFER, 0);CHECK_AL_ERROR_DEBUG();
        alSourcef(_alSource, AL_PITCH, 1.0f);CHECK_AL_ERROR_DEBUG();
        alSourcef(_alSource, AL_GAIN, _volume);CHECK_AL_ERROR_DEBUG();
        alSourcei(_alSource, AL_LOOPING, AL_FALSE);CHECK_AL_ERROR_DEBUG();
        
        if (_audioCache->_queBufferFrames == 0)
        {
            if (_loop) {
                alSourcei(_alSource, AL_LOOPING, AL_TRUE);
                CHECK_AL_ERROR_DEBUG();
            }
        }
        else
        {
            alGenBuffers(QUEUEBUFFER_NUM, _bufferIds);
            
            auto alError = alGetError();
            if (alError == AL_NO_ERROR)
            {
                for (int index = 0; index < QUEUEBUFFER_NUM; ++index)
                {
                    alBufferData(_bufferIds[index], _audioCache->_format, _audioCache->_queBuffers[index], _audioCache->_queBufferSize[index], _audioCache->_sampleRate);
                }
                CHECK_AL_ERROR_DEBUG();
            }
            else
            {
                ALOGE("%s:alGenBuffers error code:%x", __PRETTY_FUNCTION__,alError);
                break;
            }
            _streamingSource = true;
        }
        
        {
            std::unique_lock<std::mutex> lk(_sleepMutex);
            if (_isDestroyed)
                break;
            
            if (_streamingSource)
            {
                alSourceQueueBuffers(_alSource, QUEUEBUFFER_NUM, _bufferIds);
                CHECK_AL_ERROR_DEBUG();
                _rotateBufferThread = new std::thread(&ALAudioPlayer::rotateBufferThread, this, _audioCache->_queBufferFrames * QUEUEBUFFER_NUM + 1);
            }
            else
            {
                alSourcei(_alSource, AL_BUFFER, _audioCache->_alBufferId);
                
                ALOGE("%s:alSource %u bind buffer :%u", __PRETTY_FUNCTION__, _alSource, _audioCache->_alBufferId);
                
                CHECK_AL_ERROR_DEBUG();
            }
            
            alSourcePlay(_alSource);
        }
        
        auto alError = alGetError();
        if (alError != AL_NO_ERROR)
        {
            ALOGE("%s:alSourcePlay error code:%x", __PRETTY_FUNCTION__,alError);
            break;
        }
        
        ALint state;
        alGetSourcei(_alSource, AL_SOURCE_STATE, &state);
        assert(state == AL_PLAYING);
        _ready = true;
        ret = true;
    } while (false);
    
    if (!ret)
    {
        _removeByAudioEngine = true;
    }
    
    _play2dMutex.unlock();
    return ret;
}

void ALAudioPlayer::rotateBufferThread(int offsetFrame)
{
    char* tmpBuffer = nullptr;
    AudioDecoder decoder;
    do
    {
        BREAK_IF(!decoder.open(_audioCache->_fileFullPath.c_str()));
        
        uint32_t framesRead = 0;
        const uint32_t framesToRead = _audioCache->_queBufferFrames;
        const uint32_t bufferSize = framesToRead * decoder.getBytesPerFrame();
        tmpBuffer = (char*)malloc(bufferSize);
        memset(tmpBuffer, 0, bufferSize);
        
        if (offsetFrame != 0) {
            decoder.seek(offsetFrame);
        }
        
        ALint sourceState;
        ALint bufferProcessed = 0;
        bool needToExitThread = false;
        
        while (!_isDestroyed) {
            alGetSourcei(_alSource, AL_SOURCE_STATE, &sourceState);
            if (sourceState == AL_PLAYING) {
                alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &bufferProcessed);
                while (bufferProcessed > 0) {
                    bufferProcessed--;
                    if (_timeDirty) {
                        _timeDirty = false;
                        offsetFrame = _currTime * decoder.getSampleRate();
                        decoder.seek(offsetFrame);
                    }
                    else {
                        _currTime += QUEUEBUFFER_TIME_STEP;
                        if (_currTime > _audioCache->_duration) {
                            if (_loop) {
                                _currTime = 0.0f;
                            } else {
                                _currTime = _audioCache->_duration;
                            }
                        }
                    }
                    
                    framesRead = decoder.readFixedFrames(framesToRead, tmpBuffer);
                    
                    if (framesRead == 0) {
                        if (_loop) {
                            decoder.seek(0);
                            framesRead = decoder.readFixedFrames(framesToRead, tmpBuffer);
                        } else {
                            needToExitThread = true;
                            break;
                        }
                    }
                    
                    ALuint bid;
                    alSourceUnqueueBuffers(_alSource, 1, &bid);
                    alBufferData(bid, _audioCache->_format, tmpBuffer, framesRead * decoder.getBytesPerFrame(), decoder.getSampleRate());
                    alSourceQueueBuffers(_alSource, 1, &bid);
                }
            }
            
            std::unique_lock<std::mutex> lk(_sleepMutex);
            if (_isDestroyed || needToExitThread) {
                break;
            }
            
            if (!_needWakeupRotateThread)
            {
                _sleepCondition.wait_for(lk,std::chrono::milliseconds(75));
            }
            
            _needWakeupRotateThread = false;
        }
        
    } while(false);
    
    ALOGV("Exit rotate buffer thread ...");
    decoder.close();
    free(tmpBuffer);
    _isRotateThreadExited = true;
}

void ALAudioPlayer::wakeupRotateThread()
{
    _needWakeupRotateThread = true;
    _sleepCondition.notify_all();
}

const std::function<void (int, const std::string &)> ALAudioPlayer::getFinishCallback()
{
    return _finishCallbak;
}


void ALAudioPlayer::setFinishCallback(const std::function<void (int, const std::string &)> &callback)
{
    _finishCallbak = callback;
}

bool ALAudioPlayer::setLoop(bool loop)
{
    if (_ready) {
        if (!_streamingSource) {
            if (loop) {
                alSourcei(_alSource, AL_LOOPING, AL_TRUE);
            } else {
                alSourcei(_alSource, AL_LOOPING, AL_FALSE);
            }
            
            auto error = alGetError();
            if (error != AL_NO_ERROR) {
                ALOGE("%s: player id = %d, error = %x", __PRETTY_FUNCTION__,_id,error);
            }
        }
    }
    
    if (!_isDestroyed ) {
        _loop = loop;
        return true;
    }
    
    return false;
}

void ALAudioPlayer::setVolume(float p_volume)
{
    _volume = p_volume;
    
    if (_ready) {
        alSourcef(_alSource, AL_GAIN, _volume);
        
        auto error = alGetError();
        if (error != AL_NO_ERROR) {
            ALOGE("%s: audio id = %d, error = %x", __PRETTY_FUNCTION__,_id,error);
        }
    }
}

bool ALAudioPlayer::setTime(float time)
{
    bool ret = false;
    if (_ready) {
        if (!_streamingSource)
        {
            if (_audioCache->_framesRead != _audioCache->_totalFrames && (time * _audioCache->_sampleRate) > _audioCache->_framesRead)
                ALOGE("%s: audio id = %d", __PRETTY_FUNCTION__,_id);
            else
            {
                alSourcef(_alSource, AL_SEC_OFFSET, time);
                ret = true;
                
                auto error = alGetError();
                if (error != AL_NO_ERROR) {
                    ALOGE("%s: audio id = %d, error = %x", __PRETTY_FUNCTION__,_id,error);
                }
            }
        }
        else
        {
            if (!_isDestroyed && time >= 0.0f && time < _audioCache->_duration) {
                _currTime = time;
                _timeDirty = true;
                ret = true;
            }
            ret = false;
        }
    }
    return ret;
}

float ALAudioPlayer::getTime()
{
    float ret = _currTime;
    if(_ready){
        if (!_streamingSource) {
            alGetSourcef(_alSource, AL_SEC_OFFSET, &ret);
            
            auto error = alGetError();
            if (error != AL_NO_ERROR) {
                ALOGE("%s, player id:%d,error code:%x", __PRETTY_FUNCTION__,_id,error);
            }
        }
    }
    return ret;
}

bool ALAudioPlayer::pause()
{
    bool ret = true;
    alSourcePause(_alSource);
    
    auto error = alGetError();
    if (error != AL_NO_ERROR) {
        ret = false;
        ALOGE("%s: player id = %d, error = %x", __PRETTY_FUNCTION__,_id,error);
    }
    return ret;
}

bool ALAudioPlayer::resume()
{
    bool ret = true;
    alSourcePlay(_alSource);
    
    auto error = alGetError();
    if (error != AL_NO_ERROR) {
        ret = false;
        ALOGE("%s: player id = %d, error = %x", __PRETTY_FUNCTION__,_id,error);
    }
    return ret;
}

bool ALAudioPlayer::isStopped()
{
    ALint sourceState;
    alGetSourcei(_alSource, AL_SOURCE_STATE, &sourceState);
    
    return _ready && (sourceState == AL_STOPPED);
}

float ALAudioPlayer::getDuration()
{
    if (_audioCache)
        return _audioCache->_duration;
    return 0.f;
}

ALuint ALAudioPlayer::getAlSource()
{
    return _alSource;
}
void ALAudioPlayer::setAlSource(ALuint p_source)
{
    _alSource = p_source;
}




#endif
