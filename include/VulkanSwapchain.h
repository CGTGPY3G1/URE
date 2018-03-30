#ifndef UNNAMED_VULKAN_SWAPCHAIN_H
#define UNNAMED_VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>
namespace B00289996 {
	struct VulkanSwapChainBuffer {
		vk::Image image;
		std::vector<vk::ImageView> views;
		vk::Framebuffer frameBuffer;
		vk::Fence fence;
	};
	class VulkanDevice;
	class VulkanSurface;
	class VulkanSwapchain {
		friend class VulkanDevice;
		friend class ContextManagerVulkan;
	public: 
		VulkanSwapchain();
		VulkanSwapchain(const std::shared_ptr<VulkanDevice> & vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface);
		~VulkanSwapchain();
		const vk::Fence & GetFence() const;
		const std::vector<vk::Image> & GetImages() const { return images; }
		const vk::Semaphore & getPresentCompleteSemaphore() const { return presentSemaphore; }
		const vk::SwapchainKHR GetSwapchain() const;
		const std::size_t GetNumberOfBuffers() const;
		
		std::uint32_t GetNextImage(vk::CommandBuffer & commandBuffer);

		const std::vector<VulkanSwapChainBuffer> GetBuffers() const;
		const vk::PresentModeKHR GetPresentMode() const;
		void BuildBuffer(const vk::RenderPass & renderPass);
		void BuildSwapchain(const std::uint32_t & width = 1280, const std::uint32_t & height = 720, const bool & clipped = true);
	private:
		void Destroy();
		std::vector<bool> toggles;
		vk::DeviceMemory depthMemory;
		vk::Image depthImage;
		std::vector<vk::Image> images;
		std::vector<VulkanSwapChainBuffer> swapchainBuffers;
		vk::Semaphore aquireImageSemaphore, presentSemaphore;
		vk::SwapchainKHR swapchain;
		std::vector<uint32_t> queueFamilyIdices;
		vk::Extent2D size;
		vk::Instance instance;
		vk::PresentModeKHR presentMode;
		std::uint32_t currentBuffer, lastBuffer;
		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanSurface> surface;
	};
}
#endif // !UNNAMED_VULKAN_SWAPCHAIN_H
