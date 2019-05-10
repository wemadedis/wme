#include "GameController.hpp"
#include <iostream>
#include <string>

using namespace RTE;
using namespace RTE::StdComponents;

void GameController::Initialize(BallController *controller,
                                RTE::StdComponents::TransformComponent *trans,
                                float delay)
{
    _controller = controller;
    _trans = trans;
    _delay = delay;
}

void GameController::ShowScore()
{
    ImVec2 pos(500, 50);
    auto cond = ImGuiCond_Always;
    ImVec2 pivot = {0.5, 0.5};
    ImGui::SetNextWindowPos(pos, cond, pivot);
    ImVec2 size = {100, 50};
    ImGui::SetNextWindowSize(size, cond);
    auto flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;
    bool *open = nullptr;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin(" ", open, flags);
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%i - %i", _leftScore, _rightScore);
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void GameController::Update(float deltaTime)
{
    if (_roundOver)
    {
        _controller->SetBallMoveState(false);
        _counter += deltaTime;
        if (_delay < _counter)
        {
            _controller->SetBallMoveState(true);
            _counter = 0;
            _roundOver = false;
        }
    }
}

void GameController::AddScore(bool left)
{
    if (left)
        _leftScore++;
    else
        _rightScore++;

    ResetBall();
}

void GameController::ResetBall()
{
    _trans->Transform.Pos = _ballStartPos;
    _roundOver = true;
}