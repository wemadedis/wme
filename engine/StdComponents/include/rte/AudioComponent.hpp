#pragma once

#include "rte/TransformComponent.hpp"
#include "rte/Component.h"
#include "rte/Emitter.hpp"

namespace RTE::StdComponents
{

class AudioComponent : public Runtime::Component
{
    Audio::Emitter *_emitter;
    TransformComponent *_transformComponent;
public:
    AudioComponent();
    void Initialize(
        const std::string& filePath,
        TransformComponent* transformComponent,
        float volume=100.f,
        float soundOffset=0.f);
    virtual void Update(float deltaTime) override;
    Audio::Emitter *GetEmitter();
    Audio::State GetState();
    void SetVolume(float volume);
    void Play();
    void Pause();
    void Stop();

};

}