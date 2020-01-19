#pragma once

#include "Asset.hpp"
#include "FileTree.hpp"

#include<string>
#include<vector>

namespace RTE::AssetManagement
{


class AssetManager
{
private:

    std::vector<std::string> _validExtensions;

    std::vector<Asset> _assets;

    FileTree* _fileTree;

    std::string ReadFile(std::string path);

    void ReadProjectConfig();

    void LoadAsset();

    void LoadAllAssets();

public:
    AssetManager();

    void ScanAssets();

    
};

}