add_library(platform WindowManager.cpp)
target_include_directories(platform PUBLIC include)
target_link_libraries(platform rte)