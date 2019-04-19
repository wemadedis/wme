#include <rte/AudioEmitter.hpp>
#include <string>
#include <iostream>

int main()
{
    const std::string path = "E:\\projects\\rte\\engine\\assets\\audio\\testSound.wav";
    AudioEmitter audio;
    audio.PlaySound(path);
}