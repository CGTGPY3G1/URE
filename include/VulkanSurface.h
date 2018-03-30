#ifndef UNNAMED_VULKAN_SURFACE_H
#define UNNAMED_VULKAN_SURFACE_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>
namespace B00289996 {
	
	class VulkanDevice;
	class VulkanSurface {
	public:
		VulkanSurface(const std::uint32_t & width, const std::uint32_t & height, const vk::SurfaceKHR & vulkanSurface, const std::shared_ptr<VulkanDevice> & device);
		~VulkanSurface();
		const vk::SurfaceKHR & GetSurface() const;
		const vk::Format &GetColourFormat() const;
		const vk::Format &GetDepthFormat() const;
		const vk::ColorSpaceKHR GetColourSpace() const;
		const vk::Extent2D GetSurfaceSize() const;
		const vk::Rect2D GetRenderArea() const;
		const vk::Viewport GetViewport() const;
		void SetViewport(const float & width, const float & height, const float & x = 0, const float & y = 0, const float & minDepth = 0.0f, const float & maxDepth = 1.0f);
	private:
		void Destroy();
		vk::SurfaceKHR surface;
		vk::Format colourFormat, depthFormat;
		vk::ColorSpaceKHR colourSpace;
		vk::Extent2D surfaceSize;
		vk::Rect2D renderArea;
		vk::Viewport viewport;
		std::shared_ptr<VulkanDevice> device;
	};
}

#endif // !UNNAMED_VULKAN_SURFACE_H