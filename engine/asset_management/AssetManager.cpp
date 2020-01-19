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

void AssetManager::LoadAllAssets()
{
    vector<string> files = _fileTree->GetAllFiles();
    for(string file : files)
    {
        fs::path p = fs::path(file);
        string extension = p.extension().string();
        // TODO: Create a function that picks an import function based on the extension
    }
}


AssetManager::AssetManager()
{
    string assetsPath = ENGINE_ASSET_DIR;
    _fileTree = new FileTree(assetsPath);
}

void AssetManager::ScanAssets()
{
    LoadAllAssets();
}

}