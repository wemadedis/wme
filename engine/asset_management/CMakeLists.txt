add_library(assetmanagement 
    AssetManager.cpp
    FileTree.cpp)
target_include_directories(assetmanagement PUBLIC include)
target_link_libraries(assetmanagement rte)