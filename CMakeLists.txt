cmake_minimum_required(VERSION 3.4.0)

set( CMAKE_VERBOSE_MAKEFILE off )
set (CMAKE_CXX_STANDARD 17)
set( ENGINE_INCLUDE "${PROJECT_SOURCE_DIR}/engine/include" )
set( ENGINE_ASSETS "${PROJECT_SOURCE_DIR}/engine/assets" )

macro (PRINT_TARGETS)
    get_property(targets
                 DIRECTORY "."
        PROPERTY BUILDSYSTEM_TARGETS)
    message("Targets: " ${targets})
endmacro (PRINT_TARGETS)

###<RTE>
add_library(rte INTERFACE)
target_include_directories(rte INTERFACE engine/include)
###</RTE>
###<GLM> 
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE engine/third_party/glm)
###</GLM>

###<VULKAN>
find_package(Vulkan REQUIRED)
execute_process( COMMAND "glslangvalidator -V ${ENGINE_ASSETS}/shaders/shader.frag" )
execute_process( COMMAND "glslangvalidator -V ${ENGINE_ASSETS}/shaders/shader.vert" )
###</VULKAN>

###<ASSIMP>
add_subdirectory(engine/third_party/assimp)
###</ASSIMP>

###<RENDERER>
add_library(renderer engine/renderer/RenderManager.cpp)
target_link_libraries(renderer glm)
target_include_directories(renderer PUBLIC engine/renderer/include)
target_include_directories(renderer PRIVATE ${ENGINE_INCLUDE})
###</RENDERER>

###<GLFW>
# Don't build GLFW documentation, tests, and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(engine/third_party/glfw)
###</GLFW>
add_subdirectory(engine)
add_subdirectory(apps)