cmake_minimum_required(VERSION 3.7.0)


set( CMAKE_VERBOSE_MAKEFILE off )
set( CMAKE_CXX_STANDARD 17)
set( ENGINE_ASSETS "${PROJECT_SOURCE_DIR}/engine/assets" )
set( BUILD_SHARED_LIBS OFF)

add_library(rte INTERFACE)

target_compile_definitions(rte INTERFACE ENGINE_ASSET_DIR="${ENGINE_ASSETS}/")


add_subdirectory(engine)
add_subdirectory(apps)


target_link_libraries(rte INTERFACE
    core
    physics
    renderer 
    scene_manager
    runtime_architecture 
    model_importer 
    platform 
    AudioEmitter
    glm 
    vma 
    vulkan 
    assimp 
    glfw 
    bullet
    imgui
    sfml-audio
    
)
