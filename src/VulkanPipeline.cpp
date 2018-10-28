#include "VulkanPipeline.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "ShaderProgram.h"
namespace B00289996 {
	VulkanPipeline::VulkanPipeline() : std::enable_shared_from_this<VulkanPipeline>(), cacheCreated(false){
	}
	VulkanPipeline::VulkanPipeline(const std::shared_ptr<VulkanDevice>& device, const std::shared_ptr<VulkanSurface> & surface, const vk::RenderPass & rPass) : std::enable_shared_from_this<VulkanPipeline>(), cacheCreated(false), dirty(false),
		cacheUsed(false), pipelineLayout(), viewports(std::vector<vk::Viewport>()), scissors(std::vector<vk::Rect2D>()), pipelineCache(vk::PipelineCache()), dynamicStates({ vk::DynamicState::eViewport, vk::DynamicState::eScissor }), renderPass(rPass) {
		this->device = device;
		this->surface = surface;
	}

	VulkanPipeline::~VulkanPipeline() {
	}

	void VulkanPipeline::SetRenderPass(const vk::RenderPass & renderPass) {
		this->renderPass = renderPass;
	}

	void VulkanPipeline::Construct(const std::vector<std::shared_ptr<VulkanShader>> & shaders, const std::vector<std::shared_ptr<VulkanVAO>> & VAOs, const std::shared_ptr<VulkanPipeline> & oldPipeline) {

		if (!cacheCreated) {
			pipelineCache = oldPipeline ? oldPipeline->GetPipelineCache() : device->GetDevice().createPipelineCache(vk::PipelineCacheCreateInfo());
			cacheCreated = true;
		}
		for (size_t i = 0; i < shaders.size(); i++) {
			
			if (shaders[i]->GetName().compare("Default") == 0) {
				std::vector<vk::PushConstantRange> pushConstantRanges = {
					vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(UniformM)),
					vk::PushConstantRange(vk::ShaderStageFlagBits::eFragment, 144, sizeof(VulkanMaterial)) };
				pipelineLayout = device->GetDevice().createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), (std::uint32_t) shaders[i]->descriptorSetLayouts.size(), shaders[i]->descriptorSetLayouts.data(), pushConstantRanges.size(), pushConstantRanges.data()));
				
				pipelineShaderStages.push_back(vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), shaders[i]->shaders[0].stageFlag, shaders[i]->shaders[0].sModule, "main", nullptr));
				pipelineShaderStages.push_back(vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), shaders[i]->shaders[1].stageFlag, shaders[i]->shaders[1].sModule, "main", nullptr));
			}
		}
		vertexInfo.reserve(VAOs.size());
		for (std::shared_ptr<VulkanVAO> vao : VAOs) {
			vertexInfo.push_back(vao->attributes.inputState);
			attachedVAOs[vao->id] = true;
		}
		auto pipelineInputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo(vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);
		auto pt = vk::PipelineTessellationStateCreateInfo();
		viewports = { surface->GetViewport() };
		scissors = { surface->GetRenderArea() };
		auto pv = vk::PipelineViewportStateCreateInfo(vk::PipelineViewportStateCreateFlagBits(), (std::uint32_t) viewports.size(), viewports.data(), (std::uint32_t) scissors.size(), scissors.data());

		auto pr = vk::PipelineRasterizationStateCreateInfo(vk::PipelineRasterizationStateCreateFlags(), VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eCounterClockwise, VK_FALSE, 0, 0, 0, 1.0f);
		auto pm = vk::PipelineMultisampleStateCreateInfo(vk::PipelineMultisampleStateCreateFlags(), vk::SampleCountFlagBits::e1);
		auto pds = vk::PipelineDepthStencilStateCreateInfo(vk::PipelineDepthStencilStateCreateFlags(), VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual, VK_FALSE, VK_FALSE, vk::StencilOpState(), vk::StencilOpState(), 0, 0);
		std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments = {
			vk::PipelineColorBlendAttachmentState(VK_FALSE, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
				vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			)
		};

		auto pbs = vk::PipelineColorBlendStateCreateInfo(vk::PipelineColorBlendStateCreateFlags(), 0, vk::LogicOp::eClear, colorBlendAttachments.size(), colorBlendAttachments.data());
		auto pdy = vk::PipelineDynamicStateCreateInfo(vk::PipelineDynamicStateCreateFlags(), (std::uint32_t) dynamicStates.size(), dynamicStates.data());
		vk::PipelineCreateFlags flags = oldPipeline ? vk::PipelineCreateFlags(vk::PipelineCreateFlagBits::eDerivative) : vk::PipelineCreateFlags(vk::PipelineCreateFlagBits::eAllowDerivatives);
		createInfo = vk::GraphicsPipelineCreateInfo(flags, (std::uint32_t) pipelineShaderStages.size(), pipelineShaderStages.data(), vertexInfo.data(), &pipelineInputAssemblyState, &pt, &pv, &pr, &pm, &pds, &pbs, &pdy, pipelineLayout, renderPass, 0);
		graphicsPipeline = device->GetDevice().createGraphicsPipeline(pipelineCache, createInfo);
	}

	void VulkanPipeline::Merge(const std::vector<std::shared_ptr<VulkanPipeline>>& pipelines) {
		if (!pipelines.empty()) {
			std::vector<vk::PipelineCache> caches;
			for (std::shared_ptr<VulkanPipeline> pipeline : pipelines) {
				caches.push_back(pipeline->pipelineCache);
			}
			device->GetDevice().mergePipelineCaches(pipelineCache, caches.size(), caches.data());
		}
	}

	void VulkanPipeline::Merge(const std::shared_ptr<VulkanPipeline>& pipeline) {
		if (pipeline) {
			device->GetDevice().mergePipelineCaches(pipelineCache, 1, &pipeline->pipelineCache);
		}
	}

	const vk::PipelineLayout & VulkanPipeline::GetPipelineLayout() const {
		return pipelineLayout;
	}

	const vk::Pipeline & VulkanPipeline::GetGraphicsPipeline() const {
		return graphicsPipeline;
	}

	const std::vector<vk::Viewport> VulkanPipeline::GetViewports() const {
		return { surface->GetViewport() };
	}

	const std::vector<vk::Rect2D> VulkanPipeline::GetScissors() const {
		return  { surface->GetRenderArea() };
	}
	const vk::PipelineCache VulkanPipeline::GetPipelineCache() const {
		return pipelineCache;
	}
	void VulkanPipeline::GetPipelineCache(vk::PipelineCache & storage) const {
		storage = pipelineCache;
	}
	void VulkanPipeline::Destroy() {
		if (device) {
			device->GetDevice().destroyPipelineCache(pipelineCache);
			device->GetDevice().destroyPipelineLayout(pipelineLayout);
			device->GetDevice().destroyPipeline(graphicsPipeline);
		}
	}
}
