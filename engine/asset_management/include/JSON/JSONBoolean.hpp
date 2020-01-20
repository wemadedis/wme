#pragma once

#include "JSONValue.hpp"

namespace RTE::AssetManagement
{

class JSONBoolean : public JSONValue
{
private:
    bool _value;
public:

    JSONBoolean(bool value);

    JSONBoolean& AsBoolean();

    bool GetValue();

};

}