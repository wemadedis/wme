#pragma once

#include "rte/Component.h"
#include <iostream>

class FloatComponent : public Component
{

  public:
    float value = 0;
    void Update()
    {
        std::cout << (value++) << std::endl;
    }
};