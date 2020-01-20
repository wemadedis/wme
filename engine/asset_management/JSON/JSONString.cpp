#pragma once

#include "JSON/JSONString.hpp"

namespace RTE::AssetManagement
{

JSONString::JSONString(std::string value) : JSONValue(JSONType::STRING)
{
    _value = value;
}

JSONString& JSONString::AsString()
{
    return *this;  
}

std::string JSONString::GetValue()
{
    return _value;
}

}