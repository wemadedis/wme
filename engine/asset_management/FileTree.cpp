#include "include/FileTree.hpp"

#include <iostream>

using namespace std;

namespace RTE::AssetManagement
{

FileTree::Node FileTree::ConstructNode(const fs::directory_entry &path)
{
    Node n = Node();
    n.DirectoryName = path.path().filename().string();
    for (const auto & entry : fs::directory_iterator(path))
    {
        if(fs::is_directory(entry))
        {
            n.Children.push_back(ConstructNode(entry));
        }
        else if(fs::is_regular_file(entry))
        {
            n.Files.push_back(entry.path().filename().string());
        }
    } 
    return n;
}

FileTree::Node FileTree::ConstructTree(string path)
{
    Node root = Node();
    root.DirectoryName = path;
    for (const auto & entry : fs::directory_iterator(path))
    {
        if(fs::is_directory(entry))
        {
            root.Children.push_back(ConstructNode(entry));
        }
        else if(fs::is_regular_file(entry))
        {
            root.Files.push_back(entry.path().string());
        }
    }
    return root;
}

void FileTree::GetNodeFilesRecursive(Node& node, std::string currentPath, std::vector<std::string>& outFiles)
{
    currentPath += node.DirectoryName;
    if(currentPath.back() != '/')
    {
        currentPath += + "/";
    }

    for(string fileName : node.Files)
    {
        outFiles.push_back(currentPath+fileName);
    }

    for(Node& child : node.Children)
    {
        GetNodeFilesRecursive(child, currentPath, outFiles);
    }
}

FileTree::FileTree(string path)
{
    root = ConstructTree(path);
}

std::vector<std::string> FileTree::GetAllFiles()
{
    vector<string> files = {};
    GetNodeFilesRecursive(root, "", files);
    return files;
}

}