cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
cmake_policy(VERSION 3.8)
project(URE)
project(URE C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-std=c++17")

IF(MSVC)
    SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(MSVC)
set(URE_CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")

find_package(Vulkan REQUIRED)

execute_process(COMMAND git submodule update --init --force --recursive
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE STRING "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE STRING "" FORCE)
set(GLFW_INSTALL        OFF CACHE STRING "" FORCE)
add_subdirectory(external/glfw)

# glew
add_definitions(-DGLEW_STATIC)
option(glew-cmake_BUILD_SHARED "" OFF)
add_subdirectory(external/glew-cmake)

# glm
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# ASSIMP
set(ASSIMP_BUILD_ASSIMP_TOOLS 	OFF CACHE STRING "" FORCE)
set(ASSIMP_BUILD_SAMPLES 	OFF CACHE STRING "" FORCE)
set(ASSIMP_BUILD_TESTS		OFF CACHE STRING "" FORCE)
set(ASSIMP_LIBRARY_DIRS 	"external/assimp/build/code")
set(ASSIMP_INCLUDE_DIRS 	"external/assimp/include" "external/assimp/build/include")
add_subdirectory(external/assimp)

set(URE_HEADERS
	./include/AnimatedModel.h
	./include/AnimationSystem.h
	./include/Animator.h
	./include/BitMask.h
	./include/Bounds.h
	./include/Camera.h
	./include/Component.h
	./include/ComponentManager.h
	./include/ContextManager.h
	./include/ContextManagerOpenGL3.h
	./include/ContextManagerVulkan.h
	./include/CullingResult.h
	./include/CullingSystem.h
	./include/Debug.h
	./include/DebugDraw.h
	./include/Emitter.h
	./include/Engine.h
	./include/Enums.h
	./include/Factory.h
	./include/FileInput.h
	./include/Frustum.h
	./include/Geometry.h
	./include/IDGenerator.h
	./include/Input.h
	./include/Light.h
	./include/LightingSystem.h
	./include/Material.h
	./include/MeshRenderer.h
	./include/MessagingSystem.h
	./include/Node.h
	./include/Object.h
	./include/ObjectCreationInfo.h
	./include/RenderableResource.h
	./include/Renderer.h
	./include/RendererOpenGL3.h
	./include/RendererVulkan.h
	./include/ShaderProgram.h
	./include/TextParser.h
	./include/Texture.h
	./include/TextureLoader.h
	./include/TextureLoaderDefault.h
	./include/Transformable.h
	./include/TypeInfo.h
	./include/Utility.h
	./include/VulkanCommandPool.h
	./include/VulkanDevice.h
	./include/VulkanInstanceData.h
	./include/VulkanPipeline.h
	./include/VulkanRenderPass.h
	./include/VulkanSurface.h
	./include/VulkanSwapchain.h
	./include/VulkanUniformData.h
	./include/Window.h
	./include/WindowFactory.h
	./include/WindowGLFW.h
)
file(GLOB_RECURSE URE_SOURCE ${PROJECT_SOURCE_DIR}/src/*)

source_group("Header Files" FILES ${URE_HEADERS})
source_group("Source Files" FILES ${URE_SOURCE})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	include
	src
	src/3rdParty/stb
	${Vulkan_INCLUDE_DIR}
	external/glew-cmake/include 
	external/glew-cmake/src
	external/glm
	external/glfw/include
	${ASSIMP_INCLUDE_DIRS}
)
link_directories(${ASSIMP_LIBRARY_DIRS})

add_executable(URE main.cpp ${URE_HEADERS} ${URE_SOURCE})
target_link_libraries(URE ${Vulkan_LIBRARY} glfw ${GLFW_LIBRARIES} libglew_static assimp)
