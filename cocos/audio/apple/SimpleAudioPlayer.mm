//
//  SimpleAudioPlayer.cmm
//  cocos2d_libs
//
//  Created by Guilhem Vors on 20/03/2018.
//

#define LOG_TAG "SimpleAudioPlayer.mm"

#include "SimpleAudioPlayer.h"
#include "AudioCache.h"
#include "audio/include/SimpleAudioEngine.h"


using namespace cocos2d;
using namespace cocos2d::experimental;

SimpleAudioPlayer::SimpleAudioPlayer(std::string filePath) : AudioPlayer()
, _filePath(filePath)
{
    
}

SimpleAudioPlayer::~SimpleAudioPlayer()
{
    
}

void SimpleAudioPlayer::destroy()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    
    if (_isDestroyed)
        return;
    
    _isDestroyed = true;
    
    do
    {
        if (_audioCache != nullptr)
        {
            if (_audioCache->_state == AudioCache::State::INITIAL)
            {
                ALOGV("AudioPlayer::destroy, id=%u, cache isn't ready!", _id);
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
            }
        }
    } while(false);
    
    _removeByAudioEngine = true;
    _ready = false;
}

bool SimpleAudioPlayer::play2d()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
    _ready = true;
    _streamingSource = true;
    return true;
}

bool SimpleAudioPlayer::setLoop(bool loop) {
    _loop = loop;
    if (!_isDestroyed)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
        return true;
    }
    return false;
}
void SimpleAudioPlayer::setVolume(float volume) {
    _volume = volume;
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

void SimpleAudioPlayer::pause() {
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SimpleAudioPlayer::resume() {
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}

float SimpleAudioPlayer::getTime()
{
    ALOGE("Getting current time for music on IOS is not supported.");
    return 0.f;
}

bool SimpleAudioPlayer::setTime(float time)
{
    ALOGE("Setting time for music on IOS is not supported.");
    return false;
}

float SimpleAudioPlayer::getDuration()
{
    return CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicDuration();
}

bool SimpleAudioPlayer::isStopped()
{
    return !CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}
