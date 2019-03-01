#pragma once
#include <iostream>
#include <vector>
#include "rte/InputCommand.h"

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
        struct
        {
            KeyboardKey Left;
            KeyboardKey Up;
            KeyboardKey Right;
            KeyboardKey Down;
        } Directional;
    };
};

union InputData {
    bool Triggered;
    struct
    {
        float X;
        float Y;
    };
};

}; // namespace RTE::InputManager