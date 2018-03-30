#ifndef UNNAMED_VULKAN_UNIFORM_DATA_H
#define UNNAMED_VULKAN_UNIFORM_DATA_H
#include <vulkan/vulkan.hpp>

namespace B00289996 {
	class VulkanDevice;
	class VulkanUniformData {
		friend class VulkanDevice;
	public:
		VulkanUniformData(const std::shared_ptr<VulkanDevice> & vulkanDevice = std::shared_ptr<VulkanDevice>(), const std::string & uniformName = "");
		~VulkanUniformData();
		template <typename T> void WriteData(const T data);
		const bool IsValid() const;
	private:
		const vk::Device & GetDevice() const;
		std::shared_ptr<VulkanDevice> device;
		std::string name;
		vk::DeviceMemory memory;
		vk::Buffer buffer;
		vk::DescriptorBufferInfo descriptor;
		void *pData;
	};
	template<typename T>
	inline void VulkanUniformData::WriteData(const T data) {
		if (device) {
			vk::Device d = GetDevice();
			pData = d.mapMemory(memory, 0, sizeof(T));
			memcpy(pData, &data, sizeof(T));
			d.unmapMemory(memory);
		}
	}
}
#endif // !UNNAMED_VULKAN_UNIFORM_DATA_H
