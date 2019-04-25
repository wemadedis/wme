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
        _transform->Transform.Rot.x = glm::radians(-90.0f);
    }

    void Update(float deltaTime)
    {

        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_W) == GLFW_PRESS)
        {
            _transform->Transform.Pos.z -= 5.0f*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_A) == GLFW_PRESS)
        {
            _transform->Transform.Pos.x -= 5.0f*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_S) == GLFW_PRESS)
        {
            _transform->Transform.Pos.z += 5.0f*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_D) == GLFW_PRESS)
        {
            _transform->Transform.Pos.x += 5.0f*deltaTime;
        }
    }
};