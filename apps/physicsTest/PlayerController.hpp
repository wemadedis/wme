#pragma once

#include <rte/RTE.hpp>

class PlayerController : public RTE::Runtime::Component
{

private:
    RTE::StdComponents::TransformComponent *_transform;
    RTE::StdComponents::PhysicsComponent *_physics;

public:
    void Initialize(
        RTE::StdComponents::TransformComponent *trans,
        RTE::StdComponents::PhysicsComponent *phys)
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