#include "VulkanUniformData.h"
#include "VulkanDevice.h"
namespace B00289996 {
	VulkanUniformData::VulkanUniformData(const std::shared_ptr<VulkanDevice>& vulkanDevice, const std::string & uniformName) : device(vulkanDevice), name(uniformName), pData(nullptr), memory(), buffer(), descriptor() {
		device->GetDevice();
	}

	VulkanUniformData::~VulkanUniformData() {
	}
	const bool VulkanUniformData::IsValid() const {
		return device ? true : false;
	}

	const vk::Device & VulkanUniformData::GetDevice() const {
		return device ? device->GetDevice() : nullptr;
	}
}