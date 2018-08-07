//
//  SimpleAudioPlayer.cmm
//  cocos2d_libs
//
//  Created by Guilhem Vors on 20/03/2018.
//

#include <cocos/audio/apple/SimpleAudioPlayer.h>

#include <cocos/audio/apple/AudioMacros.h>
#include <cocos/audio/apple/AudioPlayer.h>
#include <cocos/audio/include/SimpleAudioEngine.h>
#include <cocos/base/CCConsole.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <iosfwd>
#include <string>

using namespace cocos2d;
using namespace cocos2d::experimental;

SimpleAudioPlayer::SimpleAudioPlayer(std::string filePath)
: AudioPlayer()
, _filePath(filePath)
{
    _streamingSource = true;
}

SimpleAudioPlayer::~SimpleAudioPlayer()
{
}

void SimpleAudioPlayer::destroy()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
    _isDestroyed = true;
}

bool SimpleAudioPlayer::play2d()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
    _ready = true;
    return true;
}

bool SimpleAudioPlayer::setLoop(bool loop)
{
    _loop = loop;
    if (!_isDestroyed)
    {
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(_filePath.c_str(), _loop);
        return true;
    }
    return false;
}
void SimpleAudioPlayer::setVolume(float volume)
{
    _volume = volume;
    CocosDenshion::SimpleAudioEngine::getInstance()->setBackgroundMusicVolume(volume);
}

bool SimpleAudioPlayer::pause()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    return true;
}

bool SimpleAudioPlayer::resume()
{
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    return true;
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

std::chrono::milliseconds SimpleAudioPlayer::getDuration()
{
    return std::chrono::milliseconds(static_cast<std::size_t>(CocosDenshion::SimpleAudioEngine::getInstance()->getBackgroundMusicDuration() * 1000.0));
}

bool SimpleAudioPlayer::isStopped()
{
    return !CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}
