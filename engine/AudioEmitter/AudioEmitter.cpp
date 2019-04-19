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
    while(true)
    {
        if (sound.getStatus() != sf::Sound::Playing)
        {
            sound.play();
        }
    }

    log(path);

    return 0;

}