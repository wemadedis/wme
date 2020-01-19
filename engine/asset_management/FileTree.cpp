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
            n.Files.push_back(entry.path().string());
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

FileTree::FileTree(string path)
{
    root = ConstructTree(path);
    int q = 0;
}



}