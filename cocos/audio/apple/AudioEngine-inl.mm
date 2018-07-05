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

#define LOG_TAG "AudioEngine-inl.mm"

#include "platform/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC

#    if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#        import <UIKit/UIKit.h>
#    elif CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#        import <AppKit/AppKit.h>
#    endif

#    include "audio/apple/AudioEngine-inl.h"

#    import <AVFoundation/AVFoundation.h>
#    import <OpenAL/alc.h>

#    include "audio/include/AudioEngine.h"
#    include "audio/include/SimpleAudioEngine.h"
#    include "base/CCDirector.h"
#    include "base/CCScheduler.h"
#    include "base/ccUtils.h"
#    include "platform/CCFileUtils.h"

#    include <utility>

using namespace cocos2d;
using namespace cocos2d::experimental;

static ALCdevice* s_ALDevice = nullptr;
static ALCcontext* s_ALContext = nullptr;
static AudioEngineImpl* s_instance = nullptr;

typedef ALvoid (*alSourceNotificationProc)(ALuint sid, ALuint notificationID, ALvoid* userData);
typedef ALenum (*alSourceAddNotificationProcPtr)(ALuint sid, ALuint notificationID, alSourceNotificationProc notifyProc, ALvoid* userData);
static ALenum alSourceAddNotificationExt(ALuint sid, ALuint notificationID, alSourceNotificationProc notifyProc, ALvoid* userData)
{
    static alSourceAddNotificationProcPtr proc = nullptr;

    if (proc == nullptr)
    {
        proc = (alSourceAddNotificationProcPtr)alcGetProcAddress(nullptr, "alSourceAddNotification");
    }

    if (proc)
    {
        return proc(sid, notificationID, notifyProc, userData);
    }
    return AL_INVALID_VALUE;
}

#    if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
@interface AudioEngineSessionHandler : NSObject {
}

- (id)init;
- (void)handleInterruption:(NSNotification*)notification;

@end

@implementation AudioEngineSessionHandler

// only enable it on iOS. Disable it on tvOS
#        if !defined(CC_TARGET_OS_TVOS)
void AudioEngineInterruptionListenerCallback(void* user_data, UInt32 interruption_state)
{
    if (kAudioSessionBeginInterruption == interruption_state)
    {
        alcMakeContextCurrent(nullptr);
    }
    else if (kAudioSessionEndInterruption == interruption_state)
    {
        OSStatus result = AudioSessionSetActive(true);
        if (result)
            NSLog(@"Error setting audio session active! %d\n", static_cast<int>(result));

        alcMakeContextCurrent(s_ALContext);
    }
}
#        endif

- (id)init
{
    if (self = [super init])
    {
        if ([[[UIDevice currentDevice] systemVersion] intValue] > 5)
        {
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(handleInterruption:)
                                                         name:AVAudioSessionInterruptionNotification
                                                       object:[AVAudioSession sharedInstance]];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(handleInterruption:)
                                                         name:UIApplicationDidBecomeActiveNotification
                                                       object:nil];
            [[NSNotificationCenter defaultCenter] addObserver:self
                                                     selector:@selector(handleInterruption:)
                                                         name:UIApplicationWillResignActiveNotification
                                                       object:nil];
        }
        // only enable it on iOS. Disable it on tvOS
        // AudioSessionInitialize removed from tvOS
#        if !defined(CC_TARGET_OS_TVOS)
        else
        {
            AudioSessionInitialize(NULL, NULL, AudioEngineInterruptionListenerCallback, self);
        }
#        endif

        BOOL success = [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:nil];
        if (!success)
            ALOGE("Fail to set audio session.");
    }
    return self;
}

- (void)handleInterruption:(NSNotification*)notification
{
    static bool isAudioSessionInterrupted = false;
    static bool resumeOnBecomingActive = false;
    static bool pauseOnResignActive = false;

    if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification])
    {
        NSInteger reason = [[[notification userInfo] objectForKey:AVAudioSessionInterruptionTypeKey] integerValue];
        if (reason == AVAudioSessionInterruptionTypeBegan)
        {
            isAudioSessionInterrupted = true;

            if ([UIApplication sharedApplication].applicationState != UIApplicationStateActive)
            {
                ALOGD("AVAudioSessionInterruptionTypeBegan, application != UIApplicationStateActive, alcMakeContextCurrent(nullptr)");
                alcMakeContextCurrent(nullptr);
            }
            else
            {
                ALOGD("AVAudioSessionInterruptionTypeBegan, application == UIApplicationStateActive, pauseOnResignActive = true");
                pauseOnResignActive = true;
            }
        }

        if (reason == AVAudioSessionInterruptionTypeEnded)
        {
            isAudioSessionInterrupted = false;

            if ([UIApplication sharedApplication].applicationState == UIApplicationStateActive)
            {
                ALOGD("AVAudioSessionInterruptionTypeEnded, application == UIApplicationStateActive, alcMakeContextCurrent(s_ALContext)");
                NSError* error = nil;
                [[AVAudioSession sharedInstance] setActive:YES error:&error];
                alcMakeContextCurrent(s_ALContext);
                if (Director::getInstance()->isPaused())
                {
                    ALOGD("AVAudioSessionInterruptionTypeEnded, director was paused, try to resume it.");
                    Director::getInstance()->resume();
                }
            }
            else
            {
                ALOGD("AVAudioSessionInterruptionTypeEnded, application != UIApplicationStateActive, resumeOnBecomingActive = true");
                resumeOnBecomingActive = true;
            }
        }
    }
    else if ([notification.name isEqualToString:UIApplicationWillResignActiveNotification])
    {
        ALOGD("UIApplicationWillResignActiveNotification");
        if (pauseOnResignActive)
        {
            pauseOnResignActive = false;
            ALOGD("UIApplicationWillResignActiveNotification, alcMakeContextCurrent(nullptr)");
            alcMakeContextCurrent(nullptr);
        }
    }
    else if ([notification.name isEqualToString:UIApplicationDidBecomeActiveNotification])
    {
        ALOGD("UIApplicationDidBecomeActiveNotification");
        if (resumeOnBecomingActive)
        {
            resumeOnBecomingActive = false;
            ALOGD("UIApplicationDidBecomeActiveNotification, alcMakeContextCurrent(s_ALContext)");
            NSError* error = nil;
            BOOL success = [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryAmbient error:&error];
            if (!success)
            {
                ALOGE("Fail to set audio session.");
                return;
            }
            [[AVAudioSession sharedInstance] setActive:YES error:&error];
            alcMakeContextCurrent(s_ALContext);
        }
        else if (isAudioSessionInterrupted)
        {
            ALOGD("Audio session is still interrupted, pause director!");
            Director::getInstance()->pause();
        }
    }
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:AVAudioSessionInterruptionNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIApplicationWillResignActiveNotification object:nil];

    [super dealloc];
}
@end

static id s_AudioEngineSessionHandler = nullptr;
#    endif

ALvoid AudioEngineImpl::myAlSourceNotificationCallback(ALuint sid, ALuint notificationID, ALvoid* userData)
{
    // Currently, we only care about AL_BUFFERS_PROCESSED event
    if (notificationID != AL_BUFFERS_PROCESSED)
        return;

    AudioPlayer* player = nullptr;
    s_instance->_threadMutex.lock();
    for (const auto& e : s_instance->_audioPlayers)
    {
        player = e.second;
        if (player->getAlSource() == sid && player->isStreamingSource())
        {
            player->wakeupRotateThread();
        }
    }
    s_instance->_threadMutex.unlock();
}

AudioEngineImpl::AudioEngineImpl()
: _lazyInitLoop(true)
, _currentAudioID(0)
, _scheduler(nullptr)
{
    s_instance = this;
}

AudioEngineImpl::~AudioEngineImpl()
{
    if (_scheduler != nullptr)
    {
        _scheduler->unschedule(CC_SCHEDULE_SELECTOR(AudioEngineImpl::update), this);
    }

    if (s_ALContext)
    {
        alDeleteSources(MAX_AUDIOINSTANCES, _alSources);

        _audioCaches.clear();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(s_ALContext);
    }
    if (s_ALDevice)
    {
        alcCloseDevice(s_ALDevice);
    }

#    if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    [s_AudioEngineSessionHandler release];
#    endif
    s_instance = nullptr;
}

bool AudioEngineImpl::init()
{
    bool ret = false;
    do
    {
#    if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        s_AudioEngineSessionHandler = [[AudioEngineSessionHandler alloc] init];
#    endif

        s_ALDevice = alcOpenDevice(nullptr);

        if (s_ALDevice)
        {
            s_ALContext = alcCreateContext(s_ALDevice, nullptr);
            alcMakeContextCurrent(s_ALContext);

            alGenSources(MAX_AUDIOINSTANCES, _alSources);
            auto alError = alGetError();
            if (alError != AL_NO_ERROR)
            {
                ALOGE("%s:generating sources failed! error = %x", __PRETTY_FUNCTION__, alError);
                break;
            }

            for (int i = 0; i < MAX_AUDIOINSTANCES; ++i)
            {
                _unusedSourcesPool.push_back(_alSources[i]);
                alSourceAddNotificationExt(_alSources[i], AL_BUFFERS_PROCESSED, myAlSourceNotificationCallback, nullptr);
            }

            // fixed #16170: Random crash in alGenBuffers(AudioCache::readDataTask) at startup
            // Please note that, as we know the OpenAL operation is atomic (threadsafe),
            // 'alGenBuffers' may be invoked by different threads. But in current implementation of 'alGenBuffers',
            // When the first time it's invoked, application may crash!!!
            // Why? OpenAL is opensource by Apple and could be found at
            // http://opensource.apple.com/source/OpenAL/OpenAL-48.7/Source/OpenAL/oalImp.cpp .
            /*

            void InitializeBufferMap()
            {
                if (gOALBufferMap == NULL) // Position 1
                {
                    gOALBufferMap = new OALBufferMap ();  // Position 2

                    // Position Gap

                    gBufferMapLock = new CAGuard("OAL:BufferMapLock"); // Position 3
                    gDeadOALBufferMap = new OALBufferMap ();

                    OALBuffer   *newBuffer = new OALBuffer (AL_NONE);
                    gOALBufferMap->Add(AL_NONE, &newBuffer);
                }
            }

            AL_API ALvoid AL_APIENTRY alGenBuffers(ALsizei n, ALuint *bids)
            {
                ...

                try {
                    if (n < 0)
                    throw ((OSStatus) AL_INVALID_VALUE);

                    InitializeBufferMap();
                    if (gOALBufferMap == NULL)
                    throw ((OSStatus) AL_INVALID_OPERATION);

                    CAGuard::Locker locked(*gBufferMapLock);  // Position 4
                ...
                ...
            }

             */
            // 'gBufferMapLock' will be initialized in the 'InitializeBufferMap' function,
            // that's the problem. It means that 'InitializeBufferMap' may be invoked in different threads.
            // It will be very dangerous in multi-threads environment.
            // Imagine there're two threads (Thread A, Thread B), they call 'alGenBuffers' simultaneously.
            // While A goto 'Position Gap', 'gOALBufferMap' was assigned, then B goto 'Position 1' and find
            // that 'gOALBufferMap' isn't NULL, B just jump over 'InitialBufferMap' and goto 'Position 4'.
            // Meanwhile, A is still at 'Position Gap', B will crash at '*gBufferMapLock' since 'gBufferMapLock'
            // is still a null pointer. Oops, how could Apple implemented this method in this fucking way?

            // Workaround is do an unused invocation in the mainthread right after OpenAL is initialized successfully
            // as bellow.
            // ================ Workaround begin ================ //

            ALuint unusedAlBufferId = 0;
            alGenBuffers(1, &unusedAlBufferId);
            alDeleteBuffers(1, &unusedAlBufferId);

            // ================ Workaround end ================ //

            _scheduler = Director::getInstance()->getScheduler();
            ret = true;
            ALOGI("OpenAL was initialized successfully!");
        }
    } while (false);

    return ret;
}

AudioCache& AudioEngineImpl::preload(std::string const& filePath, std::function<void(bool)> const& callback)
{
    auto it = _audioCaches.find(filePath);
    if (it == _audioCaches.end())
    {
        auto& audioCache = _audioCaches[filePath];
        audioCache._askedAsPreload = true;
        if (callback != nullptr)
        {
            audioCache.addLoadCallback(callback);
        }
        audioCache._fileFullPath = FileUtils::getInstance()->fullPathForFilename(filePath);

        AudioEngine::addTask([&audioCache, cacheId = audioCache._id, isCacheDestroyed = audioCache._isDestroyed]() {
            if (*isCacheDestroyed)
            {
                ALOGV("AudioCache (id=%u) was destroyed, no need to launch readDataTask.", cacheId);
                audioCache.setSkipReadDataTask(true);
            }
            else
            {
                audioCache.readDataTask(cacheId);
            }
        });
        return audioCache;
    }
    else
    {
        it->second._askedAsPreload = true;
        if (callback != nullptr)
        {
            it->second.addLoadCallback(callback);
        }

        return it->second;
    }

    CC_ASSERT(false);
    static AudioCache empty;
    return empty;
}

void AudioEngineImpl::_play2d(AudioPlayer* player, AudioCache& audioCache, bool loop, float volume)
{
    player->setLoop(loop);
    player->setVolume(volume);

    _threadMutex.lock();
    _audioPlayers[_currentAudioID] = player;
    _threadMutex.unlock();

    audioCache.addPlayCallback([this, &audioCache, audioId = _currentAudioID, isCacheDestroyed = audioCache._isDestroyed]() { _play2d(audioCache, audioId, *isCacheDestroyed); });

    if (_lazyInitLoop)
    {
        _lazyInitLoop = false;
        _scheduler->schedule(CC_SCHEDULE_SELECTOR(AudioEngineImpl::update), this, std::chrono::milliseconds(5), false);
    }
}

int AudioEngineImpl::play2d(std::string const& filePath, bool loop, float volume, bool isMusic)
{
    if (s_ALDevice == nullptr)
    {
        return AudioEngine::INVALID_AUDIO_ID;
    }

    // sound is a music and was not preloaded, we use the hardware accelerated
    // player, otherwise just use openAL
    if (isMusic && (_audioCaches.count(filePath) == 0 || !(_audioCaches.find(filePath)->second._askedAsPreload)))
    {
        AudioPlayer* player = new (std::nothrow) SimpleAudioPlayer(filePath);

        auto it = _audioCaches.find(filePath);
        if (it == _audioCaches.end())
        {
            auto& audioCache = _audioCaches[filePath];
            audioCache._fileFullPath = FileUtils::getInstance()->fullPathForFilename(filePath);
            audioCache.setState(AudioCache::State::READY);
            audioCache._isLoadingFinished = true;

            _play2d(player, audioCache, loop, volume);
        }
        else
        {
            auto& audioCache = it->second;
            audioCache.setState(AudioCache::State::READY);
            audioCache._isLoadingFinished = true;

            _play2d(player, audioCache, loop, volume);
        }
    }
    else
    {
        ALuint alSource = findValidSource();
        if (alSource == AL_INVALID)
        {
            return AudioEngine::INVALID_AUDIO_ID;
        }

        auto& audioCache = preload(filePath, nullptr);

        AudioPlayer* player = new (std::nothrow) ALAudioPlayer(audioCache);
        player->setAlSource(alSource);
        if (player == nullptr)
        {
            return AudioEngine::INVALID_AUDIO_ID;
        }

        _play2d(player, audioCache, loop, volume);
    }

    return _currentAudioID++;
}

void AudioEngineImpl::_play2d(AudioCache& cache, int audioID, bool isCacheDestroyed)
{
    // Note: It may bn in sub thread or main thread :(
    if (!isCacheDestroyed && cache.getState() == AudioCache::State::READY)
    {
        _threadMutex.lock();
        auto playerIt = _audioPlayers.find(audioID);
        if (playerIt != _audioPlayers.end() && playerIt->second->play2d())
        {
            _scheduler->performFunctionInCocosThread([audioID]() {
                if (AudioEngine::_audioIDInfoMap.find(audioID) != AudioEngine::_audioIDInfoMap.end())
                {
                    AudioEngine::_audioIDInfoMap[audioID].state = AudioEngine::AudioState::PLAYING;
                }
            });
        }
        _threadMutex.unlock();
    }
    else
    {
        ALOGD("AudioEngineImpl::_play2d, cache was destroyed or not ready!");
        auto iter = _audioPlayers.find(audioID);
        if (iter != _audioPlayers.end())
        {
            iter->second->setRemovedByEngine(true);
        }
    }
}

ALuint AudioEngineImpl::findValidSource()
{
    ALuint sourceId = AL_INVALID;
    if (!_unusedSourcesPool.empty())
    {
        sourceId = _unusedSourcesPool.front();
        _unusedSourcesPool.pop_front();
    }

    return sourceId;
}

void AudioEngineImpl::setVolume(int audioID, float volume)
{
    auto player = _audioPlayers[audioID];
    player->setVolume(volume);
}

void AudioEngineImpl::setLoop(int audioID, bool loop)
{
    auto player = _audioPlayers[audioID];
    player->setLoop(loop);
}

bool AudioEngineImpl::pause(int audioID)
{
    auto player = _audioPlayers[audioID];
    return player->pause();
}

bool AudioEngineImpl::resume(int audioID)
{
    auto player = _audioPlayers[audioID];
    return player->resume();
}

void AudioEngineImpl::stop(int audioID)
{
    auto player = _audioPlayers[audioID];
    player->destroy();

    // Call 'update' method to cleanup immediately since the schedule may be cancelled without any notification.
    update(0.0f);
}

void AudioEngineImpl::stopAll()
{
    for (auto&& player : _audioPlayers)
    {
        player.second->destroy();
    }

    // Call 'update' method to cleanup immediately since the schedule may be cancelled without any notification.
    update(0.0f);
}

std::chrono::milliseconds AudioEngineImpl::getDuration(int audioID)
{
    auto player = _audioPlayers[audioID];
    if (player->isReady())
    {
        return player->getDuration();
    }
    else
    {
        return AudioEngine::TIME_UNKNOWN;
    }
}

float AudioEngineImpl::getCurrentTime(int audioID)
{
    auto player = _audioPlayers[audioID];
    return player->getTime();
}

bool AudioEngineImpl::setCurrentTime(int audioID, float time)
{
    auto player = _audioPlayers[audioID];
    return player->setTime(time);
}

void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void(int, const std::string&)>& callback)
{
    _audioPlayers[audioID]->setFinishCallback(callback);
}

void AudioEngineImpl::update(float dt)
{
    int audioID;
    AudioPlayer* player;
    ALuint alSource;

    //    ALOGV("AudioPlayer count: %d", (int)_audioPlayers.size());

    for (auto it = _audioPlayers.begin(); it != _audioPlayers.end();)
    {
        audioID = it->first;
        player = it->second;
        alSource = player->getAlSource();

        if (player->isRemovedByEngine())
        {
            AudioEngine::remove(audioID);
            _threadMutex.lock();
            it = _audioPlayers.erase(it);
            _threadMutex.unlock();
            delete player;
            if (alSource != AL_INVALID)
                _unusedSourcesPool.push_back(alSource);
        }
        else if (player->isStopped())
        {
            std::string filePath;
            if (player->getFinishCallback())
            {
                auto& audioInfo = AudioEngine::_audioIDInfoMap[audioID];
                filePath = audioInfo.filePath;
            }

            AudioEngine::remove(audioID);
            _threadMutex.lock();
            it = _audioPlayers.erase(it);
            _threadMutex.unlock();

            if (player->getFinishCallback())
            {
                (player->getFinishCallback())(audioID, filePath); // FIXME: callback will delay 50ms
            }

            delete player;
            if (alSource != AL_INVALID)
                _unusedSourcesPool.push_back(alSource);
        }
        else
        {
            ++it;
        }
    }

    if (_audioPlayers.empty())
    {
        _lazyInitLoop = true;
        _scheduler->unschedule(CC_SCHEDULE_SELECTOR(AudioEngineImpl::update), this);
    }
}

void AudioEngineImpl::uncache(const std::string& filePath)
{
    _audioCaches.erase(filePath);
}

void AudioEngineImpl::uncacheAll()
{
    _audioCaches.clear();
}

#endif
