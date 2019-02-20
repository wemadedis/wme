add_executable(triangle main.cpp)
target_compile_definitions(triangle PUBLIC ENGINE_ASSET_DIR="${ENGINE_ASSETS}/")
target_link_libraries(triangle 
    PUBLIC glm glfw Vulkan::Vulkan renderer rte
)