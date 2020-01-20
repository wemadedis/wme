#pragma once

#include "JSON/JSONObject.hpp"

namespace RTE::AssetManagement
{

JSONObject::JSONObject(std::string json) : JSONValue(JSONType::OBJECT)
{

}

JSONObject& JSONObject::AsObject()
{
    return *this;
}

JSONValue& JSONObject::operator[](std::string name)
{
    return *_values[name];
}

}