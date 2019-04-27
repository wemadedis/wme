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


    /**
     * @brief Load sound from soundfile
     *  
     * @param path Path to the soundfile
     */
    void LoadSoundFromFile(const std::string& path);

    /**
    * @brief Set volume
    * 
    * @param volume Value range from 0.f - 100.f
    */
    void SetVolume(float volume);

    /**
    * @brief Sets the position for the sound emitter
    * 
    * @details
    * Positional audio works only with mono sounds, this method is ignored if the
    * loaded file is a stereo sound file
    * 
    * @params position A glm::vec3 defining the x, y, z coordinates
    */
    void SetPosition(glm::vec3 position);

    /**
    * @brief Enables or disables looping of audio
    *
    * @params loop bool false by default, if true, audio loops indefinitely or until
    * stopped by Stop()/Pause()
    */
    void SetLoop(bool loop);

    /**
    * @brief Sets playback offset
    * 
    * @details
    * The offset specifies from which second should the audio start playing, i.e
    * if the offset is set to 2.0 the sound starts playing from the second second
    * of the audiofile
    * 
    * @params
    * offset seconds
    */
    void SetPlayingOffset(float offset);

    /**
    * @brief Gets the playback state of the emitter
    * 
    * @return RTE::Audio::State A state enum with the values of Stopped, Paused
    * or Playing
    */
    RTE::Audio::State GetState();

    /**
    * @brief Starts audio playback
    */
    void Play();

    /**
    * @brief Pauses audio playback
    */
    void Pause();

    /**
    * @brief Stopps audio playback (resets the audio to 0 seconds)
    */
    void Stop();

private:

    /**
    * @brief sets the audio state
    * 
    * @details this methiod is being called in Play(), Stop(), Pause()
    */
    void SetState(State state);

    /**
    * @brief SFML sound buffer
    */
    sf::SoundBuffer _soundBuffer;
    
    /**
    * @brief SFML SoundObj
    */
    sf::Sound _sound;

    /**
    * @brief local playback state
    */
    Audio::State _state;
};

}
