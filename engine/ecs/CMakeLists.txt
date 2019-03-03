add_library(ecs ECS.cpp)
target_include_directories(ecs PUBLIC include)
target_link_libraries(ecs PUBLIC rte)