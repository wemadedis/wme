#pragma once

#include "rte/Component.h"
#include "rte/PhysicsManager.hpp"

namespace RTE::StandardComponents
{
class PhysicsComponent : public Runtime::Component
{
private:
public:
    void Update(float deltaTime) override;
    friend class PhysicsManager;
};
}; // namespace RTE::StandardComponents
