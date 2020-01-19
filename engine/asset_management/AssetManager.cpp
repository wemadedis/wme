#include "AssetManager.hpp"

#include <rapidjson/rapidjson.h>


#include <fstream>
#include <iostream>

using namespace std;

namespace RTE::AssetManagement
{


std::string AssetManager::ReadFile(std::string path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer.data();
}

void AssetManager::ReadProjectConfig()
{

}

void AssetManager::ReadAssets()
{

}


AssetManager::AssetManager()
{
    string assetsPath = "C:\\Users\\Svampex\\Documents\\Projects\\wme\\engine\\assets";
    _fileTree = new FileTree(assetsPath);

}

void AssetManager::ScanAssets()
{
    
    
}

}