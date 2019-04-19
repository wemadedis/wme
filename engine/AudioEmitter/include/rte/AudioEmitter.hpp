#pragma once

#include <string>
#include <iostream>
#include <SFML/Audio.hpp>

class AudioEmitter 
{
    public:
    
    int PlaySound(const std::string& path);
    void PlayTrack();

};
