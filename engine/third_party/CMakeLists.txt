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

#set(ZLIB_LIBRARIES zlibstatic)
#set(ENABLE_BOOST_WORKAROUND ON)
#set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_BUILD_ZLIB ON)

set(BUILD_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
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

add_library(bullet INTERFACE)
target_include_directories(bullet INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/bullet/include")
set(BULLET_LIBS_DEBUG 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletCollision_vs2010_x64_debug.lib" 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletDynamics_vs2010_x64_debug.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/LinearMath_vs2010_x64_debug.lib" 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletSoftBody_vs2010_x64_debug.lib"
)
set(BULLET_LIBS_RELEASE 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletCollision_vs2010_x64_release.lib" 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletDynamics_vs2010_x64_release.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/LinearMath_vs2010_x64_release.lib" 
    "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/release/BulletSoftBody_vs2010_x64_release.lib"
)

message(${CMAKE_BUILD_TYPE})
if (NOT DEFINED CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE undefined!")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Using release")
    target_link_libraries(bullet INTERFACE ${BULLET_LIBS_RELEASE}) 
else()
    message("Using debug")
    target_link_libraries(bullet INTERFACE ${BULLET_LIBS_DEBUG}) 
endif ()
###</BULLET>


###<DEAR IMGUI>
file(GLOB  imgui_source "imgui/imgui/*.cpp")
add_library(imgui ${imgui_source})
target_include_directories(imgui PUBLIC imgui)
###</DEAR IMGUI>

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

### <RAPIDJSON>
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/rapidjson")
### </RAPIDJSON>