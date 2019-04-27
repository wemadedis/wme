#pragma once

#include "rte/Component.h"
#include "rte/TransformComponent.hpp"
#include "rte/Listener.hpp"

namespace RTE::StdComponents
{
class ListenerComponent : public Runtime::Component
{
    TransformComponent *_transformComponent;
    bool _enabled;
public:
    ListenerComponent();
    ~ListenerComponent();

    void Initialize(TransformComponent* transformComponent, bool enabled=true);

    virtual void Update(float deltaTime) override;

    void SetGlobalVolume(float volume);

    void Enable();
    void Disable();
    bool GetState();
};
}
