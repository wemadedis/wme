#pragma once

#include "rte/AudioListener.hpp"

namespace RTE::Audio
{
// Does this make even sense?[]
class AudioManager
{

public:
    static AudioListener GetAudioListener(); 
};

} // namespace RTE
