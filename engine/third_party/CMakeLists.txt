###<GLM> 
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)
###</GLM>

###<VMA> 
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE vma)
###</VMA>

###<VULKAN>
find_package(Vulkan REQUIRED)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${Vulkan_INCLUDE_DIR})
target_link_libraries(vulkan INTERFACE ${Vulkan_LIBRARY})
###</VULKAN>

###<ASSIMP>
add_library(assimp INTERFACE)
target_link_libraries(assimp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/assimp/lib/assimp.lib)
target_include_directories(assimp INTERFACE include)
###</ASSIMP>

###<GLFW>
# Don't build GLFW documentation, tests, and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
###</GLFW>