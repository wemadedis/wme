#include "rte/InputManager.h"
#include <iostream>

namespace RTE::InputManager
{
void PlayerController::MovePlayerCB(InputData data)
{
}

void PlayerController::JumpCB(InputData data)
{
    
}

void PlayerController::Caller()
{
    // I want to move the player,
    // I want a single callback any trigger to the 
    auto inputAction = MakeInputAction()
        ->AddBinding({InputType::KEYBOARD, KeyboardKey::W, PressType::PRESS})
        ->AddBinding({InputType::KEYBOARD, KeyboardKey::A, PressType::PRESS})
        ->AddBinding({InputType::KEYBOARD, KeyboardKey::S, PressType::PRESS})
        ->AddBinding({InputType::KEYBOARD, KeyboardKey::D, PressType::PRESS})
        ->AddCallback(&PlayerController::MovePlayerCB);

    auto jumpAction = MakeInputAction()
        ->AddBinding({InputType::KEYBOARD, KeyboardKey::SPACE, PressType::PRESS})
        ->AddCallback(&PlayerController::JumpCB);
}

InputAction *InputAction::AddBinding(InputBinding binding)
{
    return new InputAction();
}


InputAction *MakeInputAction()
{
    return new InputAction();
}

} // namespace RTE::InputManager