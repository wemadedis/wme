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
set(BULLET_COLLISION_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletCollision_vs2010_x64_debug.lib")
set(BULLET_DYNAMICS_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletDynamics_vs2010_x64_debug.lib")
set(BULLET_MATH_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/LinearMath_vs2010_x64_debug.lib")
set(BULLET_SOFTBODY_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/bullet/lib/debug/BulletSoftBody_vs2010_x64_debug.lib")
set(BULLET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bullet/include")
#find_package(Bullet REQUIRED)

add_library(bullet INTERFACE)
target_link_options(bullet INTERFACE "/NODEFAULTLIB:LIBCMTD")
target_include_directories(bullet INTERFACE ${BULLET_INCLUDE_DIR})
target_link_libraries(bullet INTERFACE 
    ${BULLET_COLLISION_LIBRARY} 
    ${BULLET_DYNAMICS_LIBRARY} 
    ${BULLET_MATH_LIBRARY} 
    ${BULLET_SOFTBODY_LIBRARY}
)

if(TARGET bullet)
    get_target_property(BULLET_TARGET_LIBS bullet INTERFACE_LINK_LIBRARIES)
    message("Bullet target libs: ${BULLET_TARGET_LIBS}")
endif()
###</BULLET>


###<DEAR IMGUI>
add_library(imgui imgui/imgui/imgui.cpp imgui/imgui/imgui_draw.cpp imgui/imgui/imgui_widgets.cpp imgui/imgui/imgui_demo.cpp)
target_include_directories(imgui PUBLIC imgui)
###</DEAR IMGUI>