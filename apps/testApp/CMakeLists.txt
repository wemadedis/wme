add_executable(testApp Main.cpp MyClass.cpp)
target_include_directories(testApp 
    PRIVATE ${ENGINE_INCLUDE}
)
target_link_libraries(testApp 
    PUBLIC renderer glm glfw vulkan
)