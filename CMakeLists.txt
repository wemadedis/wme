cmake_minimum_required(VERSION 3.4.0)

set( CMAKE_VERBOSE_MAKEFILE off )
set( CMAKE_CXX_STANDARD 17)
set( ENGINE_ASSETS "${PROJECT_SOURCE_DIR}/engine/assets" )

add_library(rte INTERFACE)
target_compile_definitions(rte INTERFACE ENGINE_ASSET_DIR="${ENGINE_ASSETS}/")
target_include_directories(rte INTERFACE engine/include)
target_link_libraries(rte INTERFACE
    glm vma vulkan assimp renderer platform glfw ecs
)

add_subdirectory(engine)
add_subdirectory(apps)
