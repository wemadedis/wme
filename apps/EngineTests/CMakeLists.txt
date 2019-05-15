add_executable(EngineTests main.cpp PlayerController.hpp ObjectStructs.hpp Rotator.hpp)

target_link_libraries(EngineTests
    PUBLIC rte
)