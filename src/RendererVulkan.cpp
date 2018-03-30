#include "RendererVulkan.h"
#include "MeshRenderer.h"
#include "MessagingSystem.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Node.h"
#include "Light.h"
#include "TypeInfo.h"
#include "ContextManagerVulkan.h"
#include <iostream>

namespace B00289996 {
	RendererVulkan::RendererVulkan() : Renderer(std::make_shared<ContextManagerVulkan>()) {
		MessagingSystem::GetInstance().ProcessQueue(EventTarget::TARGET_RENDERING_SYSTEM);
	}
	RendererVulkan::~RendererVulkan() {
	}
	void RendererVulkan::Render(const std::vector<std::shared_ptr<Node>>& objects) {
		if (!this->contextManager) {
			std::cout << "No context manager attached to RendererOpenGL3" << std::endl;
			return;
		}
		std::shared_ptr<ContextManagerVulkan> context = std::static_pointer_cast<ContextManagerVulkan>(this->contextManager);
		std::vector<std::shared_ptr<Node>> renderables;
		std::vector<std::shared_ptr<Node>> lights;
		std::vector<std::shared_ptr<Node>> cameras;
		const BitMask mRenderBitmask = TypeInfo::GetComponentID<MeshRenderer>();
		const BitMask mCameraBitmask = TypeInfo::GetComponentID<Camera>();
		const BitMask mLightBitmask = TypeInfo::GetComponentID<Light>();
		for (std::vector<std::shared_ptr<Node>>::const_iterator i = objects.begin(); i < objects.end(); i++) {
			const std::shared_ptr<Node> object = (*i);
			if (object->HasAnyOfComponents(mRenderBitmask)) renderables.push_back(object);
			if (object->HasAnyOfComponents(mCameraBitmask)) cameras.push_back(object);
			if (object->HasAnyOfComponents(mLightBitmask)) lights.push_back(object);
		}
		const bool useLights = !lights.empty(), hasCamera = !cameras.empty(), objectsToRender = !renderables.empty();
		/*if (!hasCamera) {
			std::cout << "No Cameras found in render list" << std::endl;
		}
		if (!objectsToRender) {
			std::cout << "Nothing to Render" << std::endl;
		}
		if (!useLights) std::cout << "No Lights to Render" << std::endl;*/

		if (hasCamera && objectsToRender) {
			UniformVP vp;
			
			vp.viewProjection = vulkanPerspectiveClip * cameras[0]->GetComponent<Camera>()->GetProjection() * cameras[0]->GetComponent<Camera>()->GetView();
			vp.viewPosition = cameras[0]->GetComponent<Camera>()->GetPosition();

			std::shared_ptr<ShaderProgram> shaderProgram;
			std::shared_ptr<Texture> texture;
			Viewport v = cameras[0]->GetComponent<Camera>()->GetViewport();
			context->UpdateViewport(v.min.x, v.min.y, v.max.x, v.max.y);
			std::vector<std::shared_ptr<Node>> batch;
			//std::vector<std::shared_ptr<VulkanVAO>> vaos;
			for (std::vector<std::shared_ptr<Node>>::iterator i = renderables.begin(); i != renderables.end(); ++i) {
				std::shared_ptr<Node> node = (*i);
				std::shared_ptr<MeshRenderer> meshRenderer = node->GetComponent<MeshRenderer>();
				if (meshRenderer) {
					std::uint32_t meshCount = 0;
					std::vector<std::shared_ptr<Mesh>> meshes = meshRenderer->GetMeshes();
					for (std::vector<std::shared_ptr<Mesh>>::iterator j = meshes.begin(); j != meshes.end(); ++j) {
						const Material material = meshRenderer->GetMaterial(meshCount);
						std::shared_ptr<ShaderProgram> shader = material.GetShader();

						if (shader) {
							if (shaderProgram != shader) {
								shaderProgram = shader;
								context->BindLights(shaderProgram, lights);
								//context->BindViewProjection(shaderProgram, vp);
								context->BindShader(shaderProgram);
							}
						}
					}
				}
				batch.push_back(node);
			}

			context->RenderBatch(shaderProgram, vp, batch, texture, lights);

		}
		else {
			UniformVP tempVP;
			context->RenderBatch(std::shared_ptr<ShaderProgram>(), tempVP, std::vector<std::shared_ptr<Node>>(), std::shared_ptr<Texture>());
		}
		
	}

	void RendererVulkan::BeginRendering() {
		if (!this->contextManager) {
			std::cout << "No context manager attached to RendererOpenGL3" << std::endl;
			return;
		}
		std::shared_ptr<ContextManagerVulkan> context = std::static_pointer_cast<ContextManagerVulkan>(this->contextManager);
		context->AlignScreenSize();
		context->BeginRenderPass();
	}

	void RendererVulkan::EndRendering() {
		if (!this->contextManager) {
			std::cout << "No context manager attached to RendererOpenGL3" << std::endl;
			return;
		}
		std::shared_ptr<ContextManagerVulkan> context = std::static_pointer_cast<ContextManagerVulkan>(this->contextManager);
		context->EndRenderPass();
		context->Finalise();
		MessagingSystem::GetInstance().SendEvent(Event(EventType::EVENT_TYPE_SWAP, EventTarget::TARGET_WINDOWING_SYSTEM));
	}


	void RendererVulkan::ProcessEvent(const Event & toProcess) {
		switch (toProcess.GetType()) {
		case EventType::EVENT_TYPE_REGISTER:
			ProcessRegistrationEvent(toProcess);
			break;
		case EventType::EVENT_TYPE_UNREGISTER:
			ProcessDeregistrationEvent(toProcess);
			break;

		case EventType::EVENT_TYPE_NULL:
			std::cout << "Null Event Type Sent To Renderer";
			break;
		default:
			break;
		}
	}

	void RendererVulkan::ProcessDeregistrationEvent(const Event & toProcess) {
		switch (toProcess.GetDataType()) {
		case EventDataType::EVENT_DATA_MESH:
			if (contextManager) contextManager->UnregisterMesh(std::any_cast<ObjectContainer<std::shared_ptr<Mesh>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_TEXTURE:
			if (contextManager) contextManager->UnregisterTexture(std::any_cast<ObjectContainer<std::shared_ptr<Texture>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_SHADER_PROGRAM:
			if (contextManager) contextManager->UnregisterShader(std::any_cast<ObjectContainer<std::shared_ptr<ShaderProgram>>>(toProcess.GetData()).GetObject());
			break;
		default:
			break;
		}
	}

	void RendererVulkan::ProcessRegistrationEvent(const Event & toProcess) {
		switch (toProcess.GetDataType()) {
		case EventDataType::EVENT_DATA_MESH:
			if (contextManager) contextManager->RegisterMesh(std::any_cast<ObjectContainer<std::shared_ptr<Mesh>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_TEXTURE:
			if (contextManager) contextManager->RegisterTexture(std::any_cast<ObjectContainer<std::shared_ptr<Texture>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_SHADER_PROGRAM:
			if (contextManager) contextManager->RegisterShader(std::any_cast<ObjectContainer<std::shared_ptr<ShaderProgram>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_VULKAN_INSTANCE:
			if (contextManager) {
				std::static_pointer_cast<ContextManagerVulkan>(contextManager)->SetInstanceData(std::any_cast<VulkanInstanceData>(toProcess.GetData()));
				Event e = Event(EventType::EVENT_TYPE_UPDATE, EventTarget::TARGET_RENDERABLE_OBJECTS);
				MessagingSystem::GetInstance().SendEvent(e);
			}
			break;
		default:
			break;
		}
	}
}
