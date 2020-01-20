#include "include/JSON/JSONValue.hpp"
#include "include/JSON/JSONArray.hpp"
#include "include/JSON/JSONBoolean.hpp"
#include "include/JSON/JSONNull.hpp"
#include "include/JSON/JSONNumber.hpp"
#include "include/JSON/JSONObject.hpp"
#include "include/JSON/JSONString.hpp"

namespace RTE::AssetManagement
{

JSONValue::JSONValue(JSONType type) : Type(type)
{

}

JSONNumber& JSONValue::AsNumber()
{
    throw "Not a JSON Number!";
}

JSONString& JSONValue::AsString()
{
    throw "Not a JSON String!";
}

JSONBoolean& JSONValue::AsBool()
{
    throw "Not a JSON Boolean!";
}

JSONArray& JSONValue::AsArray()
{
    throw "Not a JSON Array!";
}

JSONObject& JSONValue::AsObject()
{
    throw "Not a JSON Object!";
}

JSONNull& JSONValue::AsNull()
{
    throw "Not a JSON Null!";
}



}