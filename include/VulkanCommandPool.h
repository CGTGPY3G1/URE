#ifndef UNNAMED_VULKAN_COMMAND_POOL_H
#define UNNAMED_VULKAN_COMMAND_POOL_H

#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
namespace B00289996 {
	class VulkanDevice;
	class VulkanSwapchain;
	class VulkanSurface; 
	class VulkanRenderPass;
	class VulkanPipeline;
	struct VulkanVAO;
	struct VulkanShader;
	struct VulkanMaterial;
	struct VulkanTexture;
	struct UniformM;
	struct UniformVP;
	class VulkanCommandBuffer;
	class VulkanCommandPool : std::enable_shared_from_this<VulkanCommandPool> {
		friend class ContextManagerVulkan;
	public:
		VulkanCommandPool();
		VulkanCommandPool(const std::shared_ptr<VulkanDevice> & vulkanDevice, const std::shared_ptr<VulkanSurface> & vulkanSurface, const std::shared_ptr<VulkanSwapchain> & vulkanSwapchain, vk::CommandPoolCreateFlags flags = {});
		~VulkanCommandPool();
		void SetRenderPass(const std::shared_ptr<VulkanRenderPass> & vulkanRenderPass);
		void SetPipeline(const std::shared_ptr<VulkanPipeline> & vulkanPipline);
		void AllocateBuffers(const std::uint32_t & count, const vk::CommandBufferLevel & bufferlevel = vk::CommandBufferLevel::ePrimary);
		void AllocateSubBuffers(const std::uint32_t & count);
		void Init(const std::shared_ptr<VulkanSurface> & surface, const std::shared_ptr<VulkanSwapchain>& swapchain, const std::shared_ptr<VulkanRenderPass> & renderPass, const std::shared_ptr<VulkanPipeline> & pipeline);
		void BeginRenderPass(const vk::ClearColorValue & clearcolour = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}), const vk::ClearDepthStencilValue & depthStencil = vk::ClearDepthStencilValue(1.0f, 0));
		void EndRenderPass();
		void BindTexture(const std::shared_ptr<VulkanShader>& shader, const std::shared_ptr<VulkanTexture> & texture);
		void UpdateViewport(const float & x, const float & y, const float & maxX, const float & maxY);
		void UpdateView(const std::shared_ptr<VulkanShader>& shader, const UniformVP & newValues);
		void BindShader(const std::shared_ptr<VulkanShader> & shader, std::vector<std::uint32_t> offsets = { 0 });
		void BindDescriptor(const std::vector<vk::DescriptorSet> & sets, std::vector<std::uint32_t> offsets = { 0 });
		void BindPipeline();
		void FlushCommandBuffer(const vk::CommandBuffer & commandBuffer);
		void FlushCommandBuffer(const std::size_t & index);
		void FlushCommandBuffers();
		void QueueMeshes(const std::vector<std::shared_ptr<VulkanVAO>> & VAOs, const std::vector<UniformM> & modelInfo, const std::vector<VulkanMaterial> & materials);
		template <typename Uniforms> void QueueMeshes(const std::vector<std::shared_ptr<VulkanVAO>> & VAOs, const std::vector<Uniforms> & constants);
		const vk::CommandPool GetCommandPool() const;
		const std::vector<vk::CommandBuffer> & GetCommandBuffers() const;
		const vk::CommandBuffer & GetCommandBuffer(const std::size_t & index) const;
		const vk::CommandBuffer & Switch(const bool & flush = true);
		const bool SetCurrentBuffer(const std::size_t & index);
	private:
		bool first = true;
		void Destroy();
		std::vector<vk::ClearValue> clearValues;
		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanRenderPass> renderPass;
		std::shared_ptr<VulkanSurface> surface;
		std::shared_ptr<VulkanPipeline> pipeline;
		std::shared_ptr<VulkanSwapchain> swapchain;
		vk::CommandPool commandPool;
		std::vector<vk::CommandBuffer> commandBuffers;
		std::vector<std::vector<vk::CommandBuffer>> subBuffers;
		std::size_t currentBuffer, currentSubBuffer;
		bool passStarted;
	};
	template<typename Uniforms>
	inline void VulkanCommandPool::QueueMeshes(const std::vector<std::shared_ptr<VulkanVAO>>& VAOs, const std::vector<Uniforms>& constants) {
		std::uint32_t i = 0;
		for (const std::shared_ptr<VulkanVAO> & vao : VAOs) {
			commandBuffers[currentBuffer].pushConstants(pipeline->GetPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(Uniforms), &constants[i]);
			commandBuffers[currentBuffer].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetGraphicsPipeline());
			std::array<vk::DeviceSize, 1> offsets = { 0 };
			commandBuffers[currentBuffer].bindVertexBuffers(0, vao->VBO.buffer, offsets);
			commandBuffers[currentBuffer].bindIndexBuffer(vao->IBO.buffer, vao->IBO.size, vk::IndexType::eUint32);
			commandBuffers[currentBuffer].drawIndexed(vao->IBO.count, 1, 0, 0, 0);
			if(i < constants.size() - 1) i++;
		}
	}
}
#endif // !UNNAMED_VULKAN_COMMAND_POOL_H
