add_library(importer 
    Importer
    AssimpMeshImporter
    STBITextureImporter
)

target_include_directories(importer PUBLIC include.)
target_link_libraries(importer PUBLIC rte)
