cmake_minimum_required(VERSION 3.7.0)


set( CMAKE_VERBOSE_MAKEFILE off )
set( CMAKE_CXX_STANDARD 17)
set( BUILD_SHARED_LIBS OFF)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")


set( ENGINE_ASSETS "${PROJECT_SOURCE_DIR}/engine/assets" )

add_library(rte INTERFACE)

target_compile_definitions(rte INTERFACE ENGINE_ASSET_DIR="${ENGINE_ASSETS}/")

add_subdirectory(engine)
add_subdirectory(apps)


target_link_libraries(rte INTERFACE
    core
    physics
    renderer
    StandardComponents
    runtime_architecture 
    model_importer 
    platform 
    Audio
    utilities
    StandardComponents
    glm 
    vma 
    vulkan 
    assimp 
    glfw 
    bullet
    imgui
    sfml-audio
    
)
