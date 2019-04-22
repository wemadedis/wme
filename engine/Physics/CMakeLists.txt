add_library(physics
    PhysicsManager
    RigidBody
)

target_include_directories(physics PUBLIC include)
target_link_libraries(physics PUBLIC rte)
target_compile_definitions(physics PUBLIC BT_USE_DOUBLE_PRECISION=1)