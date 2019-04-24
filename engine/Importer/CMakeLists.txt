add_library(importer ModelImporter.cpp ImportFunctions.cpp)
target_include_directories(importer PUBLIC include .)
target_link_libraries(importer PUBLIC rte)
