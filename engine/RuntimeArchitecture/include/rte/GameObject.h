#pragma once

#include <vector>

#include "rte/Component.h"
#include "Defs.h"

class GameObject
{
  private:
    u64 Id;

  public:
    GameObject(u64 id);
    u64 GetId();
    void SetId(u64 id);
};
