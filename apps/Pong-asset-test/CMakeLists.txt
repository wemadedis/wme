add_executable(Pong-asset-test 
    Pong
    PaddleController
    BallController
    GameController
)
target_include_directories(Pong-asset-test PUBLIC .)
# Link with RTE
target_link_libraries(Pong-asset-test
    PUBLIC rte
)