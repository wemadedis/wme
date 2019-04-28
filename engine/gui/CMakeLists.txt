add_library(gui 
    GUIModule.cpp
    imgui_impl_vulkan.cpp
)
target_include_directories(gui PUBLIC include .)
target_link_libraries(gui PUBLIC rte)