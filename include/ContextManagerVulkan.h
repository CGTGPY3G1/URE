#ifndef UNNAMED_CONTEXT_MANAGER_VULKAN_H
#define UNNAMED_CONTEXT_MANAGER_VULKAN_H
#include "ContextManager.h"
#include "VulkanPipeline.h"
#include "VulkanInstanceData.h"
#include <unordered_map>
#include <memory>
#include <vulkan/vulkan.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
namespace B00289996 {
	class Node;
	class Animator;
	struct VulkanInstanceData;
	class VulkanSwapchain;
	class VulkanRenderPass;
	struct VulkanShader;
	struct VulkanVAO;
	struct CullingResult;
	class ContextManagerVulkan : public ContextManager {
		friend class RendererVulkan;
	public:
		ContextManagerVulkan();
		~ContextManagerVulkan();
		void BeginRenderPass();
		void EndRenderPass();
		void Finalise();
		void BindPipelIne();
		void BindShader(const std::shared_ptr<ShaderProgram>& shader);
		void BindTextures(const std::shared_ptr<ShaderProgram>& shader);
		void BindLights(const std::shared_ptr<ShaderProgram>& shader, const std::vector<std::shared_ptr<Node>>& lights);
		//void BindAnimations(const std::shared_ptr<ShaderProgram>& shader, const std::vector<std::shared_ptr<Animator>>& lights);
		void BindViewProjection(const std::shared_ptr<ShaderProgram>& shader, const UniformVP & viewUniforms);
		void UpdateViewport(const float & x, const float & y, const float & maxX, const float & maxY);
		void RenderBatch(const std::shared_ptr<ShaderProgram>& shader, const UniformVP & viewUniforms, const std::vector<std::shared_ptr<Node>> & renderables, const std::shared_ptr<Texture>& texture, const std::vector<std::shared_ptr<Node>>& lights = std::vector<std::shared_ptr<Node>>());
		void RenderAll(const std::shared_ptr<ShaderProgram>& shader, const glm::mat4 & viewProjection, const std::vector<glm::mat4> & transforms);
		std::shared_ptr<VulkanVAO> GetMeshID(const std::shared_ptr<Mesh>& mesh);
		std::shared_ptr<VulkanShader> GetShaderID(const std::shared_ptr<ShaderProgram>& shader);
		std::shared_ptr<VulkanTexture> GetTextureID(const std::shared_ptr<Texture>& texture);
		void RegisterTexture(const std::shared_ptr<Texture>& texture) override;
		void RegisterMesh(const std::shared_ptr<Mesh>& mesh) override;
		void RegisterShader(const std::shared_ptr<ShaderProgram>& shader) override;
		void UnregisterTexture(const std::shared_ptr<Texture>& texture) override;
		void UnregisterMesh(const std::shared_ptr<Mesh>& mesh) override;
		void UnregisterShader(const std::shared_ptr<ShaderProgram>& shader) override;
		void AlignScreenSize();

		void AllocateMemory(const std::vector<CullingResult> & cullingResults);
	private:
		
		bool firstBatch = true, lightsBound = false, texturesBound = false, rebuildPipeline = false;
		std::shared_ptr<VulkanVAO> LoadMesh(const std::shared_ptr<Mesh> & mesh);
		std::shared_ptr<VulkanShader> LoadShader(const std::shared_ptr<ShaderProgram>& shader);
		std::shared_ptr<VulkanTexture> LoadTexture(const std::shared_ptr<Texture>& texture);
		void SetInstanceData(const VulkanInstanceData & data);
		vk::Instance instance;
		std::shared_ptr<VulkanDevice> device;
		std::shared_ptr<VulkanSurface> surface;
		std::shared_ptr<VulkanSwapchain> swapchain;
		std::shared_ptr<VulkanRenderPass> mainRenderPass;
		std::shared_ptr<VulkanCommandPool> commandPool;
		std::shared_ptr<VulkanPipeline> pipeline;
		std::unordered_map<std::uint16_t, std::shared_ptr<VulkanVAO>> meshes;
		std::unordered_map<std::uint16_t, std::shared_ptr<VulkanShader>> shaderPrograms;
		std::unordered_map<std::uint16_t, std::shared_ptr<VulkanTexture>> textures;
		std::unordered_map<std::uint16_t, uint16_t> textureIndices;
		std::uint32_t oldScreenWidth, oldScreenHeight;
	};
}

#endif // !UNNAMED_CONTEXT_MANAGER_VULKAN_H
