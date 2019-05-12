#pragma once

#include <unordered_set>

#include <rte/RTE.hpp>

class GameController;

class BallController : public RTE::Runtime::Component
{
private:
    float _speed = 5;
    glm::vec3 _noMove;
    glm::vec3 _startingDirections[4];
    int _directionIndex = 0;

    glm::vec3 _dir = _noMove;
    GameObjectId _leftCollider;
    GameObjectId _rightCollider;
    RTE::StdComponents::PhysicsComponent *_phys;
    RTE::StdComponents::TransformComponent *_trans;
    RTE::StdComponents::AudioComponent *_ac;
    GameController *_gameController;
    std::unordered_set<CollisionId> _collisionIds;
    
public:
    void Initialize(RTE::StdComponents::PhysicsComponent *phys,
                    RTE::StdComponents::TransformComponent *trans,
                    RTE::StdComponents::AudioComponent *ac,
                    GameController *gameController,
                    GameObjectId leftCollider,
                    GameObjectId rightCollider);
    void Update(float deltaTime) override;
    void SetBallMoveState(bool move);
};