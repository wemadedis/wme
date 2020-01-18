add_library(core
    RTECore
    RTEConfig
)
target_include_directories(core PUBLIC include)
target_link_libraries(core rte)
