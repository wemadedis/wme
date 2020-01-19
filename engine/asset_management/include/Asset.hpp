#pragma once

#include <string>
#include <stdint.h>


namespace RTE::AssetManagement
{

enum class AssetType
{
    UNKNOWN,
    MODEL,
    SHADER,
    SOUND,

};

struct Asset
{
    AssetType Type = AssetType::UNKNOWN;
    std::string Path = "";
    uint32_t ID = 0;
};

}

