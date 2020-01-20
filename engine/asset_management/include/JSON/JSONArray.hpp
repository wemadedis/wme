#pragma once

#include "JSONValue.hpp"

#include <vector>
#include <stdint.h>

namespace RTE::AssetManagement
{

class JSONArray : public JSONValue
{
private:
    std::vector<JSONValue*> _values;
public:

    JSONArray(std::string json);

    JSONArray& AsArray();

    uint64_t Size();

    JSONValue& operator[](uint64_t index);
};

}