#pragma once

#include "rte/ECSCore.h"

class FloatComponent : public ECSComponent
{
  public:
    float value = 0;
};