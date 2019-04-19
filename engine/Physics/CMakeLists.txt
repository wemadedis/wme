add_library(physics
    PhysicsManager
)

target_include_directories(physics PUBLIC include)
target_link_libraries(physics PUBLIC rte)