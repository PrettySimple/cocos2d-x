

#include "AudioPlayer.h"



using namespace cocos2d;
using namespace cocos2d::experimental;

AudioPlayer::AudioPlayer()
: _isDestroyed(false)
, _removeByAudioEngine(false)
, _ready(false)
, _loop(false)
, _volume(1.f)
, _currTime(0.0f)
, _streamingSource(false)
{
    
}

AudioPlayer::~AudioPlayer()
{
}



bool AudioPlayer::isStreamingSource()
{
    return _streamingSource;
}

bool AudioPlayer::isRemovedByEngine()
{
    return _removeByAudioEngine;
}
