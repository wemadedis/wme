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

###<ASSIMP> 💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀💀

set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

add_subdirectory("assimp-4.1.0")
target_include_directories(assimp PUBLIC assimp-4.1.0/include)

###</ASSIMP>

###<GLFW>
# Don't build GLFW documentation, tests, and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
###</GLFW>

###<BULLET>
add_library(bullet INTERFACE)
target_include_directories(bullet INTERFACE bullet/include)
target_link_libraries(bullet INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/Bullet3Collision.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/Bullet3Dynamics.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletSoftBody.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/LinearMath.lib
)

###</BULLET>

### <SFML>
set(SFML_BUILD_EXAMPLES OFF)
set(SFML_BUILD_DOC OFF)
set(SFML_BUILD_GRAPHICS OFF)
set(SFML_BUILD_WINDOW OFF)
set(SFML_BUILD_NETWORK OFF)
set(SFML_USE_STATIC_STD_LIBS OFF)

set(SFML_BUILD_AUDIO ON)
add_subdirectory(SFML)
### </SFML>
