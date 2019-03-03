#include "PlayerController.h"
#include "rte/InputManager.h"
#include <iostream>

static int _round = 0;

void MovePlayerCommand::Execute()
{
    std::cout << "Frame: " << _round << std::endl;
}

// This is called ONCE
// [Player, Transform, Mesh, Input]
void PlayerController::Init()
{
    _movement = new MovePlayerCommand();
}

// This is called every frame
void PlayerController::OnUpdate(int thisRound)
{
    _round = thisRound;
    _movement->Execute();
}
