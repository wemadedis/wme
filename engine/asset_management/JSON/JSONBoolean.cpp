#pragma once

#include "JSON/JSONBoolean.hpp"

namespace RTE::AssetManagement
{

JSONBoolean::JSONBoolean(bool value)  : JSONValue(JSONType::BOOLEAN)
{
    _value = value;
}

JSONBoolean& JSONBoolean::AsBoolean()
{
    return *this;
}

bool JSONBoolean::GetValue()
{
    return _value;
}

}