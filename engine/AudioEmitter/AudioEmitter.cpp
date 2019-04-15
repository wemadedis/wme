#include <rte/AudioEmitter.hpp>

#include <iostream>

void log(std::string message){
    std::cout << message << std::endl;
}

int AudioEmitter::PlaySound(const std::string& path)
{
    // Open and read data from audio file
    std::string filePath = "E:\\projects\\rte\\engine\\assets\\audiio\\testSoEund.wav";

    wave::File waveFile;
    wave::Error err = waveFile.Open(path, wave::kIn);

    if (err) 
    {
        log("something went wrong while OPENING the file");
        return 1;
    }

    std::vector<float> content;
    err = waveFile.Read(&content);

    if (err)
    {
        log("something went wrong while READING the file");
        return 2;
    }

    std::cout<< waveFile.sample_rate() << std::endl;    


    // setup OpenAL context device and instance
    ALCdevice *device;
    ALCcontext *context;
    
    device = alcOpenDevice(NULL);

    if (device)
    {
        context = alcCreateContext(device, NULL);
        alcMakeContextCurrent(context);
    }

    auto g_bEAX = alIsExtensionPresent("EAX2.0");
    log(alGetError());

    ALuint bufferArray[1];
    alGenBuffers(1, bufferArray);

    log(alGetError());

    //alBufferData(bufferArray[0], AL_FORMAT_STEREO16, ALvoid);

    auto devices = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    std::cout << devices << std::endl;
    return 0;

}