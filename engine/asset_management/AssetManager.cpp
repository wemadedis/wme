#include "AssetManager.hpp"

#include <rapidjson/rapidjson.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

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

void AssetManager::GenerateProjectAssetDescriptor()
{
    using namespace rapidjson;
    Document d;
    d.SetObject();
    auto& allocator = d.GetAllocator();

    Value assets(kArrayType);
    // assets.PushBack()
    d.AddMember("Assets", 1, allocator);
    int q = 0;
    
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
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
    GenerateProjectAssetDescriptor();
}

}