#pragma once

#include "JSONValue.hpp"

namespace RTE::AssetManagement
{

class JSONNull : public JSONValue
{

public:

    JSONNull();

    JSONNull& AsNull();

};

}