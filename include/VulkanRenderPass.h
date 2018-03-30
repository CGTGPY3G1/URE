#ifndef UNNAMED_VULKAN_RENDER_PASS_H
#define UNNAMED_VULKAN_RENDER_PASS_H
#include "VulkanDevice.h"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>
namespace B00289996 {
	class VulkanDevice;
	class VulkanSurface;
	class VulkanRenderPass {
		friend class ContextManagerVulkan;
	public:
		VulkanRenderPass(const std::shared_ptr<VulkanDevice> & vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface);
		~VulkanRenderPass();
		//void AddSubpass(const bool & useColour = true, const bool & useDepth = true);
		vk::RenderPass CreateRenderPass();
		const vk::RenderPass & GetLastRenderPass() const;
		const std::shared_ptr<VulkanRenderPass> Copy() const;
	private:
		void Destroy();

		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanSurface> surface;
		vk::RenderPass lastPass;
		std::vector<vk::SubpassDescription> subpasses;
		std::vector<vk::AttachmentDescription> attachmentDescriptions;
		std::vector<vk::SubpassDependency> dependencies;
		std::vector<vk::AttachmentReference> colorReferences, depthReferences;
	};
}

#endif // !UNNAMED_VULKAN_RENDER_PASS_H