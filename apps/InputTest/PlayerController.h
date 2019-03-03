#pragma once
#include "rte/InputCommand.h"
class MovePlayerCommand : public RTE::InputManager::InputCommand
{
  void Execute();
};

class PlayerController
{
public:
  void Init();
  void OnUpdate(int thisRound);

private:
  RTE::InputManager::InputCommand *_movement;
};
