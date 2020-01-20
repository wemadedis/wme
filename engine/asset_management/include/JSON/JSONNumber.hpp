#pragma once

#include "JSONValue.hpp"

namespace RTE::AssetManagement
{

class JSONNumber : public JSONValue
{
private:
    double _value;
public:
    JSONNumber(double value);

    JSONNumber& AsNumber();

    double GetValue();
};

}