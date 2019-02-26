add_executable(triangle main.cpp DeviceMemoryManager.cpp Utilities.cpp)
target_link_libraries(triangle 
    PUBLIC rte
)