#pragma once

#include <string>
#include <iostream>

#include "glm/glm.hpp"
#include "SFML/Audio.hpp"

#include "rte/RTEException.h"

namespace RTE::Audio{
    enum State
    {
        Stopped, ///< Sound is not playing
        Paused,  ///< Sound is paused
        Playing  ///< Sound is playing
    };
class AudioEmitter 
{
public:

    void TEST(const std::string& path);
    
    void LoadSoundFromFile(const std::string& path);
    void SetVolume(float);
    void SetPosition(glm::vec3);
    void SetLoop(bool);
    void SetPlayingOffset(float);
    RTE::Audio::State GetState();
    void Play();
    void Pause();
    void Stop();

private:

    void setState(State);

    sf::SoundBuffer soundBuffer;
    sf::Sound sound;
    RTE::Audio::State state;
};

}
