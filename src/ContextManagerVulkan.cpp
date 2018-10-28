#include "ContextManagerVulkan.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandPool.h"
#include "ShaderProgram.h"
#include "MeshRenderer.h"
#include "Node.h"
#include "Texture.h"
#include "Geometry.h"
#include "TextParser.h"
#include "Debug.h"
#include "Light.h"
#include "CullingResult.h"
#include <fstream>
#include <utility>
#include <iostream>

namespace B00289996 {
	ContextManagerVulkan::ContextManagerVulkan() : ContextManager(), firstBatch(true){
	}
	ContextManagerVulkan::~ContextManagerVulkan() {
		if (device) {
			device->WaitForQueue();
			for (std::unordered_map<std::uint16_t, std::shared_ptr<VulkanVAO>>::iterator i = meshes.begin(); i != meshes.end(); ++i) {
				device->DestroyVAO((*i).second);
			}
			for (std::unordered_map<std::uint16_t, std::shared_ptr<VulkanTexture>>::iterator i = textures.begin(); i != textures.end(); ++i) {
				device->DestroyTexture((*i).second);
			}
			for (std::unordered_map<std::uint16_t, std::shared_ptr<VulkanShader>>::iterator i = shaderPrograms.begin(); i != shaderPrograms.end(); ++i) {
				device->DestroyShader((*i).second);
			}
			pipeline->Destroy();
			mainRenderPass->Destroy();
			swapchain->Destroy();
			commandPool->Destroy();
			device->Destroy();
			instance.destroySurfaceKHR(surface->GetSurface());
			instance.destroy();
		}
	}

	void ContextManagerVulkan::BeginRenderPass() {
	
		if (firstBatch) {
			std::vector<std::shared_ptr<VulkanVAO>> vaos;
			for (auto item : meshes) vaos.push_back(item.second);
			std::vector<std::shared_ptr<VulkanShader>> shaders;
			for (auto sh : shaderPrograms) shaders.push_back(sh.second);
			pipeline->Construct(shaders, vaos);
			firstBatch = false;
			
		}
		lightsBound = false;
		texturesBound = false;
		commandPool->SetPipeline(pipeline);
		commandPool->BeginRenderPass();

	}

	void ContextManagerVulkan::EndRenderPass() {
		commandPool->EndRenderPass();
	}
	
	void ContextManagerVulkan::Finalise() {
		commandPool->Switch();
	}

	void ContextManagerVulkan::BindPipelIne() {
		commandPool->SetPipeline(pipeline);
		commandPool->BindPipeline();
	}

	void ContextManagerVulkan::BindShader(const std::shared_ptr<ShaderProgram>& shader) {
		BindTextures(shader);
		commandPool->BindShader(GetShaderID(shader));
	}

	void ContextManagerVulkan::BindTextures(const std::shared_ptr<ShaderProgram>& shader) {
		if (!texturesBound) {
			std::shared_ptr<VulkanShader> s = shader ? GetShaderID(shader) : std::shared_ptr<VulkanShader>();
			UniformTextures ut;
			std::size_t size = textures.size();
			std::vector<std::shared_ptr<VulkanTexture>> t;
			std::uint32_t count = 0;
			for (auto toAdd : textures) {
				if (toAdd.second) {
					t.push_back(toAdd.second);
					textureIndices[toAdd.second->id] = count;
					count++;
				}

			}
			while (t.size() < MAX_TEXTURES) t.push_back((*textures.begin()).second);
			for (size_t i = 0; i < MAX_TEXTURES; i++) {
				ut.textures[i] = vk::DescriptorImageInfo(t[i]->sampler, t[i]->view, vk::ImageLayout::eShaderReadOnlyOptimal);
			}
			device->UpdateShaderUniforms(s, "textures", ut);

			texturesBound = true;
		}
	}

	void ContextManagerVulkan::BindLights(const std::shared_ptr<ShaderProgram>& shader, const std::vector<std::shared_ptr<Node>>& lights) {
		if (!lightsBound) {
			std::shared_ptr<VulkanShader> s = GetShaderID(shader);
			UniformLights upl;
			upl.numberOfLights = lights.size() > MAX_LIGHTS ? MAX_LIGHTS : lights.size();
			for (size_t i = 0; i < upl.numberOfLights; i++) {
				std::shared_ptr<Light> light = lights[i]->GetComponent<Light>();
				std::shared_ptr<LightStruct> pl = light->GetLight();
				upl.position[i] = glm::vec4(lights[i]->GetPosition(), (light->GetLightType() == LightType::POINT_LIGHT ? 1.0f : 0.0f));
				upl.ambient[i] = glm::vec4(pl->ambient, 1.0f);
				upl.diffuse[i] = glm::vec4(pl->diffuse, 1.0f);
				upl.specular[i] = glm::vec4(pl->specular, 1.0f);
				upl.clqi[i] = glm::vec4(pl->constant, pl->linear, pl->quadratic, pl->intensity);
			}
			device->UpdateShaderUniforms(s, "pointLights", upl);
			lightsBound = true;
		}
	}

	void ContextManagerVulkan::BindViewProjection(const std::shared_ptr<ShaderProgram>& shader, const UniformVP & viewUniforms) {
		std::shared_ptr<VulkanShader> s = GetShaderID(shader);
		device->UpdateShaderUniforms(s, "viewUniforms", viewUniforms);
	}

	void ContextManagerVulkan::UpdateViewport(const float & x, const float & y, const float & maxX, const float & maxY) {
		commandPool->UpdateViewport(x, y, maxX, maxY);
	}


	void ContextManagerVulkan::RenderBatch(const std::shared_ptr<ShaderProgram>& shader, const UniformVP & viewUniforms, const std::vector<std::shared_ptr<Node>>& renderables, const std::shared_ptr<Texture>& texture, const std::vector<std::shared_ptr<Node>>& lights) {
		if (shader) {
			std::vector<std::shared_ptr<VulkanVAO>> vaos;
			std::vector<VulkanMaterial> materials;
			std::vector<UniformM> transforms;
			
			
			BindShader(shader);
			commandPool->BindPipeline();
			for (auto renderable : renderables) {

				std::shared_ptr<MeshRenderer> meshRenderer = renderable->GetComponent<MeshRenderer>();
				if (meshRenderer) {
					std::uint32_t meshCount = 0;
					std::vector<std::shared_ptr<Mesh>> meshes = meshRenderer->GetMeshes();
					for (std::vector<std::shared_ptr<Mesh>>::iterator j = meshes.begin(); j != meshes.end(); ++j) {
						std::shared_ptr<Mesh> mesh = (*j);					
						if (mesh) {
							UniformM um;
							vaos.push_back(GetMeshID(mesh));
							um.model = renderable->GetWorldTransform();
							um.viewPosition = viewUniforms.viewPosition;
							um.viewProjection = viewUniforms.viewProjection;
							transforms.push_back(um);
							Material m = meshRenderer->GetMaterial(meshCount);
							VulkanMaterial vm;
							vm.ambient = m.GetAmbient();
							vm.diffuse = m.GetDiffuse();
							vm.specular = m.GetSpecular();
							vm.shininess = m.GetShininess();
							vm.textureIndex = m.GetTextures().size() > 0 ? textureIndices[m.GetTexture(0)->GetID()] : MAX_TEXTURES;
							materials.push_back(vm);
							meshCount++;
						}
					}
				}
			}
			if (transforms.size() > 0) commandPool->QueueMeshes(vaos, transforms, materials);
		}
	}

	void ContextManagerVulkan::RenderAll(const std::shared_ptr<ShaderProgram>& shader, const glm::mat4 & viewProjection, const std::vector<glm::mat4>& transforms) {
		static bool first = true;
		std::shared_ptr<VulkanShader> s = GetShaderID(shader);
		UniformVP vp;

		vp.viewProjection = viewProjection;
		device->UpdateShaderUniforms(s, "viewProjection", vp);
		device->UpdateShaderUniforms(s, "textures", (*textures.begin()).second);
		std::vector<std::shared_ptr<VulkanVAO>> vaos;
		for (auto item : meshes) vaos.push_back(item.second);
		if (first) {
			std::vector<std::shared_ptr<VulkanShader>> shaders;
			for (auto sh : shaderPrograms) shaders.push_back(sh.second);
			pipeline->Construct(shaders, vaos);
			first = false;
		}
		commandPool->SetPipeline(pipeline);
		commandPool->BeginRenderPass();
		commandPool->BindShader(s);

		commandPool->QueueMeshes(vaos, transforms);
		commandPool->EndRenderPass();
		commandPool->Switch();

	}

	std::shared_ptr<VulkanVAO> ContextManagerVulkan::GetMeshID(const std::shared_ptr<Mesh>& mesh) {
		const std::uint16_t id = mesh->GetID();
		if (meshes.count(id) > 0) return meshes[id];
		return LoadMesh(mesh);
	}
	std::shared_ptr<VulkanShader> ContextManagerVulkan::GetShaderID(const std::shared_ptr<ShaderProgram>& shader) {
		const std::uint16_t id = shader->GetID();
		if (shaderPrograms.count(id) > 0) return shaderPrograms[id];
		return LoadShader(shader);
	}
	std::shared_ptr<VulkanTexture> ContextManagerVulkan::GetTextureID(const std::shared_ptr<Texture>& texture) {
		const std::uint16_t id = texture->GetID();
		if (shaderPrograms.count(id) > 0) return textures[id];
		return LoadTexture(texture);
	}
	void ContextManagerVulkan::RegisterTexture(const std::shared_ptr<Texture>& texture) {
		if (texture) {
			const std::uint16_t id = texture->GetID();
			if (textures.count(id) > 0) std::cout << "Texture " << id << " already registered!" << std::endl;
			else LoadTexture(texture);
		}
	}
	void ContextManagerVulkan::RegisterMesh(const std::shared_ptr<Mesh>& mesh) {
		if (mesh) {
			const std::uint16_t id = mesh->GetID();
			if (meshes.count(id) > 0) std::cout << "Mesh " << id << " already registered!" << std::endl;
			else LoadMesh(mesh);
		}
	}
	void ContextManagerVulkan::RegisterShader(const std::shared_ptr<ShaderProgram>& shader) {
		if (shader) {
			const std::uint16_t id = shader->GetID();
			if (shaderPrograms.count(id) > 0) std::cout << "Shader Program " << id << " already registered!" << std::endl;
			else LoadShader(shader);
		}
	}
	void ContextManagerVulkan::UnregisterTexture(const std::shared_ptr<Texture>& texture) {
	}
	void ContextManagerVulkan::UnregisterMesh(const std::shared_ptr<Mesh>& mesh) {
	}
	void ContextManagerVulkan::UnregisterShader(const std::shared_ptr<ShaderProgram>& shader) {
	}

	void ContextManagerVulkan::AlignScreenSize() {
		std::uint32_t newWidth = Engine::GetInstance().GetWindowWidth(), newHeight = Engine::GetInstance().GetWindowHeight();
		if (oldScreenWidth != newWidth || oldScreenHeight != newHeight) {
			surface->SetViewport(newWidth, newHeight);
			swapchain->Destroy();
			swapchain = std::make_shared<VulkanSwapchain>(device, surface);
			swapchain->BuildSwapchain(newWidth, newHeight);
			swapchain->BuildBuffer(mainRenderPass->GetLastRenderPass());
			commandPool->Destroy();
			commandPool = std::make_shared<VulkanCommandPool>(device, surface, swapchain, vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));
			commandPool->SetRenderPass(mainRenderPass);
			commandPool->AllocateBuffers((std::uint32_t) swapchain->GetNumberOfBuffers());
			//commandPool->AllocateSubBuffers(5);
			device->SetSwapChain(swapchain);
			oldScreenWidth = newWidth; oldScreenHeight = newHeight;
		}
	}

	void ContextManagerVulkan::AllocateMemory(const std::vector<CullingResult>& cullingResults) {

	}

	std::shared_ptr<VulkanVAO> ContextManagerVulkan::LoadMesh(const std::shared_ptr<Mesh>& mesh) {
		std::shared_ptr<VulkanVAO> toReturn;
		if (mesh) {
			toReturn = device->CreateVAO(mesh);
			if (toReturn) {
				meshes[mesh->GetID()] = toReturn;
				rebuildPipeline = true;
			}
		}
		return toReturn;
	}
	std::shared_ptr<VulkanShader> ContextManagerVulkan::LoadShader(const std::shared_ptr<ShaderProgram>& shader) {
		std::shared_ptr<VulkanShader> toReturn;
		if (shader) {
			const std::uint16_t id = shader->GetID();
			if (shaderPrograms.count(id) != 0) return shaderPrograms[id]; // if the shader program has already been loaded, return it
			else {
				
				toReturn = device->CreateShaderDesciptions(shader);
				if (toReturn) {
					shaderPrograms[id] = toReturn;
					rebuildPipeline = true;
				}
			}
		}
		return toReturn;
	}
	std::shared_ptr<VulkanTexture> ContextManagerVulkan::LoadTexture(const std::shared_ptr<Texture>& texture) {
		std::shared_ptr<VulkanTexture> toReturn;
		if (texture) {
			const std::uint16_t id = texture->GetID();
			if (textures.count(id) != 0) return textures[id]; // if the textures has already been loaded, return it
			else {
				toReturn = device->CreateTexture(texture);
				if (toReturn) {
					textures[id] = toReturn;
					rebuildPipeline = true;
				}
			}
		}
		return toReturn;
	}
	void ContextManagerVulkan::SetInstanceData(const VulkanInstanceData & data) {
		instance = data.instance;	
		device = data.device;
		surface = data.surface;
		mainRenderPass = std::make_shared<VulkanRenderPass>(device, surface);
		mainRenderPass->CreateRenderPass();
		swapchain = std::make_shared<VulkanSwapchain>(device, surface);
		oldScreenWidth = Engine::GetInstance().GetWindowWidth();
		oldScreenHeight = Engine::GetInstance().GetWindowHeight();
		swapchain->BuildSwapchain(oldScreenWidth, oldScreenHeight);
		swapchain->BuildBuffer(mainRenderPass->GetLastRenderPass());
		device->SetSwapChain(swapchain);
		commandPool = std::make_shared<VulkanCommandPool>(device, surface, swapchain, vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));
		commandPool->SetRenderPass(mainRenderPass);
		commandPool->AllocateBuffers((std::uint32_t)swapchain->GetNumberOfBuffers());
		pipeline = std::make_shared<VulkanPipeline>(device, surface, mainRenderPass->GetLastRenderPass());
	}
}
