#include "rte/AudioManager.hpp"

namespace RTE::Audio
{

AudioManager *AudioManager::_instance = nullptr;

AudioManager::AudioManager()
{
    _instance = this;
}

AudioManager::~AudioManager()
{
}

AudioManager* AudioManager::GetInstance()
{
    if (!_instance) AudioManager();
    return _instance;
}

Emitter* AudioManager::CreateEmitter()
{
    return new Emitter();
}

} // namespace RTE