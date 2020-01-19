#pragma once

#include<vector>
#include<string>
#include<filesystem>


namespace fs = std::filesystem;

namespace RTE::AssetManagement
{
// TODO: Make tree iterator?

class FileTree
{
private:
    // Node representing a directory. Contains a list of all files and subdirectories.
    struct Node
    {
        std::string DirectoryName;
        std::vector<Node> Children;
        std::vector<std::string> Files;
    };

    Node root;

    Node ConstructNode(const fs::directory_entry &path);

    Node ConstructTree(std::string path);

    void GetNodeFilesRecursive(Node& node, std::string currentPath, std::vector<std::string>& outFiles);

public:
    FileTree(std::string path);

    std::vector<std::string> GetAllFiles();

};

}