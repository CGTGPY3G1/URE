cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(VERSION 3.8)
project(URE)
project(URE C CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
set(INCLUDE '${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(SOURCE '${CMAKE_CURRENT_SOURCE_DIR}/src/')
include_directories(include)


file(GLOB SOURCES "src/*.cpp")



add_executable(UnnameRenderingEngine ${SOURCES})
find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})
link_libraries(${Vulkan_LIBRARY})
execute_process(COMMAND git submodule update --init --force --recursive
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/assimp/include)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/packages/assimp/lib)


