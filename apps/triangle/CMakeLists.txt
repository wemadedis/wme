add_executable(triangle main.cpp DeviceMemoryManager.cpp Utilities.cpp GraphicsPipeline.cpp RenderPass.cpp Instance.cpp SwapChain.cpp CommandBufferManager.cpp ImageManager.cpp DescriptorManager.cpp)
target_link_libraries(triangle 
    PUBLIC rte
)