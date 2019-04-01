#pragma once
#define AL_Withthesource

#include <al.h>
#include <alc.h>
#include <string>
#include <iostream>

class AudioEmitter 
{
    public:
    
    void PlaySound(const std::string& path);
    void PlayTrack();

};