#include "AssetManager.hpp"

#include <rapidjson/rapidjson.h>


#include <fstream>
#include <iostream>


AssetManager::AssetManager()
{
    using namespace rapidjson;
    ifstream fis(filename);
    IStreamWrapper isw(fis);
    Document d;
    d.ParseStream(isw);
}