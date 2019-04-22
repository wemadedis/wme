add_library(utilities 
    Utility
)
target_include_directories(utilities PUBLIC include)
target_link_libraries(utilities PUBLIC rte)