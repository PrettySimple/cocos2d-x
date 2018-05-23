//
//  SimpleAudioPlayer.hpp
//  cocos2d_libs
//
//  Created by Guilhem Vors on 20/03/2018.
//

#ifndef SimpleAudioPlayer_h
#define SimpleAudioPlayer_h

#include "AudioPlayer.h"

#include <chrono>

NS_CC_BEGIN
namespace experimental{
class SimpleAudioPlayer : public cocos2d::experimental::AudioPlayer
{
private:
    std::string _filePath;
public:
    SimpleAudioPlayer(std::string filePath);
    virtual ~SimpleAudioPlayer();
    
    void destroy() override;
    bool play2d() override;
    
    bool pause() override;
    bool resume() override;
    
    bool setLoop(bool loop) override;
    void setVolume(float volume) override;
    
    float getTime() override;
    bool setTime(float time) override;
    std::chrono::milliseconds getDuration() override;
    bool isStopped() override;
    
};
}
NS_CC_END
#endif /* SimpleAudioPlayer_h */
