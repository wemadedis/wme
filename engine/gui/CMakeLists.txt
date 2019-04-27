add_library(gui 
    GUIModule.cpp
)
target_include_directories(gui PUBLIC include .)
target_link_libraries(gui PUBLIC rte)