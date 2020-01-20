#pragma once

#include "JSONValue.hpp"

#include <string>
#include <unordered_map>

namespace RTE::AssetManagement
{

class JSONObject : public JSONValue
{
private:
    std::unordered_map<std::string,JSONValue*> _values;
public:
    JSONObject(std::string json);

    JSONObject& AsObject();

    JSONValue& operator[](std::string name);

    // May require a Keys() function
};

}