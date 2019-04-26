#pragma once

#include <rte/RTE.hpp>

class PlayerController : public RTE::Runtime::Component
{

private:
    RTE::StdComponents::TransformComponent *_transform;
    RTE::StdComponents::PhysicsComponent *_physics;
    RTE::StdComponents::CameraComponent *_camera;

public:
    void Initialize(
        RTE::StdComponents::TransformComponent *trans,
        RTE::StdComponents::PhysicsComponent *phys,
        RTE::StdComponents::CameraComponent *cam)
    {
        _transform = trans;
        _physics = phys;
        _camera = cam;
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
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_Q) == GLFW_PRESS)
        {
            _camera->FieldOfView -= 1.0f*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_E) == GLFW_PRESS)
        {
            _camera->FieldOfView += 1.0f*deltaTime;
        }
        std::cout << _camera->FieldOfView << std::endl;
    }
};