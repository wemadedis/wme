#include <rte/AudioEmitter.hpp>

#include <iostream>

void log(std::string message){
    std::cout << message << std::endl;
}

int AudioEmitter::PlaySound(const std::string& path)
{
    sf::SoundBuffer buffer;
    buffer.loadFromFile(path);
   
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setPosition(0.f, 0.f, 0.f);
    sound.play();
    while(sound.getStatus() == sf::Sound::Playing){}

    return 0;

}