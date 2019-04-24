#include "rte/Emitter.hpp"

namespace RTE::Audio{

Emitter::Emitter()
{
    
}

void Emitter::setState(Audio::State state)
{
    _state = state;
}


RTE::Audio::State Emitter::GetState()
{
    sf::Sound::Status status = _sound.getStatus();
    switch (status)
    {
        case sf::Sound::Stopped : return RTE::Audio::Stopped;
        case sf::Sound::Paused :  return RTE::Audio::Paused;
        case sf::Sound::Playing : return RTE::Audio::Playing;
    }

    return RTE::Audio::Stopped;
}


void Emitter::LoadSoundFromFile(const std::string& path)
{
    _soundBuffer.loadFromFile(path);
    _sound.setBuffer(_soundBuffer);
}

void Emitter::SetVolume(float volume)
{
    _sound.setVolume(volume);   
}

void Emitter::SetPosition(glm::vec3 position)
{
    _sound.setPosition(position.x, position.y, position.z);
}

void Emitter::SetLoop(bool loopBool)
{
    _sound.setLoop(loopBool);
}

void Emitter::SetPlayingOffset(float offset)
{
    _sound.setPlayingOffset(sf::seconds(offset));
}

void Emitter::Play()
{
    _sound.play();
    setState(Playing);
}

void Emitter::Pause()
{
    _sound.pause();
    setState(Paused);
}

void Emitter::Stop()
{
    _sound.stop();
    setState(Stopped);
}

}