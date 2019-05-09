#include "PaddleController.hpp"
#include <rte/RTE.hpp>

using namespace RTE::Platform;

void PaddleController::Initialize(
    RTE::StdComponents::PhysicsComponent *phys,
    RTE::StdComponents::TransformComponent *trans,
    bool isLeft)
{
    this->_phys = phys;
    this->_trans = trans;
    this->_isLeft = isLeft;
}

void PaddleController::Update(float deltaTime)
{
    bool moveUp = false;
    bool moveDown = false;
    if (_isLeft)
    {
        moveUp = WindowManager::GetKey(GLFW_KEY_W);
        moveDown = WindowManager::GetKey(GLFW_KEY_S);
    }
    else
    {
        moveUp = WindowManager::GetKey(GLFW_KEY_UP);
        moveDown = WindowManager::GetKey(GLFW_KEY_DOWN);
    }

    Direction dir = Direction::NONE;
    if (moveUp && !moveDown)
    {
        dir = Direction::UP;
    }
    else if (!moveUp && moveDown)
    {
        dir = Direction::DOWN;
    }

    switch (dir)
    {
    case Direction::UP:
        _phys->GetRigidBody()->SetLinearVelocity({0, _speed, 0});
        break;
    case Direction::DOWN:
        _phys->GetRigidBody()->SetLinearVelocity({0, -_speed, 0});
        break;
    case Direction::NONE:
        _phys->GetRigidBody()->SetLinearVelocity({0, 0, 0});
        break;
    };
}