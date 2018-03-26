//
//  SimpleAudioPlayer.cmm
//  cocos2d_libs
//
//  Created by Guilhem Vors on 20/03/2018.
//

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
    AudioPlayer::destroy();
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
