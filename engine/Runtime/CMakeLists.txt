add_library(runtime
    Scene 
    GameObject
    Component
    SceneManager
)
target_include_directories(runtime PUBLIC include)
target_link_libraries(runtime rte)