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
        _phys->Collisions->pop();
        if (col.NewCollision)
        {
            float angle = glm::dot(_dir, col.Normal);
            if (angle < 0)
            {
                _dir = glm::normalize(glm::reflect(_dir, col.Normal));
            }

            if (col.GoId == _leftCollider)
            {
                Debug("LEFT");
            }

            else if (col.GoId == _rightCollider)
            {
                Debug("RIGHT");
            }
        }
    }
    while (!_phys->EndCollisions->empty())
    {
        _phys->EndCollisions->pop();
    }

    _phys->GetRigidBody()->SetLinearVelocity(_dir * _speed);
}