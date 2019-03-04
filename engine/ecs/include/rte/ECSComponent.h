#pragma once

#include <cstdint>

class ECSComponent
{
  public:
    ECSComponent(uint64_t mask);
    uint32_t Mask;
};