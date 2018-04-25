cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(VERSION 3.8)
project(URE)
project(URE C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(URE_CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")

file(GLOB_RECURSE source_files ${PROJECT_SOURCE_DIR}/src/*)
add_executable(URE ${source_files})

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
link_libraries(${Vulkan_LIBRARY})

execute_process(COMMAND git submodule update --init --force --recursive
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
add_subdirectory(external/glfw)
include_directories(external/glfw/include)

# glew
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(external/glew-cmake)
include_directories(external/glew-cmake/include external/glew-cmake/src)

# glm
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
include_directories(external/glm)

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(URE glfw ${GLFW_LIBRARIES} libglew_static)