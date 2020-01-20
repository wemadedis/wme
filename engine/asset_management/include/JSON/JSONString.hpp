#pragma once

#include "JSONValue.hpp"

#include <string>

namespace RTE::AssetManagement
{

class JSONString : public JSONValue
{
private:
    std::string _value;
public:

    JSONString(std::string value);

    JSONString& AsString();

    std::string GetValue();

};

}