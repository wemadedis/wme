add_library(AudioEmitter AudioEmitter.cpp)
target_include_directories(AudioEmitter PUBLIC include .)
target_link_libraries(AudioEmitter PUBLIC rte)
