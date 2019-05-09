#include "BallController.hpp"

void BallController::Initialize(RTE::StdComponents::PhysicsComponent *phys,
                                RTE::StdComponents::TransformComponent *trans,
                                GameObjectId leftCollider,
                                GameObjectId rightCollider)
{
    _phys = phys;
    _trans = trans;
    _leftCollider = leftCollider;
    _rightCollider = rightCollider;
}

void BallController::Update(float deltaTime)
{
    while (!_phys->Collisions->empty())
    {
        RTE::Physics::OnCollisionData col = _phys->Collisions->front();
        if (col.GoId == _leftCollider)
        {
            Debug("LEFT");
        }
        if(col.GoId = _rightCollider)
        {
            Debug("RIGHT");
        }
        else
        {
        }
        _phys->Collisions->pop();
    }
    while(!_phys->EndCollisions->empty())
    {
        _phys->EndCollisions->pop();
    }

    _phys->GetRigidBody()->SetLinearVelocity(_dir * _speed);
}