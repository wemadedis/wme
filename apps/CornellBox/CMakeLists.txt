add_executable(CornellBox main.cpp PlayerController.hpp ObjectStructs.hpp Rotator.hpp)

target_link_libraries(CornellBox
    PUBLIC rte
)