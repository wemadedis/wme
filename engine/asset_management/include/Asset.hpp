#pragma once

#include <string>
#include <stdint.h>


namespace RTE::AssetManagement
{

typedef uint32_t AssetID;

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
    AssetID ID = 0;
};

}

