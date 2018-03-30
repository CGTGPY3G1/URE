#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include <iostream>
namespace B00289996 {
	VulkanSwapchain::VulkanSwapchain() {
	}
	VulkanSwapchain::VulkanSwapchain(const std::shared_ptr<VulkanDevice> & vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface)
		: device(vulkanDevice), surface(vulkanSurface), swapchainBuffers(std::vector<VulkanSwapChainBuffer>()), currentBuffer(0), lastBuffer(0), 
		aquireImageSemaphore(), presentSemaphore(), toggles(std::vector<bool>()){
	}

	VulkanSwapchain::~VulkanSwapchain() {
	}

	const vk::Fence & VulkanSwapchain::GetFence() const {
		return swapchainBuffers[currentBuffer].fence;
	}

	void VulkanSwapchain::BuildSwapchain(const std::uint32_t & width, const std::uint32_t & height, const bool & clipped) {
		vk::Device d = device->GetDevice();
		vk::PhysicalDevice pd = device->GetPhysicalDevice();
		vk::SurfaceKHR s = surface->GetSurface();
		vk::SurfaceCapabilitiesKHR surfaceCapabilities;
		vk::Result result = pd.getSurfaceCapabilitiesKHR(s, &surfaceCapabilities);
		if (result == vk::Result::eSuccess) {
			std::vector<vk::PresentModeKHR> surfacePresentModes = pd.getSurfacePresentModesKHR(s);

			if (!(surfaceCapabilities.currentExtent.width == -1 || surfaceCapabilities.currentExtent.height == -1)) {
				size = surface->GetSurfaceSize();
			}
			size = vk::Extent2D(width, height);
			// search for mailbox preset mode, defaulting to immediate if not found
			presentMode = vk::PresentModeKHR::eImmediate;
			for (auto& pm : surfacePresentModes) {
				if (pm == vk::PresentModeKHR::eMailbox) {
					presentMode = vk::PresentModeKHR::eMailbox;
					break;
				}
			}
			std::uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
			if ((surfaceCapabilities.maxImageCount > 0) && (imageCount > surfaceCapabilities.maxImageCount)) imageCount = surfaceCapabilities.maxImageCount;
			
			std::vector<vk::QueueFamilyProperties> queueProps = pd.getQueueFamilyProperties();
			size_t queueCount = queueProps.size();
			std::uint32_t queueIndex = UINT32_MAX;
			for (std::uint32_t i = 0; i < queueCount; i++) {
				if (queueProps[i].queueFlags & vk::QueueFlagBits::eGraphics) {
					if (s && !pd.getSurfaceSupportKHR(i, s)) {
						continue;
					}
					queueIndex = i;
					break; 
				}
			}
			
			
			vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR();
			swapchainCreateInfo.surface = s;
			swapchainCreateInfo.minImageCount = imageCount;
			swapchainCreateInfo.imageFormat = surface->GetColourFormat();
			swapchainCreateInfo.imageColorSpace = surface->GetColourSpace();
			swapchainCreateInfo.imageExtent = size;
			swapchainCreateInfo.imageArrayLayers = 1;
			swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
			swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
			swapchainCreateInfo.clipped = clipped;
			queueFamilyIdices = { queueIndex };

			swapchainCreateInfo.queueFamilyIndexCount = queueFamilyIdices.size();
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIdices.data();
			swapchainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
			swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
			swapchainCreateInfo.presentMode = presentMode;
			swapchain = d.createSwapchainKHR(swapchainCreateInfo);
			images = d.getSwapchainImagesKHR(swapchain);
		}
	}
	void VulkanSwapchain::Destroy() {
		if (device) {
			for (std::size_t i = 0; i < swapchainBuffers.size(); i++) {
				for (std::size_t j = 0; j < swapchainBuffers[i].views.size(); j++) {
					device->GetDevice().destroyImageView(swapchainBuffers[i].views[j]);
				}
				device->GetDevice().destroyFence(swapchainBuffers[i].fence);
				device->GetDevice().destroyFramebuffer(swapchainBuffers[i].frameBuffer);
			}
			device->GetDevice().destroySemaphore(aquireImageSemaphore);
			device->GetDevice().destroySemaphore(presentSemaphore);
			device->GetDevice().freeMemory(depthMemory);
			device->GetDevice().destroyImage(depthImage);
			device->GetDevice().destroySwapchainKHR(swapchain);
		}
	}
	const vk::SwapchainKHR VulkanSwapchain::GetSwapchain() const {
		return swapchain;
	}
	const std::size_t VulkanSwapchain::GetNumberOfBuffers() const {
		return swapchainBuffers.size();
	}
	std::uint32_t VulkanSwapchain::GetNextImage(vk::CommandBuffer & commandBuffer) {
		lastBuffer = currentBuffer;
		
		vk::Result result = device->GetDevice().acquireNextImageKHR(swapchain, UINT64_MAX, aquireImageSemaphore, nullptr, &currentBuffer);

		if (result != vk::Result::eSuccess) {
			// TODO handle eSuboptimalKHR
			std::cout << "Invalid acquire result: " << vk::to_string(result) << std::endl;
			throw std::error_code(result);
		}
		if (!toggles[currentBuffer]) toggles[currentBuffer] = true;
		else {
			result = device->GetDevice().waitForFences(1, &swapchainBuffers[currentBuffer].fence, VK_FALSE, UINT64_MAX);
			device->GetDevice().resetFences(1, &swapchainBuffers[currentBuffer].fence);
		}

		vk::PipelineStageFlags kernelPipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		auto kernel = vk::SubmitInfo(1, &aquireImageSemaphore, &kernelPipelineStageFlags, 1, &commandBuffer, 1, &presentSemaphore);
		const vk::Queue & graphicsQueue = device->GetGraphicsQueue();
		graphicsQueue.submit(1, &kernel, swapchainBuffers[currentBuffer].fence);
		graphicsQueue.presentKHR(vk::PresentInfoKHR(1, &presentSemaphore, 1, &swapchain, &currentBuffer, &result));
		if (result != vk::Result::eSuccess) std::cout << vk::to_string(result) << std::endl;
		graphicsQueue.waitIdle();
		commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
		return currentBuffer;
	}
	const std::vector<VulkanSwapChainBuffer> VulkanSwapchain::GetBuffers() const {
		return swapchainBuffers;
	}
	const vk::PresentModeKHR VulkanSwapchain::GetPresentMode() const {
		return presentMode;
	}
	void VulkanSwapchain::BuildBuffer(const vk::RenderPass & renderPass) {

		depthImage = device->GetDevice().createImage(vk::ImageCreateInfo(vk::ImageCreateFlags(), vk::ImageType::e2D, surface->GetDepthFormat(), vk::Extent3D(size.width, size.height, 1), 1, 1, vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, 
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, queueFamilyIdices.size(), queueFamilyIdices.data(), vk::ImageLayout::eUndefined));

		auto depthMemoryReq = device->GetDevice().getImageMemoryRequirements(depthImage);
		uint32_t depthMemoryTypeIndex = device->GetMemoryTypeIndex(depthMemoryReq.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);

		depthMemory = device->GetDevice().allocateMemory(vk::MemoryAllocateInfo(depthMemoryReq.size * images.size(), depthMemoryTypeIndex));
		device->GetDevice().bindImageMemory(depthImage, depthMemory, 0);
		
		swapchainBuffers.resize(images.size());

		for (std::size_t i = 0; i < images.size(); i++) {
			swapchainBuffers[i].image = images[i];
			swapchainBuffers[i].views.push_back(device->GetDevice().createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), images[i], vk::ImageViewType::e2D, 
					surface->GetColourFormat(), vk::ComponentMapping(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))));
			swapchainBuffers[i].views.push_back(device->GetDevice().createImageView(vk::ImageViewCreateInfo(vk::ImageViewCreateFlags(), depthImage, vk::ImageViewType::e2D, surface->GetDepthFormat(),
				vk::ComponentMapping(), vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1))));
			swapchainBuffers[i].frameBuffer = device->GetDevice().createFramebuffer(vk::FramebufferCreateInfo(vk::FramebufferCreateFlags(), 
				renderPass, swapchainBuffers[i].views.size(), swapchainBuffers[i].views.data(), size.width, size.height, 1));

			swapchainBuffers[i].fence = device->GetDevice().createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
			device->GetDevice().resetFences(1, &swapchainBuffers[i].fence);
		}
		toggles = std::vector<bool>(GetNumberOfBuffers(), false);
		presentSemaphore = device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
		aquireImageSemaphore = device->GetDevice().createSemaphore(vk::SemaphoreCreateInfo());
	}
}
