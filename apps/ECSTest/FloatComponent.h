#pragma once

#include "rte/ECS.h"

class FloatComponent : public ECSComponent
{
    int GetMask();
    void Trigger();
}