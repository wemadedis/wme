add_library(Audio AudioEmitter.cpp AudioListener.cpp)
target_include_directories(Audio PUBLIC include .)
target_link_libraries(Audio PUBLIC rte)
