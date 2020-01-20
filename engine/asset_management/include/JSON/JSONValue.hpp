#pragma once

#include <string>
#include "rapidjson/document.h"

namespace RTE::AssetManagement
{

class JSONNumber;
class JSONString;
class JSONBool;
class JSONArray;
class JSONObject;
class JSONNull;

enum class JSONType { NUMBER, STRING, BOOLEAN, ARRAY, OBJECT, NULLVALUE };

class JSONValue
{

public:

    const JSONType Type;

    JSONValue(JSONType type);

    virtual JSONNumber& AsNumber();
    virtual JSONString& AsString();
    virtual JSONBool& AsBool();
    virtual JSONArray& AsArray();
    virtual JSONObject& AsObject();
    virtual JSONNull& AsNull();
    virtual std::string ToString();
};

}