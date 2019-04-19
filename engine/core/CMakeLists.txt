add_library(core
    RTECore
)
target_include_directories(core PUBLIC include)
target_link_libraries(core rte)
