#include <rte/AudioEmitter.hpp>

#include <iostream>

void log(std::string message){
    std::cout << message << std::endl;
}

void AudioEmitter::PlaySound(const std::string& path)
{
    ALCdevice *device;
    device = alcOpenDevice(NULL);

    ALCcontext *context;

    if (device)
    {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }

    auto devices = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    std::cout << devices << std::endl;

}