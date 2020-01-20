#pragma once

#include "JSON/JSONArray.hpp"

namespace RTE::AssetManagement
{

JSONArray::JSONArray(std::string json) : JSONValue(JSONType::ARRAY)
{

}

JSONArray& JSONArray::AsArray()
{
    return *this;
}


uint64_t JSONArray::Size()
{
    return _values.size();
}

JSONValue& JSONArray::operator[](uint64_t index)
{
    return *_values[index];
}

}