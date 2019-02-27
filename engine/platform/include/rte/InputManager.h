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

struct KeyPress
{
    KeyboardKey Key;
    PressType Type;
};

struct InputBinding
{
    InputType InputType;
    union {
        KeyPress Key;
        KeyPress Keys[4];
    };
};

union InputData
{
    bool Triggered;
    struct
    {
        float X;
        float Y;
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