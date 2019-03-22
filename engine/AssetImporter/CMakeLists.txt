add_library(AssetImporter AssetImporter.cpp)
target_include_directories(AssetImporter PUBLIC include .)
target_link_libraries(AssetImporter PUBLIC rte)
