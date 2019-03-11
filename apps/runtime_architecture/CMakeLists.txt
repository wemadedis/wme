add_executable(runtime_test Main.cpp)
target_include_directories(runtime_test PUBLIC .)
target_link_libraries(runtime_test PUBLIC rte)