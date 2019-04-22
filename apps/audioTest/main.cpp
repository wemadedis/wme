#include <iostream>
#include <string>

#include "rte/AudioEmitter.hpp"


int main()
{
    const std::string path = "E:\\projects\\rte\\engine\\assets\\audio\\stereo.wav";
    RTE::Audio::AudioEmitter audio;

    audio.LoadSoundFromFile(path);
    audio.Play();

    while(audio.GetState() == RTE::Audio::State::Playing){}
}