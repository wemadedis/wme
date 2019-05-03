add_executable(ReflectionTest main.cpp PlayerController.hpp ObjectStructs.hpp)

target_link_libraries(ReflectionTest
    PUBLIC rte
)