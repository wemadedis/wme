add_executable(platformTest Main.cpp DeviceMemoryManager.cpp)
target_link_libraries(platformTest 
    PUBLIC rte
)