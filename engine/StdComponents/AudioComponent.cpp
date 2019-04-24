#include "rte/AudioComponent.hpp"
#include "rte/AudioManager.hpp"

namespace RTE::StdComponents
{

AudioComponent::AudioComponent()
{}

void AudioComponent::Initialize(
    const std::string& filePath,
    TransformComponent *transformComponent,
    float volume,
    float soundOffset)
{
    using namespace Audio;
    AudioManager *audioManager = AudioManager::GetInstance();
    _transformComponent = transformComponent;

    _emitter = audioManager->CreateEmitter();
    _emitter->LoadSoundFromFile(filePath);
    _emitter->SetVolume(volume);
    _emitter->SetPlayingOffset(soundOffset);
}


void AudioComponent::Update(float deltaTime)
{
    _emitter->SetPosition(_transformComponent->Transform.Pos);
}

Audio::Emitter *AudioComponent::GetEmitter()
{
    return _emitter;
}

void AudioComponent::Play()
{
    _emitter->Play();
}

void AudioComponent::Pause()
{
    _emitter->Pause();
}

void AudioComponent::Stop()
{
    _emitter->Stop();
}

void AudioComponent::SetVolume(float volume)
{
    _emitter->SetVolume(volume);
}

Audio::State AudioComponent::GetState()
{
    return _emitter->GetState();
}

}; // namespace RTE::StdComponents