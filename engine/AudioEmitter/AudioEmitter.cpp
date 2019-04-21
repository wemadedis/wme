#include <rte/AudioEmitter.hpp>

void log(std::string message){
    std::cout << message << std::endl;
}

namespace RTE::Audio{

// AudioEmitter::AudioEmitter() : state{Stopped}
// {

// }

void AudioEmitter::setState(State _state)
{
    state = _state;
}

AudioEmitter::State AudioEmitter::getState()
{
    sf::Sound::Status status = sound.getStatus();
    switch (status)
    {
        case sf::Sound::Stopped : return Stopped;
        case sf::Sound::Paused :  return Paused;
        case sf::Sound::Playing : return Playing;
    }
}

void AudioEmitter::TEST(const std::string& path)
{
    sf::SoundBuffer buffer;
    buffer.loadFromFile(path);
   
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setPosition(0.f, 0.f, 0.f);
    sound.play();
    while(sound.getStatus() == sf::Sound::Playing){}
}

void AudioEmitter::LoadSoundFromFile(const std::string& path)
{
    soundBuffer.loadFromFile(path);
    sound.setBuffer(soundBuffer);
}

void AudioEmitter::SetVolume(float volume)
{
    sound.setVolume(volume);   
}

void AudioEmitter::SetPosition(glm::vec3 position)
{
    sound.setPosition(position.x, position.y, position.z);
}

void AudioEmitter::SetLoop(bool loopBool)
{
    sound.setLoop(loopBool);
}

void AudioEmitter::SetPlayingOffset(float offset)
{
    sound.setPlayingOffset(sf::seconds(offset));
}

void AudioEmitter::Play()
{
    sound.play();
    setState(Playing);
}

void AudioEmitter::Pause()
{
    sound.pause();
    setState(Paused);
}

void AudioEmitter::Stop()
{
    sound.stop();
    setState(Stopped);
}

}