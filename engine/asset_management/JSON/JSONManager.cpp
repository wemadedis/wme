#pragma once

#include "JSONObject.hpp"

#include<string>

namespace RTE::AssetManagement
{

class JSONManager
{
private:

public:
    JSONObject ParseJson();

    std::string ConvertToString(JSONObject& object);

    void WriteToFile(JSONObject& object);
};


}
