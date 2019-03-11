#pragma once

#include "Defs.h"

typedef u32 ComponentId;

class Component
{
public:
    virtual void Update();
};