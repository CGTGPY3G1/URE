#include "VulkanDevice.h"
#include "Geometry.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapchain.h"
#include "VulkanSurface.h"
#include "ShaderProgram.h"
#include "Geometry.h"
#include "IDGenerator.h"
#include "Texture.h"
#include <utility>
#include <fstream>
#include <iostream>

namespace B00289996 {
	VulkanDevice::VulkanDevice() :std::enable_shared_from_this<VulkanDevice>() {
	}
	VulkanDevice::VulkanDevice(vk::PhysicalDevice physicalDevice, const std::vector<const char*>& requestedValidationLayers, const std::vector<const char*> requestedExtensions)
		: std::enable_shared_from_this<VulkanDevice>(), queueIndices(std::unordered_map<vk::QueueFlagBits, std::vector<std::uint32_t>>()) {
		pDevice = physicalDevice;
		vk::PhysicalDeviceProperties gpuProps = pDevice.getProperties();
		vk::PhysicalDeviceMemoryProperties gpuMemoryProps = pDevice.getMemoryProperties();

		auto gpuExtensions = pDevice.enumerateDeviceExtensionProperties();
		auto gpuLayers = pDevice.enumerateDeviceLayerProperties();

		std::vector<const char*> deviceExtensions = std::vector<const char*>();

		for (auto &w : requestedExtensions) {
			for (auto &i : gpuExtensions) {
				if (std::string(i.extensionName).compare(w) == 0) {
					deviceExtensions.emplace_back(w);
					break;
				}
			}
		}

		std::vector<const char*> deviceValidationLayers = std::vector<const char*>();

		for (auto &w : requestedValidationLayers) {
			for (auto &i : gpuLayers) {
				if (std::string(i.layerName).compare(w) == 0) {
					deviceValidationLayers.emplace_back(w);
					break;
				}
			}
		}

		formatProperties = pDevice.getFormatProperties(vk::Format::eR8G8B8A8Unorm);
		features = pDevice.getFeatures();
		deviceMemoryProperties = pDevice.getMemoryProperties();
		queueProperties = pDevice.getQueueFamilyProperties();
		for (std::uint32_t i = 0; i < queueProperties.size(); i++) {
			vk::QueueFlags flags = queueProperties[i].queueFlags;
			if (flags & vk::QueueFlagBits::eGraphics) queueIndices[vk::QueueFlagBits::eGraphics].push_back(i);
			if (flags & vk::QueueFlagBits::eCompute) queueIndices[vk::QueueFlagBits::eCompute].push_back(i);
			if (flags & vk::QueueFlagBits::eTransfer) queueIndices[vk::QueueFlagBits::eTransfer].push_back(i);
			if (flags & vk::QueueFlagBits::eSparseBinding) queueIndices[vk::QueueFlagBits::eSparseBinding].push_back(i);
		}
		float priority = 0.0f;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo = std::vector<vk::DeviceQueueCreateInfo>();
		graphicsQueueFamilyIndex = queueIndices[vk::QueueFlagBits::eGraphics][0];
		queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(vk::DeviceQueueCreateFlags(), graphicsQueueFamilyIndex, 1, &priority));
		deviceProperties = pDevice.getProperties();
		device = pDevice.createDevice(vk::DeviceCreateInfo(vk::DeviceCreateFlags(), queueCreateInfo.size(), queueCreateInfo.data(), deviceValidationLayers.size(),
			deviceValidationLayers.data(), deviceExtensions.size(), deviceExtensions.data(), &features));

		graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
		vaoPool = device.createCommandPool(
			vk::CommandPoolCreateInfo(
				vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
				GetMainGraphicsQueueFamilyIndex()
			)
		);
	}
	VulkanDevice::~VulkanDevice() {

	}
	const vk::PhysicalDevice & VulkanDevice::GetPhysicalDevice() const {
		return pDevice;
	}
	const vk::Device & VulkanDevice::GetDevice() const {
		return device;
	}
	const vk::Queue VulkanDevice::GetGraphicsQueue() const {
		return graphicsQueue;
	}
	const vk::PhysicalDeviceProperties VulkanDevice::GetProperties() const {
		return deviceProperties;
	}
	const vk::PhysicalDeviceMemoryProperties VulkanDevice::GetMemoryProperties() const {
		return deviceMemoryProperties;
	}
	const std::uint32_t VulkanDevice::GetMemoryTypeIndex(std::uint32_t type, const vk::MemoryPropertyFlags & properties) {
		for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
			if ((type & 1) == 1) {
				if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}
			type >>= 1;
		}
		return 0;
	}

	const vk::FormatProperties VulkanDevice::GetFormatProperties() const {
		return formatProperties;
	}
	const vk::PhysicalDeviceFeatures VulkanDevice::GetFeatures() const {
		return features;
	}
	const std::uint32_t VulkanDevice::GetFirstQueueFamilyIndex(const vk::QueueFlagBits & queueType) {
		if(queueIndices.count(queueType) > 0) return queueIndices[queueType][0];
		else return UINT32_MAX;
	}
	const std::uint32_t VulkanDevice::GetMainGraphicsQueueFamilyIndex() const {
		return graphicsQueueFamilyIndex;
	}
	const std::vector<vk::QueueFamilyProperties> VulkanDevice::GetQueueFamilyProperties() const {
		return queueProperties;
	}
	const vk::DeviceSize VulkanDevice::GetMinimumUniformAlignment() const {
		return deviceProperties.limits.minUniformBufferOffsetAlignment;
	}

	const vk::CommandBuffer VulkanDevice::GetCommandBuffer(const bool & begin, const vk::CommandBufferUsageFlags & flags, const vk::CommandBufferLevel & bufferLevel) {
		vk::CommandBuffer cmdBuffer = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo(vaoPool, bufferLevel, 1))[0];
		if (begin) cmdBuffer.begin(vk::CommandBufferBeginInfo(flags));
		return std::move(cmdBuffer);
	}
	void VulkanDevice::FlushCommandBuffer(const vk::CommandBuffer & commandBuffer) {
		commandBuffer.end();
		std::vector<vk::SubmitInfo> submitInfos = { vk::SubmitInfo(0, nullptr, nullptr, 1, &commandBuffer, 0, nullptr) };

		// Create fence to ensure that the command buffer has finished executing
		vk::Fence fence = device.createFence(vk::FenceCreateInfo());

		// Submit to the queue
		graphicsQueue.submit(submitInfos, fence);
		// Wait for the fence to signal that command buffer has finished executing
		device.waitForFences(1, &fence, VK_TRUE, UINT32_MAX);
		device.destroyFence(fence);
		device.freeCommandBuffers(vaoPool, 1, &commandBuffer);
	};

	void VulkanDevice::SetSwapChain(const std::shared_ptr<VulkanSwapchain>& swapchain) {
		this->swapchain = swapchain;
	}
	std::vector<vk::SurfaceFormatKHR> VulkanDevice::GetSufaceFormats(vk::SurfaceKHR & surface) {
		std::vector<vk::SurfaceFormatKHR> toReturn = pDevice.getSurfaceFormatsKHR(surface);
		return toReturn;
	}
	vk::FormatProperties VulkanDevice::GetFormatProperties(const vk::Format & format) {
		return pDevice.getFormatProperties(format);
	}
	std::shared_ptr<VulkanTexture> VulkanDevice::CreateTexture(const std::shared_ptr<Texture>& texture) {
		std::shared_ptr<VulkanTexture> toReturn;
		if (texture) {
			toReturn = std::make_shared<VulkanTexture>();
			toReturn->id = texture->GetID();
			toReturn->width = static_cast<uint32_t>(texture->GetWidth());
			toReturn->height = static_cast<uint32_t>(texture->GetHeight());
			toReturn->mipLevels = 1;
			toReturn->layerCount = 1;

			vk::Buffer stagingBuffer;
			vk::DeviceMemory stagingMemory1;
			toReturn->size = texture->GetDataSize();
			std::vector<uint32_t> queueFamilyIndices{ graphicsQueueFamilyIndex };
			vk::BufferCreateInfo bufferCreateInfo = vk::BufferCreateInfo(vk::BufferCreateFlags(), toReturn->size,
				vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, queueFamilyIndices.size(), queueFamilyIndices.data());
			stagingBuffer = device.createBuffer(bufferCreateInfo);

			vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(stagingBuffer);

			stagingMemory1 = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size,
				GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
			device.bindBufferMemory(stagingBuffer, stagingMemory1, 0);
			void * data = device.mapMemory(stagingMemory1, 0, memoryRequirements.size, vk::MemoryMapFlags());
			memcpy(data, texture->GetTextureData(), (std::size_t) memoryRequirements.size);
			device.unmapMemory(stagingMemory1);
			
			vk::Format format = GetImageFormat(texture->GetChannels());
			
			vk::ImageCreateInfo imageCreateInfo;
			imageCreateInfo.imageType = vk::ImageType::e2D;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.format = format;
			imageCreateInfo.samples = vk::SampleCountFlagBits::e1;
			imageCreateInfo.extent = vk::Extent3D(toReturn->width, toReturn->height, 1);
			imageCreateInfo.usage = vk::ImageUsageFlagBits::eSampled;
			imageCreateInfo.tiling = vk::ImageTiling::eLinear;
			imageCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
			imageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
			imageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
			imageCreateInfo.mipLevels = toReturn->mipLevels;

			std::vector<vk::BufferImageCopy> bufferCopyRegions;
			uint32_t offset = 0;

			vk::BufferImageCopy bufferCopyRegion;
			bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.depth = 1;

			for (uint32_t i = 0; i < toReturn->mipLevels; i++) {
				bufferCopyRegion.imageExtent.width = toReturn->width;
				bufferCopyRegion.imageExtent.height = toReturn->height;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.bufferOffset = offset;
				bufferCopyRegions.push_back(bufferCopyRegion);
				offset += toReturn->size;
			}
		
			vk::Result result = device.createImage(&imageCreateInfo, nullptr, &toReturn->image);
			if (result != vk::Result::eSuccess) std::cout << vk::to_string(result) << std::endl;
			memoryRequirements = device.getImageMemoryRequirements(toReturn->image);
			toReturn->memory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));
			device.bindImageMemory(toReturn->image, toReturn->memory, 0);

			vk::CommandBuffer copyCmd = GetCommandBuffer(true);


			vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, toReturn->mipLevels, 0, toReturn->layerCount);
			SetImageLayout(copyCmd, toReturn->image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eTransfer, subresourceRange);

			copyCmd.copyBufferToImage(stagingBuffer, toReturn->image, vk::ImageLayout::eTransferDstOptimal, bufferCopyRegions);
			SetImageLayout(copyCmd, toReturn->image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, subresourceRange);
			
			FlushCommandBuffer(copyCmd);
			vk::SamplerCreateInfo sampler = vk::SamplerCreateInfo(vk::SamplerCreateFlags(), vk::Filter::eNearest, vk::Filter::eNearest, vk::SamplerMipmapMode::eLinear,
				vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eRepeat, 0.0f, true, 16.0f, true, vk::CompareOp::eAlways);
			//sampler.setMaxLod(toReturn->mipLevels);
			result = device.createSampler(&sampler, nullptr, &toReturn->sampler);

			if (result != vk::Result::eSuccess) std::cout << vk::to_string(result) << std::endl;
			vk::ImageViewCreateInfo view;
			view.viewType = vk::ImageViewType::e2D;
			view.format = imageCreateInfo.format;
			view.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, toReturn->mipLevels, 0, toReturn->layerCount);
			view.subresourceRange.levelCount = toReturn->mipLevels;
			view.image = toReturn->image;
			toReturn->view = device.createImageView(view);
		
			toReturn->descriptor.imageLayout = toReturn->imageLayout;
			toReturn->descriptor.imageView = toReturn->view;
			toReturn->descriptor.sampler = toReturn->sampler;

			device.destroyBuffer(stagingBuffer);
			device.freeMemory(stagingMemory1);		
		}
		return toReturn;
	}
	std::shared_ptr<VulkanVAO> VulkanDevice::CreateVAO(const std::shared_ptr<Mesh>& mesh) {
		std::shared_ptr<VulkanVAO> toReturn;
		if (mesh) {
			std::vector<Vertex> vertices = mesh->GetVertices();
			std::vector<std::uint32_t> indices = mesh->GetIndices();
			if (!vertices.empty() && !indices.empty()) {
				toReturn = std::make_shared<VulkanVAO>(mesh->GetID());
				VulkanBuffer VBO;
				VulkanBuffer IBO;
				toReturn->VBO.count = VBO.count = static_cast<std::uint32_t>(vertices.size());
				toReturn->VBO.offset = VBO.offset = sizeof(Vertex);
				toReturn->VBO.size = VBO.size = VBO.count *  VBO.offset;


				toReturn->IBO.count = IBO.count = static_cast<std::uint32_t>(indices.size());
				toReturn->IBO.offset = IBO.offset = sizeof(std::uint32_t);
				toReturn->IBO.size = IBO.size = IBO.count * IBO.offset;
				std::vector<uint32_t> queueFamilyIndices{ graphicsQueueFamilyIndex };

				VBO.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), VBO.size, vk::BufferUsageFlagBits::eTransferSrc,
					vk::SharingMode::eExclusive, queueFamilyIndices.size(), queueFamilyIndices.data()));

				vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(VBO.buffer);

				VBO.memory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size,
					GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
				void *data;

				data = device.mapMemory(VBO.memory, 0, memoryRequirements.size, vk::MemoryMapFlags());
				memcpy(data, vertices.data(), VBO.size);
				device.unmapMemory(VBO.memory);
				device.bindBufferMemory(VBO.buffer, VBO.memory, 0);

				toReturn->VBO.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), VBO.size, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive, queueFamilyIndices.size(), queueFamilyIndices.data()));

				memoryRequirements = device.getBufferMemoryRequirements(toReturn->VBO.buffer);
				toReturn->VBO.memory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));
				device.bindBufferMemory(toReturn->VBO.buffer, toReturn->VBO.memory, 0);

				IBO.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), IBO.size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, queueFamilyIndices.size(), queueFamilyIndices.data()));
				memoryRequirements = device.getBufferMemoryRequirements(IBO.buffer);
				IBO.memory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));

				data = device.mapMemory(IBO.memory, 0, IBO.size, vk::MemoryMapFlags());
				memcpy(data, indices.data(), IBO.size);
				device.unmapMemory(IBO.memory);
				device.bindBufferMemory(IBO.buffer, IBO.memory, 0);

				toReturn->IBO.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), toReturn->IBO.size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::SharingMode::eExclusive, 0, nullptr));
				memoryRequirements = device.getBufferMemoryRequirements(toReturn->IBO.buffer);
				toReturn->IBO.memory = device.allocateMemory(vk::MemoryAllocateInfo(memoryRequirements.size, GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal)));
				device.bindBufferMemory(toReturn->IBO.buffer, toReturn->IBO.memory, 0);
				vk::CommandBuffer copyCmd = GetCommandBuffer(true);

				std::vector<vk::BufferCopy> copyRegions = { vk::BufferCopy(0, 0, VBO.size) };
				copyCmd.copyBuffer(VBO.buffer, toReturn->VBO.buffer, copyRegions);
				copyRegions = { vk::BufferCopy(0, 0,  IBO.size) };
				copyCmd.copyBuffer(IBO.buffer, toReturn->IBO.buffer, copyRegions);

				FlushCommandBuffer(copyCmd);
				device.destroyBuffer(VBO.buffer);
				device.freeMemory(VBO.memory);
				device.destroyBuffer(IBO.buffer);
				device.freeMemory(IBO.memory);


				// Vertex input binding
				toReturn->attributes.inputBinding.binding = 0;
				toReturn->attributes.inputBinding.stride = sizeof(Vertex);
				toReturn->attributes.inputBinding.inputRate = vk::VertexInputRate::eVertex;
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_POSITION, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_TEXCOORD, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, textureCoords)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_NORMAL, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_TANGENT, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, tangent)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_BITANGENT, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, biTangent)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_BONE_IDS, 0, vk::Format::eR32G32B32A32Uint, offsetof(Vertex, boneIDs)));
				toReturn->attributes.inputAttributes.push_back(vk::VertexInputAttributeDescription(VERTEX_BONE_WEIGHTS, 0, vk::Format::eR32G32B32A32Sfloat, offsetof(Vertex, boneWeights)));
				// Assign to the vertex input state used for pipeline creation
				toReturn->attributes.inputState.flags = vk::PipelineVertexInputStateCreateFlags();
				toReturn->attributes.inputState.vertexBindingDescriptionCount = 1;
				toReturn->attributes.inputState.pVertexBindingDescriptions = &toReturn->attributes.inputBinding;
				toReturn->attributes.inputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(toReturn->attributes.inputAttributes.size());
				toReturn->attributes.inputState.pVertexAttributeDescriptions = toReturn->attributes.inputAttributes.data();
			}
		}
		return toReturn;
	}

	vk::Format VulkanDevice::GetImageFormat(const std::uint32_t & channels) {
		const bool rgba = channels == 4;
		return rgba ? vk::Format::eR8G8B8A8Unorm : vk::Format::eR8G8B8A8Unorm;
		if (features.textureCompressionBC) {
			return rgba ? vk::Format::eBc4UnormBlock : vk::Format::eBc3UnormBlock;
		}
		else if (features.textureCompressionASTC_LDR) {
			return rgba ? vk::Format::eAstc8x8UnormBlock : vk::Format::eAstc8x6UnormBlock;
		}
		
		return rgba ? vk::Format::eAstc8x8UnormBlock : vk::Format::eAstc8x6UnormBlock;
	}

	std::shared_ptr<VulkanShader> VulkanDevice::CreateShaderDesciptions(const std::shared_ptr<ShaderProgram> & shader) {
		std::shared_ptr<VulkanShader> toReturn;
		std::uint32_t id = shader->GetID();
		if (shader->GetName().compare("Default") == 0) {
			std::vector<vk::ShaderModule> shaderIDs;
			std::vector<Shader> shaders = shader->GetShaders();
			std::vector<ShaderModule> modules;
			//std::vector<std::vector<vk::DescriptorSetLayoutBinding>> slb;
			for (std::size_t i = 0; i < shaders.size(); i++) {
				Shader s = shaders[i];
				std::string location = "../assets/Shaders/Vulkan/" + s.name + "." + (s.type == ShaderType::VERTEX_SHADER ? "vert.spv" : "frag.spv");
				std::ifstream file(location, std::ios::binary | std::ios::ate);
				std::streamsize vertSize = file.tellg();
				file.seekg(0, std::ios::beg);
				std::vector<char> spirVData(vertSize);
				file.read(spirVData.data(), vertSize);

				if (spirVData.empty()) {
					return toReturn;
				}
				else {
					ShaderModule mod;
					mod.stageFlag = s.type == ShaderType::VERTEX_SHADER ? vk::ShaderStageFlagBits::eVertex : vk::ShaderStageFlagBits::eFragment;
					mod.sModule = device.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), spirVData.size(), reinterpret_cast<uint32_t*>(spirVData.data())));
					
					//spirv_cross::CompilerGLSL glslData((std::uint32_t*)spirVData.data(), spirVData.size());
					//spirv_cross::Compiler comp((std::uint32_t*)spirVData.data(), spirVData.size());
					//// The SPIR-V is now parsed, and we can perform reflection on it.
					//spirv_cross::ShaderResources resources = comp.get_shader_resources();
					//mod.stageFlag = GetVulkanStageFlags(comp.get_execution_model());
					//mod.sModule = device.createShaderModule(vk::ShaderModuleCreateInfo(vk::ShaderModuleCreateFlags(), spirVData.size(), reinterpret_cast<uint32_t*>(spirVData.data())));
					//// Get all sampled images in the shader.
					//for (auto &resource : resources.sampled_images) {
					//	mod.pushConstants = comp.get_active_buffer_ranges(resources.push_constant_buffers.front().id);
					//	mod.uniforms = comp.get_active_buffer_ranges(resources.uniform_buffers.front().id);
					//	//std::vector<spirv_cross::BufferRange> images = comp.get_active_buffer_ranges(resources..front().id);
					//	std::uint32_t binding = comp.get_decoration(resource.id, spv::DecorationBinding);
					//	std::uint32_t set = comp.get_decoration(resource.id, spv::DecorationDescriptorSet);
					//	if (set >= slb.size()) slb.resize(set, std::vector<vk::DescriptorSetLayoutBinding>());
					//	vk::DescriptorSetLayoutBinding b;
					//	b.binding = binding;
					//	b.stageFlags = mod.stageFlag;
					//}
					modules.push_back(mod);
				}
			}
			std::shared_ptr<VulkanShader> toReturn = std::make_shared<VulkanShader>(shader->GetName());
			toReturn->shaders = modules;
			toReturn->descriptorPoolSizes = { 
				vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1), 
				vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, 1),
				vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, MAX_TEXTURES)
			};
			
			toReturn->descriptorPools = device.createDescriptorPool(vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 3, toReturn->descriptorPoolSizes.size(), toReturn->descriptorPoolSizes.data()));

			std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings;
			setLayoutBindings.push_back(vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eUniformBufferDynamic, 1, vk::ShaderStageFlagBits::eVertex, nullptr));
			setLayoutBindings.push_back(vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eCombinedImageSampler, MAX_TEXTURES, vk::ShaderStageFlagBits::eFragment, nullptr));
			
			toReturn->descriptorSetLayouts.push_back(device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), setLayoutBindings.size(), setLayoutBindings.data())));
			setLayoutBindings.clear();
			setLayoutBindings.push_back(vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment, nullptr));
			toReturn->descriptorSetLayouts.push_back(device.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), setLayoutBindings.size(), setLayoutBindings.data())));
			
			toReturn->descriptorSets = device.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(toReturn->descriptorPools, toReturn->descriptorSetLayouts.size(), toReturn->descriptorSetLayouts.data()));

			UniformData uData;
			AllocateUniformData<UniformLights>(uData);
			toReturn->uniformData["pointLights"] = uData;
			//UniformData bData;
			//toReturn->maxAnimations = AllocateDynamicUniformData<VulkanBoneData>(uData);
			//toReturn->uniformData["boneMatrices"] = bData;
			return toReturn;
		}
		return std::shared_ptr<VulkanShader>();
	}

	const bool VulkanDevice::DestroyTexture(const std::shared_ptr<VulkanTexture>& texture) {
		if (texture) {
			if (texture->sampler) {
				device.destroySampler(texture->sampler);
				texture->sampler = vk::Sampler();
			}
			if (texture->view) {
				device.destroyImageView(texture->view);
				texture->view = vk::ImageView();
			}
			if (texture->image) {
				device.destroyImage(texture->image);
				texture->image = vk::Image();
			}
			if (texture->memory) {
				device.freeMemory(texture->memory);
				texture->memory = vk::DeviceMemory();
			}
			return true;
		}
		return false;
	}

	const bool VulkanDevice::DestroyVAO(const std::shared_ptr<VulkanVAO>& vao) {
		if (vao) {
			device.destroyBuffer(vao->VBO.buffer);
			device.freeMemory(vao->VBO.memory);

			device.destroyBuffer(vao->IBO.buffer);
			device.freeMemory(vao->IBO.memory);
			return true;
		}
		return false;
	}

	const bool VulkanDevice::DestroyShader(const std::shared_ptr<VulkanShader>& shader) {
		if (shader) {
			for (auto i : shader->uniformData) {
				device.destroyBuffer(i.second.buffer);
				device.freeMemory(i.second.memory);
			}
			for (auto layout : shader->descriptorSetLayouts) {
				device.destroyDescriptorSetLayout(layout);
			}
			device.destroyDescriptorPool(shader->descriptorPools);
			for (auto shaderModule : shader->shaders) {
				device.destroyShaderModule(shaderModule.sModule);
			}
			return true;
		}
		return false;
	}



	vk::AccessFlags VulkanDevice::AccessFlagsForLayout(const vk::ImageLayout & layout) {
		switch (layout) {
		case vk::ImageLayout::ePreinitialized:
			return vk::AccessFlagBits::eHostWrite;
		case vk::ImageLayout::eTransferDstOptimal:
			return vk::AccessFlagBits::eTransferWrite;
		case vk::ImageLayout::eTransferSrcOptimal:
			return vk::AccessFlagBits::eTransferRead;
		case vk::ImageLayout::eColorAttachmentOptimal:
			return vk::AccessFlagBits::eColorAttachmentWrite;
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
			return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			return vk::AccessFlagBits::eShaderRead;
		default:
			return vk::AccessFlags();
		}
	}

	void VulkanDevice::WaitForQueue() {
		if (graphicsQueue) graphicsQueue.waitIdle();
	}

	void VulkanDevice::Destroy() {
		device.destroyCommandPool(vaoPool);
		WaitForQueue();
		device.waitIdle();
		device.destroy();

	}

	void VulkanDevice::SetImageLayout(vk::CommandBuffer & cmdbuffer, const vk::Image & image, const vk::ImageAspectFlags & aspectMask, const vk::ImageLayout & oldImageLayout,
		const vk::ImageLayout & newImageLayout, const vk::PipelineStageFlagBits & srcAccess, const vk::PipelineStageFlagBits & dstAccess, const vk::ImageSubresourceRange & subresourceRange) {
		// Create an image barrier object
		vk::ImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.oldLayout = oldImageLayout;
		imageMemoryBarrier.newLayout = newImageLayout;
		imageMemoryBarrier.image = image;
		imageMemoryBarrier.subresourceRange = subresourceRange;
		imageMemoryBarrier.srcAccessMask = AccessFlagsForLayout(oldImageLayout);
		imageMemoryBarrier.dstAccessMask = AccessFlagsForLayout(newImageLayout);
		cmdbuffer.pipelineBarrier(srcAccess, dstAccess, vk::DependencyFlags(), nullptr, nullptr, imageMemoryBarrier);
	}
	
	VulkanShader::VulkanShader(const std::string & shaderName) : name(shaderName) {
		id = IDGenerator<VulkanShader, std::uint32_t>::GetInstance().GetNewID();
	}
	VulkanShader::~VulkanShader() {
		IDGenerator<VulkanShader, std::uint32_t>::GetInstance().RetireID(id);
	}
	const std::string VulkanShader::GetName() const {
		return name;
	}
	const std::size_t VulkanShader::MaxAnimations() const {
		return maxAnimations;
	}
}
