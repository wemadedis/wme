#pragma once

#include <al.h>
#include <alc.h>
// #include <alc

#include <string>
#include <iostream>
#include <wave/file.h>
// #include <file.h>

class AudioEmitter 
{
    public:
    
    int PlaySound(const std::string& path);
    void PlayTrack();

};
