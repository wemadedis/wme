add_executable(InputTest Main.cpp PlayerController.cpp)
target_include_directories(InputTest PUBLIC .)
target_link_libraries(InputTest rte)