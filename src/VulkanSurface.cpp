#include "VulkanSurface.h"
#include "VulkanDevice.h"
namespace B00289996 {
	VulkanSurface::VulkanSurface(const std::uint32_t & width, const std::uint32_t & height, const vk::SurfaceKHR & vulkanSurface, const std::shared_ptr<VulkanDevice> & device) : surface(vulkanSurface) {
		this->device = device;
		surfaceSize = vk::Extent2D(width, height);
		renderArea = vk::Rect2D(vk::Offset2D(), surfaceSize);
		viewport = vk::Viewport(0.0f, 0.0, width, height, 0, 1.0f);
		std::vector<vk::SurfaceFormatKHR> surfaceFormats = device->GetSufaceFormats(surface);
		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
			colourFormat = vk::Format::eB8G8R8A8Unorm;
		else
			colourFormat = surfaceFormats[0].format;
		colourSpace = surfaceFormats[0].colorSpace;
		vk::FormatProperties formatProperties = device->GetFormatProperties(vk::Format::eR8G8B8A8Unorm);

		std::vector<vk::Format> depthFormats = {
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD32Sfloat,
			vk::Format::eD24UnormS8Uint,
			vk::Format::eD16UnormS8Uint,
			vk::Format::eD16Unorm
		};
		for (auto& format : depthFormats) {
			vk::FormatProperties depthFormatProperties = device->GetFormatProperties(format);
			if (depthFormatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
				depthFormat = format;
				break;
			}
		}
		
	}

	VulkanSurface::~VulkanSurface() {
	}

	const vk::SurfaceKHR & VulkanSurface::GetSurface() const {
		return surface;
	}

	const vk::Format & VulkanSurface::GetColourFormat() const {
		return colourFormat;
	}

	const vk::Format & VulkanSurface::GetDepthFormat() const {
		return depthFormat;
	}

	const vk::ColorSpaceKHR VulkanSurface::GetColourSpace() const {
		return colourSpace;
	}
	const vk::Extent2D VulkanSurface::GetSurfaceSize() const {
		return surfaceSize;
	}
	const vk::Rect2D VulkanSurface::GetRenderArea() const {
		return renderArea;
	}
	const vk::Viewport VulkanSurface::GetViewport() const {
		return viewport;
	}

	void VulkanSurface::SetViewport(const float & width, const float & height, const float & x, const float & y, const float & minDepth, const float & maxDepth) {
		surfaceSize = vk::Extent2D(width, height);
		renderArea = vk::Rect2D(vk::Offset2D(), surfaceSize);
		viewport = vk::Viewport(x, y, width, height, minDepth, maxDepth);
	}

	void VulkanSurface::Destroy() {

	}
}
