#pragma once

#include <rte/RTE.hpp>

class PlayerController : public RTE::Runtime::Component
{

private:
    RTE::StandardComponents::TransformComponent *_transform;
    RTE::StandardComponents::PhysicsComponent *_physics;

public:
    void Initialize(
        RTE::StandardComponents::TransformComponent *trans,
        RTE::StandardComponents::PhysicsComponent *phys)
    {
        _transform = trans;
        _physics = phys;
    }

    void Update(float deltaTime)
    {

        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_A) == GLFW_PRESS)
        {
            Debug("Called");
        }
    }
};