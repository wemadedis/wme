add_library(importer 
    Importer
    AssimpMeshImporter
    STBITextureImporter
    MeshImporter
)

target_include_directories(importer PUBLIC include.)
target_link_libraries(importer PUBLIC rte)
