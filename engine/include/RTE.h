#pragma once

#include "Defs.h"
#include <vector>
#ifndef ENGINE_ASSET_DIR
#define ENGINE_ASSET_DIR "THIS_VARIABLE_IS_SET_BY_CMAKE"
#endif


struct Scene
{

};

std::vector<Scene> Initialize();
