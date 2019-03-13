#pragma once

#include <iostream>

#include "rte/Component.h"

class FloatComponent : public Component
{
  public:
    float value = 0;
    void Initialize(float startValue);
    void Update();
};