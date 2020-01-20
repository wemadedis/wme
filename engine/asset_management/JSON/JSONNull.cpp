#pragma once

#include "JSON/JSONNull.hpp"

namespace RTE::AssetManagement
{

JSONNull::JSONNull()  : JSONValue(JSONType::NULLVALUE)
{

}

JSONNull& JSONNull::AsNull()
{
    return *this;
}

}