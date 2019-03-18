add_library(renderer 
    Renderer.cpp 
    DeviceMemoryManager.cpp
    Utilities.cpp 
    GraphicsPipeline.cpp 
    RenderPass.cpp 
    Instance.cpp 
    SwapChain.cpp 
    CommandBufferManager.cpp 
    ImageManager.cpp 
    DescriptorManager.cpp
    Primitives.cpp
)
target_include_directories(renderer PUBLIC include .)
target_link_libraries(renderer PUBLIC rte)