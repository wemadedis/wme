#pragma once

#include "JSON/JSONNumber.hpp"

namespace RTE::AssetManagement
{

JSONNumber::JSONNumber(double value) : JSONValue(JSONType::NUMBER)
{
    _value = value;
}

JSONNumber& JSONNumber::AsNumber()
{
    return *this;
}

double JSONNumber::GetValue()
{
    return _value;
}

}