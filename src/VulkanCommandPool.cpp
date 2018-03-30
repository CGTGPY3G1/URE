#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanPipeline.h"
#include "glm/gtc/type_ptr.hpp"
#include <iostream>
#include "Geometry.h"
namespace B00289996 {
	VulkanCommandPool::VulkanCommandPool() : std::enable_shared_from_this<VulkanCommandPool>(), device(std::shared_ptr<VulkanDevice>()) {

	}
	VulkanCommandPool::VulkanCommandPool(const std::shared_ptr<VulkanDevice>& vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface, const std::shared_ptr<VulkanSwapchain> & vulkanSwapchain, vk::CommandPoolCreateFlags flags)
		: std::enable_shared_from_this<VulkanCommandPool>(), device(vulkanDevice), surface(vulkanSurface), swapchain(vulkanSwapchain), currentBuffer(0) {
		if (device) {
			vk::CommandPoolCreateInfo info;
			info.flags = flags;
			info.queueFamilyIndex = device->GetMainGraphicsQueueFamilyIndex();
			vk::Result result = device->GetDevice().createCommandPool(&info, nullptr, &commandPool);
			if (result != vk::Result::eSuccess) std::cout << result << std::endl;
		}
		else {
			std::cout << "Couldn't create command pool : null device" << std::endl;
		}
	}
	VulkanCommandPool::~VulkanCommandPool() {
		
	}

	void VulkanCommandPool::SetRenderPass(const std::shared_ptr<VulkanRenderPass>& vulkanRenderPass) {
		renderPass = vulkanRenderPass;
	}

	void VulkanCommandPool::SetPipeline(const std::shared_ptr<VulkanPipeline>& vulkanPipline) {
		pipeline = vulkanPipline;
	}

	void VulkanCommandPool::AllocateBuffers(const std::uint32_t & count, const vk::CommandBufferLevel & bufferlevel) {
		vk::CommandBufferAllocateInfo info;
		info.commandPool = commandPool;
		info.level = bufferlevel;
		info.commandBufferCount = count;
		commandBuffers = std::vector<vk::CommandBuffer>(count);
		vk::Result result = device->GetDevice().allocateCommandBuffers(&info, commandBuffers.data());

		if (result != vk::Result::eSuccess) std::cout << "Failed allocating command buffers : " << result << std::endl;
	}

	void VulkanCommandPool::AllocateSubBuffers(const std::uint32_t & count) {
		if (commandBuffers.size() == 0) std::cout << "Can't create secondary command buffers, no primary buffers found!" << std::endl;
		else {
			vk::CommandBufferAllocateInfo info;
			info.commandPool = commandPool;
			info.level = vk::CommandBufferLevel::eSecondary;
			info.commandBufferCount = count;
			if (subBuffers.size() < commandBuffers.size()) subBuffers.resize(commandBuffers.size(), std::vector<vk::CommandBuffer>());
			for (std::size_t i = 0; i < commandBuffers.size(); i++) {
				subBuffers[i] = std::vector<vk::CommandBuffer>(count);
				vk::Result result = device->GetDevice().allocateCommandBuffers(&info, subBuffers[i].data());
				if (result != vk::Result::eSuccess) std::cout << "Failed allocating secondary command buffer " << i << " : " << result << std::endl;
			}
		}
	}

	void VulkanCommandPool::Init(const std::shared_ptr<VulkanSurface> & surface, const std::shared_ptr<VulkanSwapchain>& swapchain, const std::shared_ptr<VulkanRenderPass>& renderPass, const std::shared_ptr<VulkanPipeline>& pipeline) {
		this->surface = surface;
		this->swapchain = swapchain;
		this->renderPass = renderPass;
		this->pipeline = pipeline;
	}

	void VulkanCommandPool::BeginRenderPass(const vk::ClearColorValue & clearcolour, const vk::ClearDepthStencilValue & depthStencil) {
		std::vector<vk::ClearValue> clearValues = { clearcolour, depthStencil };
		commandBuffers[currentBuffer].begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
		commandBuffers[currentBuffer].beginRenderPass(
			vk::RenderPassBeginInfo(
				renderPass->GetLastRenderPass(),
				swapchain->GetBuffers()[currentBuffer].frameBuffer,
				surface->GetRenderArea(),
				clearValues.size(),
				clearValues.data()
			),
			vk::SubpassContents::eInline
		);
		commandBuffers[currentBuffer].setViewport(0, pipeline->GetViewports());
		commandBuffers[currentBuffer].setScissor(0, pipeline->GetScissors());
		currentSubBuffer = 0;
		passStarted = true;
	}

	void VulkanCommandPool::EndRenderPass() {
		commandBuffers[currentBuffer].endRenderPass();
		commandBuffers[currentBuffer].end();
		currentSubBuffer = 0;
		passStarted = true;
	}

	void VulkanCommandPool::BindTexture(const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanTexture>& texture) {

	}

	void VulkanCommandPool::UpdateViewport(const float & x, const float & y, const float & maxX, const float & maxY) {
		std::vector<vk::Viewport> v = pipeline->GetViewports();
		v[0].x = x * v[0].width, v[0].y = y * v[0].height; v[0].width = (maxX - x) * v[0].width, v[0].height = (maxY - y) * v[0].height;
		commandBuffers[currentBuffer].setViewport(0, v);
	}

	void VulkanCommandPool::UpdateView(const std::shared_ptr<VulkanShader>& shader, const UniformVP & newValues) {
		commandBuffers[currentBuffer].updateBuffer<UniformVP>(shader->uniformData["viewUniforms"].buffer, 0, { newValues });
	}

	void VulkanCommandPool::BindShader(const std::shared_ptr<VulkanShader>& shader, std::vector<std::uint32_t> offsets) {
		commandBuffers[currentBuffer].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->GetPipelineLayout(), 0, shader->descriptorSets, offsets);	
	}

	void VulkanCommandPool::BindDescriptor(const std::vector<vk::DescriptorSet>& sets, std::vector<std::uint32_t> offsets) {
		commandBuffers[currentBuffer].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline->GetPipelineLayout(), 0, sets, offsets);
	}
	
	void VulkanCommandPool::BindPipeline() {
		commandBuffers[currentBuffer].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetGraphicsPipeline());
	}

	void VulkanCommandPool::FlushCommandBuffer(const vk::CommandBuffer & commandBuffer) {
		commandBuffer.end();
		std::vector<vk::SubmitInfo> submitInfos = { vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr) };

		// Create fence to ensure that the command buffer has finished executing
		vk::Fence fence = device->GetDevice().createFence(vk::FenceCreateInfo());

		// Submit to the queue
		device->GetGraphicsQueue().submit(submitInfos, fence);
		// Wait for the fence to signal that command buffer has finished executing
		vk::Result result;
		do {
			result = device->GetDevice().waitForFences(1, &fence, VK_TRUE, UINT32_MAX);
		} while (result == vk::Result::eTimeout);
		device->GetDevice().destroyFence(fence);
		
		
	}

	void VulkanCommandPool::FlushCommandBuffer(const std::size_t & index) {
		if (index < commandBuffers.size()) FlushCommandBuffer(commandBuffers[index]);
	}

	void VulkanCommandPool::FlushCommandBuffers() {
		for (std::vector<vk::CommandBuffer>::iterator i = commandBuffers.begin(); i != commandBuffers.end(); ++i) FlushCommandBuffer(*i);
	}

	void VulkanCommandPool::QueueMeshes(const std::vector<std::shared_ptr<VulkanVAO>> & VAOs, const std::vector<UniformM> & modelInfo, const std::vector<VulkanMaterial> & materials) {
		std::size_t count = 0;
		BindPipeline();
		for (const std::shared_ptr<VulkanVAO> & vao : VAOs) {
			commandBuffers[currentBuffer].pushConstants(pipeline->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(UniformM), &modelInfo[count]);
			commandBuffers[currentBuffer].pushConstants(pipeline->GetPipelineLayout(), vk::ShaderStageFlagBits::eFragment, 144, sizeof(VulkanMaterial), &materials[count]);		
			std::vector<vk::DeviceSize> offsets = { 0 };
			commandBuffers[currentBuffer].bindVertexBuffers(0, vao->VBO.buffer, offsets);
			commandBuffers[currentBuffer].bindIndexBuffer(vao->IBO.buffer, 0, vk::IndexType::eUint32);
			commandBuffers[currentBuffer].drawIndexed(vao->IBO.count, 1, 0, 0, 0);
			count++;
		}
	}

	const vk::CommandPool VulkanCommandPool::GetCommandPool() const {
		return commandPool;
	}
	const std::vector<vk::CommandBuffer> & VulkanCommandPool::GetCommandBuffers() const {
		return commandBuffers;
	}
	const vk::CommandBuffer & VulkanCommandPool::GetCommandBuffer(const std::size_t & index) const {
		if (index < commandBuffers.size()) return commandBuffers[index];
		return {};
	}
	const vk::CommandBuffer & VulkanCommandPool::Switch(const bool & flush) {
		const std::size_t oldIndex = currentBuffer;
		//if (flush) FlushCommandBuffer(oldIndex);
		
		currentBuffer = (swapchain->GetNextImage(commandBuffers[currentBuffer]) + 1) % 3;
		return commandBuffers[currentBuffer];
	}
	const bool VulkanCommandPool::SetCurrentBuffer(const std::size_t & index) {
		if (index < commandBuffers.size()) {
			currentBuffer = index;
			return true;
		}
		return false;
	}
	void VulkanCommandPool::Destroy() {
		if (device && commandPool) {
			if(!commandBuffers.empty()) {
				device->GetDevice().freeCommandBuffers(commandPool, commandBuffers.size(), commandBuffers.data());
				device->GetDevice().destroyCommandPool(commandPool);
			}
		}
	}
}