#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanDevice.h"
#include <iostream>
namespace B00289996 {
	VulkanRenderPass::VulkanRenderPass(const std::shared_ptr<VulkanDevice> & vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface)
		: device(vulkanDevice), surface(vulkanSurface), attachmentDescriptions(std::vector<vk::AttachmentDescription>()), subpasses(std::vector<vk::SubpassDescription>()),
		colorReferences(std::vector<vk::AttachmentReference>()), depthReferences(std::vector<vk::AttachmentReference>()), lastPass(vk::RenderPass()) {

	}

	VulkanRenderPass::~VulkanRenderPass() {
	}

	vk::RenderPass VulkanRenderPass::CreateRenderPass() {

		attachmentDescriptions.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlagBits(), surface->GetColourFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear
			, vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR)); 
		attachmentDescriptions.push_back(vk::AttachmentDescription(vk::AttachmentDescriptionFlagBits(), surface->GetDepthFormat(), vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear
			, vk::AttachmentStoreOp::eDontCare, vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal));
		std::vector<vk::AttachmentReference> colorReferences = std::vector<vk::AttachmentReference>();
		colorReferences.push_back({ vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal) });
		std::vector<vk::AttachmentReference> depthReferences = std::vector<vk::AttachmentReference>();
		depthReferences.push_back({ vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal) });
		subpasses.push_back(vk::SubpassDescription(vk::SubpassDescriptionFlags(), vk::PipelineBindPoint::eGraphics, 0, nullptr, colorReferences.size(), colorReferences.data(), nullptr, depthReferences.data()));
		dependencies.push_back(vk::SubpassDependency(~0U, 0, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::AccessFlagBits::eMemoryRead, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::DependencyFlagBits::eByRegion));
		dependencies.push_back(vk::SubpassDependency(0, ~0U, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe, vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, vk::AccessFlagBits::eMemoryRead, vk::DependencyFlagBits::eByRegion));
		vk::RenderPassCreateInfo renderPassInfo(vk::RenderPassCreateFlags(), attachmentDescriptions.size(), attachmentDescriptions.data(), subpasses.size(), subpasses.data(), dependencies.size(), dependencies.data());
		vk::Result result = device->GetDevice().createRenderPass(&renderPassInfo, nullptr, &lastPass);
		if (result != vk::Result::eSuccess) {
			std::cout << "Failed to create vulkan RenderPass!" << std::endl;
		}
		return lastPass;
	}

	const vk::RenderPass & VulkanRenderPass::GetLastRenderPass() const {
		return lastPass;
	}
	const std::shared_ptr<VulkanRenderPass> VulkanRenderPass::Copy() const {
		std::shared_ptr<VulkanRenderPass> toReturn = std::make_shared<VulkanRenderPass>(device, surface);
		toReturn->lastPass = lastPass;
		toReturn->CreateRenderPass();
		return toReturn;
	}
	void VulkanRenderPass::Destroy() {
		if (device) device->GetDevice().destroyRenderPass(lastPass);
	}
}
