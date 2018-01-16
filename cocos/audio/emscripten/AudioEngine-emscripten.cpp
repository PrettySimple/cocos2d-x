#include "audio/emscripten/AudioEngine-emscripten.h"

using namespace cocos2d::experimental;

AudioEngineImpl::AudioEngineImpl()
{
    //TODO EMSCRIPTEN: Implement
}

AudioEngineImpl::~AudioEngineImpl()
{
    //TODO EMSCRIPTEN: Implement
}
    
bool AudioEngineImpl::init()
{
    //TODO EMSCRIPTEN: Implement
    return true;
}

int AudioEngineImpl::play2d(const std::string &fileFullPath ,bool loop ,float volume)
{
    //TODO EMSCRIPTEN: Implement
    return -1;
}

void AudioEngineImpl::setVolume(int audioID,float volume)
{
    //TODO EMSCRIPTEN: Implement
}

void AudioEngineImpl::setLoop(int audioID, bool loop)
{
    //TODO EMSCRIPTEN: Implement
}

bool AudioEngineImpl::pause(int audioID)
{
    //TODO EMSCRIPTEN: Implement
    return true;
}

bool AudioEngineImpl::resume(int audioID)
{
    //TODO EMSCRIPTEN: Implement
    return true;
}

bool AudioEngineImpl::stop(int audioID)
{
    //TODO EMSCRIPTEN: Implement
    return true;
}

void AudioEngineImpl::stopAll()
{
    //TODO EMSCRIPTEN: Implement
}

float AudioEngineImpl::getDuration(int audioID)
{
    //TODO EMSCRIPTEN: Implement
    return -1.f;
}
    
float AudioEngineImpl::getCurrentTime(int audioID)
{
    //TODO EMSCRIPTEN: Implement
    return -1.f;
}

bool AudioEngineImpl::setCurrentTime(int audioID, float time)
{
    //TODO EMSCRIPTEN: Implement
    return true;
}

void AudioEngineImpl::setFinishCallback(int audioID, const std::function<void (int, const std::string &)> &callback)
{
    //TODO EMSCRIPTEN: Implement
}
    
void AudioEngineImpl::uncache(const std::string& filePath)
{
    //TODO EMSCRIPTEN: Implement
}

void AudioEngineImpl::uncacheAll()
{
    //TODO EMSCRIPTEN: Implement
}

int AudioEngineImpl::preload(const std::string& filePath, std::function<void(bool isSuccess)> callback)
{
    //TODO EMSCRIPTEN: Implement
    return -1;
}

int AudioEngineImpl::cancelPreload(const std::string& filePath)
{
    //TODO EMSCRIPTEN: Implement
    return -1;
}

void AudioEngineImpl::update(float dt)
{
    //TODO EMSCRIPTEN: Implement
}
