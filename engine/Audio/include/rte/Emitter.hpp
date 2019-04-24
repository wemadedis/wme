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
class Emitter 
{
public:
    
    Emitter();
    ~Emitter();

    void LoadSoundFromFile(const std::string& path);
    void SetVolume(float volume);
    void SetPosition(glm::vec3 position);
    void SetLoop(bool loop);
    void SetPlayingOffset(float offset);
    RTE::Audio::State GetState();
    void Play();
    void Pause();
    void Stop();

private:

    void setState(State state);

    sf::SoundBuffer _soundBuffer;
    sf::Sound _sound;
    Audio::State _state;
};

}
