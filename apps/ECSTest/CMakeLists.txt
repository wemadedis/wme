add_executable(ECSTest RTEMain.cpp UserMain.cpp FloatComponent.cpp MultiplyByTwoSystem.cpp)
target_include_directories(ECSTest PUBLIC .)
target_link_libraries(ECSTest rte)