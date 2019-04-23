#pragma once

#include "rte/AudioComponent.hpp"
#include "rte/AudioListener.hpp"
#include "rte/Component.h"

namespace RTE::StandardComponents
{

class AudioComponent : public Runtime::Component
{
public:
    AudioComponent();
    virtual void Update(float deltaTime) override;
};

// End RTE::StandardComponents namespace
}; // namespace RTE::StandardComponents