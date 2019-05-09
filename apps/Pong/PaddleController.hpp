#pragma once

#include <rte/RTE.hpp>

enum class Direction 
{
    NONE,
    UP,
    DOWN
};

class PaddleController : public RTE::Runtime::Component
{
private:
    float _speed = 5;
    bool _isLeft = true;
    RTE::StdComponents::PhysicsComponent *_phys;
    RTE::StdComponents::TransformComponent *_trans;

public:
    void Initialize(RTE::StdComponents::PhysicsComponent *phys,
                    RTE::StdComponents::TransformComponent *trans,
                    bool isLeft);
    void Update(float deltaTime) override;
};