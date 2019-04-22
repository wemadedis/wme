add_library(runtime_architecture 
    Scene 
    GameObject
    Component
    SceneManager
    AudioManager
)
target_include_directories(runtime_architecture PUBLIC include)
target_link_libraries(runtime_architecture rte)