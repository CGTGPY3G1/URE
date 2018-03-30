#ifndef UNNAMED_VULKAN_DEVICE_H
#define UNNAMED_VULKAN_DEVICE_H
#include <vulkan/vulkan.hpp>
#include "Engine.h"
#include <vector>
#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <spirv_cross/spirv_cross.hpp>
namespace B00289996 {
	struct VulkanVertexAttributes {
		vk::PipelineVertexInputStateCreateInfo inputState;
		vk::VertexInputBindingDescription inputBinding;
		std::vector<vk::VertexInputAttributeDescription> inputAttributes;
	};
	struct UniformVP {
		glm::mat4 viewProjection;
		glm::vec3 viewPosition;
	};
	struct UniformLights {	
		glm::vec4 position[MAX_LIGHTS] = { glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) };
		glm::vec4 ambient[MAX_LIGHTS] = { glm::vec4(0.0f) };
		glm::vec4 diffuse[MAX_LIGHTS] = { glm::vec4(1.0f) };
		glm::vec4 specular[MAX_LIGHTS] = { glm::vec4(0.0f) };
		glm::vec4 clqi[MAX_LIGHTS] = { glm::vec4(0.0f) };
		std::uint32_t numberOfLights = 0;
	};
	struct UniformTextures {
		vk::DescriptorImageInfo textures[MAX_TEXTURES] = { vk::DescriptorImageInfo() };
	};
	struct UniformFragConfig {
		std::uint32_t useTextures = 0;
	};
	struct UniformM {
		glm::mat4 model;
		glm::mat4 viewProjection;
		glm::vec3 viewPosition;
	};
	struct VulkanMaterial {
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float shininess;
		std::uint32_t textureIndex;
	};
	struct UniformData {
		std::string name;
		vk::DeviceMemory memory;
		vk::Buffer buffer;
		vk::DescriptorBufferInfo descriptor;
	};
	struct VulkanBoneData {
		glm::mat4 bones[64];
	};

	struct VulkanBuffer {
		vk::DeviceMemory memory;
		vk::Buffer buffer;
		uint32_t size;
		uint32_t count;
		uint32_t offset;
	};
	struct VulkanVAO {
		VulkanVAO(const std::uint16_t & vaoID = UINT16_MAX) : id(vaoID) {}
		const std::uint16_t id = 0;
		VulkanBuffer VBO;
		VulkanBuffer IBO;
		VulkanVertexAttributes attributes;
	};

	struct VulkanTexture {	
		vk::Image image;
		vk::DeviceMemory memory;
		vk::Sampler sampler;
		vk::ImageLayout imageLayout{ vk::ImageLayout::eShaderReadOnlyOptimal };
		vk::ImageView view;
		vk::Extent3D extent{ 0, 0, 1 };
		vk::DescriptorImageInfo descriptor;
		std::uint32_t width, height;
		std::uint32_t mipLevels{ 1 };
		std::uint32_t layerCount{ 1 };
		std::uint32_t id;
		std::uint32_t size;
		bool mapped = false;
	};
	struct ShaderModule {
		vk::ShaderModule sModule;
		vk::ShaderStageFlagBits stageFlag;
	};
	struct VulkanShader {
		friend class VulkanDevice;
	public:
		VulkanShader(const std::string & shaderName = "Unnamed");
		~VulkanShader();
		
		std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
		vk::DescriptorPool descriptorPools;
		std::vector<vk::DescriptorSet> descriptorSets;
		std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
		std::unordered_map<std::string, UniformData> uniformData;
		std::vector<ShaderModule> shaders = std::vector<ShaderModule>();
		const std::string GetName() const;
		operator bool() { return valid; }
		const bool operator ==(const VulkanShader & other) const { return id == other.id && name.compare(other.name) == 0; }
		const bool operator !=(const VulkanShader & other) const { return id != other.id && name.compare(other.name) != 0; }
		const bool operator ==(const std::shared_ptr<VulkanShader> & other) const { return other && id == other->id && name.compare(other->name) == 0; }
		const bool operator !=(const std::shared_ptr<VulkanShader> & other) const { return other && id != other->id && name.compare(other->name) != 0; }
		const std::size_t MaxAnimations() const;
	private:
		bool valid = false;
		std::size_t maxAnimations = 0;
		std::uint16_t id;
		std::string name;
	};
	struct Vertex;
	class Texture;
	class VulkanCommandPool;
	class VulkanSwapchain;
	class VulkanSurface;
	class ShaderProgram;
	class Mesh;
	class VulkanDevice : std::enable_shared_from_this<VulkanDevice> {
		friend class ContextManagerVulkan;
	public:
		VulkanDevice();
		VulkanDevice(vk::PhysicalDevice physicalDevice, const std::vector<const char*> & requestedValidationLayers, const std::vector<const char*> requestedExtensions);
		~VulkanDevice();
		const vk::PhysicalDevice & GetPhysicalDevice() const;
		const vk::Device & GetDevice() const;
		const vk::Queue GetGraphicsQueue() const;
		const vk::PhysicalDeviceProperties GetProperties() const;
		const vk::PhysicalDeviceMemoryProperties GetMemoryProperties() const;
		const std::uint32_t GetMemoryTypeIndex(std::uint32_t type, const vk::MemoryPropertyFlags & properties);
		const vk::FormatProperties GetFormatProperties() const;
		const vk::PhysicalDeviceFeatures GetFeatures() const;
		const std::uint32_t GetFirstQueueFamilyIndex(const vk::QueueFlagBits & queueType);
		const std::uint32_t GetMainGraphicsQueueFamilyIndex() const;
		const std::vector<vk::QueueFamilyProperties> GetQueueFamilyProperties() const;
		const vk::DeviceSize GetMinimumUniformAlignment() const;
		const vk::CommandBuffer GetCommandBuffer(const bool & begin, const vk::CommandBufferUsageFlags & flags = vk::CommandBufferUsageFlags(), const vk::CommandBufferLevel & bufferLevel = vk::CommandBufferLevel::ePrimary);
		void FlushCommandBuffer(const vk::CommandBuffer & commandBuffer);
		void SetSwapChain(const std::shared_ptr<VulkanSwapchain> & swapchain);
		std::vector<vk::SurfaceFormatKHR> GetSufaceFormats(vk::SurfaceKHR & surface);
		vk::FormatProperties GetFormatProperties(const vk::Format & format);
		std::shared_ptr<VulkanTexture> CreateTexture(const std::shared_ptr<Texture> & texture);
		std::shared_ptr<VulkanVAO> CreateVAO(const std::shared_ptr<Mesh>& mesh);
		vk::Format GetImageFormat(const std::uint32_t & channels);
		std::shared_ptr<VulkanShader> CreateShaderDesciptions(const std::shared_ptr<ShaderProgram> & shader);
		const bool DestroyTexture(const std::shared_ptr<VulkanTexture> & texture);
		const bool DestroyVAO(const std::shared_ptr<VulkanVAO> & vao);
		const bool DestroyShader(const std::shared_ptr<VulkanShader> & shader);
		template <typename Uniforms> void UpdateShaderUniforms(const std::shared_ptr<VulkanShader> & shader, const std::string & name, const Uniforms & uniforms, const std::uint32_t & offset = 0);
		template <typename ToAllocate> void AllocateUniformData(UniformData & data, const std::uint32_t & offset = 0, const ToAllocate & d = ToAllocate());
		template <typename DynamicData> std::size_t AllocateDynamicUniformData(UniformData & data, const std::uint32_t & offset = 0, const DynamicData & d = DynamicData());
		vk::AccessFlags AccessFlagsForLayout(const vk::ImageLayout & layout);
		void WaitForQueue();
	private:
		void Destroy();
		void SetImageLayout(vk::CommandBuffer & cmdbuffer, const vk::Image & image, const vk::ImageAspectFlags & aspectMask, const vk::ImageLayout & oldImageLayout, const vk::ImageLayout & newImageLayout, const vk::PipelineStageFlagBits & srcAccess, const vk::PipelineStageFlagBits & dstAccess, const vk::ImageSubresourceRange & subresourceRange);
		vk::PhysicalDevice pDevice;
		vk::Device device;
		vk::Queue graphicsQueue;
		vk::PhysicalDeviceProperties deviceProperties;
		vk::PhysicalDeviceMemoryProperties deviceMemoryProperties;
		vk::FormatProperties formatProperties;
		vk::PhysicalDeviceFeatures features;
		std::vector<vk::QueueFamilyProperties> queueProperties;
		std::uint32_t graphicsQueueFamilyIndex;
		std::shared_ptr<VulkanSwapchain> swapchain;
		std::unordered_map<vk::QueueFlagBits, std::vector<std::uint32_t>> queueIndices;
		vk::CommandPool vaoPool;
	};
	template<typename Uniforms>
	inline void VulkanDevice::UpdateShaderUniforms(const std::shared_ptr<VulkanShader>& shader, const std::string & name, const Uniforms & uniforms, const std::uint32_t & offset) {
		if (shader && shader->uniformData.count(name) > 0) {	
			void * pData = device.mapMemory(shader->uniformData[name].memory, 0, sizeof(Uniforms), vk::MemoryMapFlagBits());
			memcpy(pData, &uniforms, sizeof(Uniforms));
			device.unmapMemory(shader->uniformData[name].memory);
			std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(shader->descriptorSets[0], 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &(shader->uniformData[name].descriptor), nullptr) };
			device.updateDescriptorSets(descriptorWrites, nullptr);
		}
	}

	template<>
	inline void VulkanDevice::UpdateShaderUniforms<UniformLights>(const std::shared_ptr<VulkanShader>& shader, const std::string & name, const UniformLights & uniforms, const std::uint32_t & offset) {
		if (shader && shader->uniformData.count(name) > 0) {
			const size_t o = sizeof(UniformLights);
			void * pData = device.mapMemory(shader->uniformData[name].memory, 0, o, vk::MemoryMapFlagBits());
			memcpy(pData, &uniforms, o);
			device.unmapMemory(shader->uniformData[name].memory);
			std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(shader->descriptorSets[1], 1, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &(shader->uniformData[name].descriptor), nullptr) };
			device.updateDescriptorSets(descriptorWrites, nullptr);
		}
	}
	template<>
	inline void VulkanDevice::UpdateShaderUniforms<UniformTextures>(const std::shared_ptr<VulkanShader>& shader, const std::string & name, const UniformTextures & uniforms, const std::uint32_t & offset) {
		std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(shader->descriptorSets[0], 0, 0, MAX_TEXTURES, vk::DescriptorType::eCombinedImageSampler, uniforms.textures) };
		device.updateDescriptorSets(descriptorWrites, nullptr);
	}

	template<>
	inline void VulkanDevice::UpdateShaderUniforms<std::shared_ptr<VulkanTexture>>(const std::shared_ptr<VulkanShader>& shader, const std::string & name, const std::shared_ptr<VulkanTexture> & uniforms, const std::uint32_t & offset) {
		vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo(uniforms->sampler, uniforms->view, vk::ImageLayout::eGeneral);//uniforms->imageLayout
		std::vector<vk::WriteDescriptorSet> descriptorWrites = { vk::WriteDescriptorSet(shader->descriptorSets[0], 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo) };
		device.updateDescriptorSets(descriptorWrites, nullptr);
	}

	template<typename ToAllocate>
	inline void VulkanDevice::AllocateUniformData(UniformData & data, const std::uint32_t & offset, const ToAllocate & d) {
		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = 0;
		allocInfo.memoryTypeIndex = 0;
		// Create a new buffer
		data.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), sizeof(ToAllocate), vk::BufferUsageFlagBits::eUniformBuffer));
		// Get memory requirements including size, alignment and memory type 
		vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(data.buffer);
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		// Allocate memory for the uniform buffer
		data.memory = device.allocateMemory(allocInfo);
		// Bind memory to buffer
		device.bindBufferMemory(data.buffer, data.memory, offset);

		// Store information in the uniform's descriptor that is used by the descriptor set
		data.descriptor.buffer = data.buffer;
		data.descriptor.offset = offset;
		data.descriptor.range = sizeof(ToAllocate);
	}
	template<typename DynamicData>
	inline std::size_t VulkanDevice::AllocateDynamicUniformData(UniformData & data, const std::uint32_t & offset, const DynamicData & d) {
		size_t minUboAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
		size_t dynamicAlignment = sizeof(DynamicData);
		if (minUboAlignment > 0) {
			dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
		}
		size_t maxUboRange = deviceProperties.limits.maxUniformBufferRange; 
		size_t maxBlocks = maxUboRange / dynamicAlignment;
		size_t bufferSize = maxBlocks * dynamicAlignment;

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = 0;
		allocInfo.memoryTypeIndex = 0;
		data.buffer = device.createBuffer(vk::BufferCreateInfo(vk::BufferCreateFlags(), (vk::DeviceSize)bufferSize, vk::BufferUsageFlagBits::eUniformBuffer));
		// Get memory requirements including size, alignment and memory type 
		vk::MemoryRequirements memoryRequirements = device.getBufferMemoryRequirements(data.buffer);
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible);
		// Allocate memory for the uniform buffer
		data.memory = device.allocateMemory(allocInfo);
		// Bind memory to buffer
		device.bindBufferMemory(data.buffer, data.memory, offset);

		// Store information in the uniform's descriptor that is used by the descriptor set
		data.descriptor.buffer = data.buffer;
		data.descriptor.offset = offset;
		data.descriptor.range = dynamicAlignment;
		return maxBlocks;
	}
}

#endif // !UNNAMED_VULKAN_DEVICE_H