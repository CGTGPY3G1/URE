#ifndef UNNAMED_VULKAN_INSTANCE_DATA_H
#define UNNAMED_VULKAN_INSTANCE_DATA_H
#include <vulkan/vulkan.hpp>
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include <memory>
namespace B00289996 {
	struct VulkanInstanceData {
		vk::Instance instance;
		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanSurface> surface;
	};
}

#endif // !UNNAMED_VULKAN_INSTANCE_DATA_H