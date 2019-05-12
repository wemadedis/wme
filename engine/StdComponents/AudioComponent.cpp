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
    state = State::Stopped;
}


void AudioComponent::Update(float deltaTime)
{
    using namespace RTE::Audio;
    _emitter->SetPosition(_transformComponent->Transform.Pos);

    // switch (state)
    // {
    //     case State::Playing:
    //         _emitter->Play();
    //         break;
    //     case State::Paused:
    //         _emitter->Pause();
    //         break;
    //     case State::Stopped:
    //         _emitter->Stop();
    //         break;
    // }
}

Audio::Emitter *AudioComponent::GetEmitter()
{
    return _emitter;
}

void AudioComponent::Play()
{
    state = RTE::Audio::State::Playing;
    _emitter->Play();
}

void AudioComponent::Pause()
{
    state = RTE::Audio::State::Paused;
    _emitter->Pause();
}

void AudioComponent::Stop()
{
    state = RTE::Audio::State::Stopped;
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

void AudioComponent::SetLoop(bool loopBool)
{
    _emitter->SetLoop(loopBool);
}

}; // namespace RTE::StdComponents