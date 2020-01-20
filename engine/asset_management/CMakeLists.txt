add_library(assetmanagement 
    AssetManager.cpp
    FileTree.cpp
    JSON/JSONArray.cpp
    JSON/JSONString.cpp
    JSON/JSONNumber.cpp
    JSON/JSONBoolean.cpp
    JSON/JSONObject.cpp
    JSON/JSONNull.cpp)
target_include_directories(assetmanagement PUBLIC include)
target_link_libraries(assetmanagement rte)