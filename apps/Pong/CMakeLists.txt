add_executable(Pong 
    Pong
    PaddleController
    BallController
)
target_include_directories(Pong PUBLIC .)
# Link with RTE
target_link_libraries(Pong
    PUBLIC rte
)