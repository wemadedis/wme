#pragma once

#include "rte/Component.h"
#include "rte/AudioListener.hpp"
#include "rte/AudioComponent.hpp"

namespace RTE::StandardComponents
{

class AudioComponent : public Component
{
public:
    AudioComponent();
    virtual void Update() override;

};

// End RTE::StandardComponents namespace
};