#pragma once
#include <iostream>

namespace RTE::InputManager
{

enum class InputType
{
    KEYBOARD
};

enum class PressType
{
    PRESS,
    HOLD,
    RELEASE
};

enum class KeyboardKey
{
    W,
    A,
    D,
    S,
    SPACE
};

struct InputBinding
{
    InputType InputType;
    union {
        struct
        {
            KeyboardKey KeyboardKey;
            PressType PressType;
        };
    };
};

union InputData
{
    bool Triggered;
    struct
    {
        float x;
        float y;
    };
};

enum class Action
{
    MOVE
};

class PlayerController
{
  public:
    void MovePlayerCB(InputData data);
    void Caller();
    void JumpCB(InputData data);
};


class InputAction
{

  public:
    InputAction *AddBinding(InputBinding binding);
    // TODO: (danh 27/02 16:01): CONSIDER THIS TEMPLATE FUNCTION
    template<typename T>
    InputAction *AddCallback(void (T::*)(InputData))
    {
        std::cout << "ADDED A CALLBACK" << std::endl; 
        return this;
    }
};
InputAction *MakeInputAction();
} // namespace RTE::InputManager