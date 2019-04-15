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

###<OpenAL>
add_library(openal INTERFACE)
target_include_directories(openal INTERFACE openal/include)
target_link_libraries(openal INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/openal/libs/win64/OpenAL32.lib")
###</OpenAL>

set(wave_enable_tests OFF)


# add_subdirectory(wave)
message(${CMAKE_CURRENT_SOURCE_DIR})
add_library(wave
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/data_header.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/data_header.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/riff_header.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/riff_header.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/fmt_header.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/fmt_header.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/wave_header.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header/wave_header.cc
  
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header_list.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/header_list.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/error.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/file.h
  ${CMAKE_CURRENT_SOURCE_DIR}/wave/src/wave/file.cc
)

target_include_directories(wave INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/wave/src/")

