#pragma once

#include <string>

#ifndef ENGINE_ASSET_DIR
#define ENGINE_ASSET_DIR "THIS_VARIABLE_IS_SET_BY_CMAKE"
#endif

struct Engine
{
    int _a;
    std::string _b;
};