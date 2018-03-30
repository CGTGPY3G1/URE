#ifndef UNNAMED_VULKAN_PIPELINE_H
#define UNNAMED_VULKAN_PIPELINE_H
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>
namespace B00289996 {
	class VulkanDevice;
	class VulkanSurface;
	struct VulkanShader;
	struct VulkanVAO;
	class VulkanPipeline : std::enable_shared_from_this<VulkanPipeline> {
		friend class ContextManagerVulkan;
	public:
		VulkanPipeline();
		VulkanPipeline(const std::shared_ptr<VulkanDevice>& device, const std::shared_ptr<VulkanSurface> & surface, const const vk::RenderPass & renderPass);
		~VulkanPipeline();
		void SetRenderPass(const const vk::RenderPass & renderPass);
		void Construct(const std::vector<std::shared_ptr<VulkanShader>> & shaders, const std::vector<std::shared_ptr<VulkanVAO>> & VAOs, const std::shared_ptr<VulkanPipeline> & oldPipeline = std::shared_ptr<VulkanPipeline>());
		void Merge(const std::vector<std::shared_ptr<VulkanPipeline>> & pipelines);
		void Merge(const std::shared_ptr<VulkanPipeline> & pipeline);
		const vk::PipelineLayout & GetPipelineLayout() const;
		const vk::Pipeline & GetGraphicsPipeline() const;
		const std::vector<vk::Viewport> GetViewports() const;
		const std::vector<vk::Rect2D> GetScissors() const;
		const vk::PipelineCache GetPipelineCache() const;
		void GetPipelineCache(vk::PipelineCache & storage) const;
	private:
		bool cacheCreated, cacheUsed;
		vk::GraphicsPipelineCreateInfo createInfo;
		void Destroy();
		const std::vector<vk::DynamicState> dynamicStates;
		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanSurface> surface;
		vk::PipelineLayout pipelineLayout;
		std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStages;
		std::vector<vk::Viewport> viewports;
		std::vector<vk::Rect2D> scissors;
		std::unordered_map<std::uint16_t, bool> attachedShaders;
		std::unordered_map<std::uint16_t, bool> attachedVAOs;
		vk::Pipeline graphicsPipeline;
		vk::PipelineCache pipelineCache;
		vk::RenderPass renderPass;
		std::vector<vk::PipelineVertexInputStateCreateInfo> vertexInfo;
		bool dirty;
	};
}

#endif // !UNNAMED_VULKAN_PIPELINE_H