#include "GameController.hpp"

using namespace RTE;
using namespace RTE::StdComponents;

void GameController::Initialize(RTE::StdComponents::TransformComponent *trans,
                                float delay)
{
    _trans = trans;
    _delay = delay;
}

void GameController::Update(float deltaTime)
{
    if (_roundOver)
    {
        _counter += deltaTime;
        if (_delay < _counter)
        {
            _counter = 0;
            _roundOver = false;
        }
    }
    else
    {
        //_trans->Transform.Pos = _ballStartPos;
    }
}