add_executable(platformTest Main.cpp DeviceMemoryManager.cpp)
target_compile_definitions(platformTest PUBLIC ENGINE_ASSET_DIR="${ENGINE_ASSETS}/")
target_link_libraries(platformTest 
    PUBLIC glm glfw Vulkan::Vulkan rte vma platform
)