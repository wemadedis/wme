add_library(model_importer ModelImporter.cpp)
target_include_directories(model_importer PUBLIC include .)
target_link_libraries(model_importer PUBLIC rte)
