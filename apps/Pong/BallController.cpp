#include "BallController.hpp"

#include "GameController.hpp"

void BallController::Initialize(RTE::StdComponents::PhysicsComponent *phys,
                                RTE::StdComponents::TransformComponent *trans,
                                GameController *gameController,
                                GameObjectId leftCollider,
                                GameObjectId rightCollider)
{
    _phys = phys;
    _trans = trans;
    _gameController = gameController;
    _leftCollider = leftCollider;
    _rightCollider = rightCollider;
    _startingDirections[0] = glm::normalize(glm::vec3(1, 1, 0));
    _startingDirections[1] = glm::normalize(glm::vec3(1, -1, 0));
    _startingDirections[2] = glm::normalize(glm::vec3(-1, -1, 0));
    _startingDirections[3] = glm::normalize(glm::vec3(-1, 1, 0));
    _noMove = glm::vec3(0, 0, 0);
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
                Debug("Hit left");
                _gameController->AddScore(false);
            }

            else if (col.GoId == _rightCollider)
            {
                Debug("Hit right");
                _gameController->AddScore(true);
            }
        }
    }
    while (!_phys->EndCollisions->empty())
    {
        _phys->EndCollisions->pop();
    }

    _phys->GetRigidBody()->SetLinearVelocity(_dir * _speed);
}

void BallController::SetBallMoveState(bool shouldMove)
{
    if(shouldMove)
    {
        _dir = _startingDirections[_directionIndex++];
    }
    else
    {
        _dir = _noMove;
    }
}