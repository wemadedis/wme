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
        while (_physics->Collisions->empty() == false)
        {
            RTE::Physics::OnCollisionData colData = _physics->Collisions->front();
            Debug(std::to_string(colData.OtherId));
            _physics->Collisions->pop();
        }
        while (_physics->EndCollisions->empty() == false)
        {
            RTE::Physics::EndCollisionData colData = _physics->EndCollisions->front();
            _physics->EndCollisions->pop();
        }

        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_A) == GLFW_PRESS)
        {
            Debug("Called");
        }
    }
};