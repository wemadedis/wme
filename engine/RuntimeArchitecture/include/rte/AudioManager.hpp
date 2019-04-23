#pragma once

#include "rte/AudioListener.hpp"

namespace RTE::Runtime
{
// Does this make even sense?[]
class AudioManager
{

public:
    static RTE::Audio::AudioListener GetAudioListener(); 
};

} // namespace RTE
