add_library(Audio Emitter.cpp Listener.cpp AudioManager.cpp)
target_include_directories(Audio PUBLIC include .)
target_link_libraries(Audio PUBLIC rte)
