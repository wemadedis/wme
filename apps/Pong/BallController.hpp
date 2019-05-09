#pragma once

#include <rte/RTE.hpp>

#include "GameController.hpp"

class BallController : public RTE::Runtime::Component
{
private:
    float _speed = 5;
    glm::vec3 _dir = glm::normalize(glm::vec3(1, 0, 0));
    GameObjectId _leftCollider;
    GameObjectId _rightCollider;
    RTE::StdComponents::PhysicsComponent *_phys;
    RTE::StdComponents::TransformComponent *_trans;
    GameController *gameController;

public:
    void Initialize(RTE::StdComponents::PhysicsComponent *phys,
                    RTE::StdComponents::TransformComponent *trans,
                    GameObjectId leftCollider,
                    GameObjectId rightCollider);
    void Update(float deltaTime) override;
};