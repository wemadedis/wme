#pragma once

#include <rte/RTE.hpp>

class GameController : public RTE::Runtime::Component
{
public:
    void Initialize(RTE::StdComponents::TransformComponent *ballTrans,
                    float delayBetweenRounds);
    void Update(float deltaTime) override;

private:
    RTE::StdComponents::TransformComponent *_trans = nullptr;
    glm::vec3 _ballStartPos = {0, 0, 0};
    float _delay = 0;
    float _counter = 0;
    float _roundOver = true;
};