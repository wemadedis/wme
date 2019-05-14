#pragma once

#include "rte/Emitter.hpp"

namespace RTE::Audio
{

class AudioManager
{
private:
    static AudioManager *_instance;

public:    
    AudioManager();
    ~AudioManager();
    
    static AudioManager *GetInstance();
    Emitter* CreateEmitter();
};
} // namespace RTE
