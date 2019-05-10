#pragma once

#include "imgui/imgui.h"
#include <rte/RTE.hpp>

#include "BallController.hpp"

class GameController : public RTE::Runtime::Component
{
public:
    void Initialize(BallController *controller,
                    RTE::StdComponents::TransformComponent *ballTrans,
                    float delayBetweenRounds);
    void Update(float deltaTime) override;
    void ResetBall();
    void ShowScore();
    void AddScore(bool left);

private:
    BallController *_controller;
    int _leftScore = 0;
    int _rightScore = 0;
    RTE::StdComponents::TransformComponent *_trans = nullptr;
    glm::vec3 _ballStartPos = {0, 0, 0};
    float _delay = 0;
    float _counter = 0;
    float _roundOver = true;
};