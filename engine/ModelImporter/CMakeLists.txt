add_library(ModelImporter ModelImporter.cpp)
target_include_directories(ModelImporter PUBLIC include .)
target_link_libraries(ModelImporter PUBLIC rte)
