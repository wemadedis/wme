add_library(ecs ECSCore.cpp ECSSystem.cpp ECSComponent.cpp ECSEntity.cpp)
target_include_directories(ecs PUBLIC include)
target_link_libraries(ecs PUBLIC rte)