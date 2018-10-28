#include "RendererOpenGL3.h"
#include "MeshRenderer.h"
#include "MessagingSystem.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Node.h"
#include "Light.h"
#include "ContextManagerOpenGL3.h"
#include <GL/glew.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Debug.h"
#include "Engine.h"
#include "Animator.h"
#include "AnimatedModel.h"
#include "CullingResult.h"

namespace B00289996 {
	RendererOpenGL3::RendererOpenGL3() : Renderer(std::make_shared<ContextManagerOpenGL3>()){
		
	}
	RendererOpenGL3::~RendererOpenGL3() {
	}

	void RendererOpenGL3::DebugRenderBoneTree(const std::shared_ptr<BoneTransformTree> & tree, const glm::mat4 & model) {
		glm::vec3 position = glm::vec3((model * tree->transform) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		for (std::vector<std::shared_ptr<BoneTransformTree>>::iterator i = tree->children.begin(); i != tree->children.end(); i++) {
			debugDraw.DrawLine(position, glm::vec3((model * (*i)->transform) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)), glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			DebugRenderBoneTree((*i), model);
		}
	}

	void RendererOpenGL3::Render(const std::vector<CullingResult> & cullingResults) {
		
		if (!this->contextManager) {
			std::cout << "No context manager attached to RendererOpenGL3" << std::endl;
			return;
		}
		std::shared_ptr<ContextManagerOpenGL3> context = std::static_pointer_cast<ContextManagerOpenGL3>(this->contextManager);
		for (std::vector<CullingResult>::const_iterator i = cullingResults.begin(); i < cullingResults.end(); i++) {


			std::vector<std::shared_ptr<Node>> renderables = (*i).visibleNodes;
			std::vector<std::shared_ptr<Node>> lights = (*i).lights;
			std::shared_ptr<Node> camera = (*i).camera;

			const bool useLights = !lights.empty(), hasCamera = camera.use_count() > 0, objectsToRender = !renderables.empty();
			/*if (!hasCamera) {
				std::cout << "No Cameras found in render list" << std::endl;
			}
			if (!objectsToRender) {
				std::cout << "Nothing to Render" << std::endl;
			}
			if (!useLights) std::cout << "No Lights to Render" << std::endl;*/



			if (hasCamera && objectsToRender) {

				std::shared_ptr<Camera> cam = camera->GetComponent<Camera>();
				Viewport viewport = cam->GetViewport();

				glViewport((GLsizei)(viewport.min.x * Engine::GetInstance().GetWindowWidth()), (GLsizei)(viewport.min.y * Engine::GetInstance().GetWindowHeight()), (GLsizei)((viewport.max.x - viewport.min.x) * Engine::GetInstance().GetWindowWidth()), (GLsizei)((viewport.max.y - viewport.min.y) * Engine::GetInstance().GetWindowHeight()));
				glm::mat4 view = cam->GetView();
				glm::mat4 projection = cam->GetProjection();
				GLuint shaderID = 0, textureID = 0, meshID = 0;
				bool useTextures = false;

				for (std::vector<std::shared_ptr<Node>>::iterator i = renderables.begin(); i != renderables.end(); ++i) {
					std::shared_ptr<Node> node = (*i);

					std::shared_ptr<MeshRenderer> meshRenderer = node->GetComponent<MeshRenderer>();
					if (meshRenderer) {
						std::uint32_t meshCount = 0;
						std::vector<std::shared_ptr<Mesh>> meshes = meshRenderer->GetMeshes();
						for (std::vector<std::shared_ptr<Mesh>>::iterator j = meshes.begin(); j != meshes.end(); ++j) {
							//debugDraw.DrawOBB(meshRenderer->GetOBB());
						//	debugDraw.DrawSphere(meshRenderer->GetSphere());
							const Material material = meshRenderer->GetMaterial(meshCount);
							std::shared_ptr<ShaderProgram> shader = material.GetShader();
							if (shader) {
								const GLuint sID = context->GetShaderID(shader);
								bool nullShader = shaderID == 0;
								if (sID != shaderID) {
									shaderID = sID;
									glUseProgram(shaderID);
									nullShader = shaderID == 0;
								}
								GLint location = -1;
								if (!nullShader) {
									SetCameraUniforms(shaderID, view, projection, cam->GetPosition());
									SetLightingUniforms(shaderID, lights);
									std::shared_ptr<Mesh> mesh = meshRenderer->GetMesh(meshCount);
									std::shared_ptr<Animator> animator = node->GetComponent<Animator>();
									if (animator) {
										//									std::shared_ptr<BoneTransformTree> boneTree = animator->GetBoneTransformTree();
										//									if (boneTree) DebugRenderBoneTree(boneTree, node->GetWorldTransform());

										std::vector<glm::mat4> b = animator->GetBoneMatrices();
										for (size_t i = 0; i < b.size(); i++) {
											location = glGetUniformLocation(shaderID, (std::string("bones[") + std::to_string(i) + std::string("]")).c_str());
											if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(b[i]));
										}

										location = glGetUniformLocation(shaderID, "useBones");
										if (location >= 0) glUniform1i(location, true);
									}
									else {
										location = glGetUniformLocation(shaderID, "useBones");
										if (location >= 0) glUniform1i(location, false);
									}
									location = glGetUniformLocation(shaderID, "model");
									if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(node->GetWorldTransform()));
									location = glGetUniformLocation(shaderID, "material.ambient");
									if (location >= 0) glUniform4fv(location, 1, glm::value_ptr(material.GetAmbient()));
									location = glGetUniformLocation(shaderID, "material.diffuse");
									if (location >= 0) glUniform4fv(location, 1, glm::value_ptr(material.GetDiffuse()));
									location = glGetUniformLocation(shaderID, "material.specular");
									if (location >= 0) glUniform4fv(location, 1, glm::value_ptr(material.GetSpecular()));
									location = glGetUniformLocation(shaderID, "material.shininess");
									if (location >= 0) glUniform1f(location, material.GetShininess());
									std::vector<std::shared_ptr<Texture>> textures = material.GetTextures();
									if (!textures.empty()) {
										if (!useTextures) {
											useTextures = true;
											location = glGetUniformLocation(shaderID, "useTexture");
											if (location >= 0) glUniform1i(location, useTextures);
											glBindTexture(GL_TEXTURE_2D, 0);
											textureID = 0;
										}
										for (std::vector<std::shared_ptr<Texture>>::iterator i = textures.begin(); i != textures.end(); ++i) {
											std::shared_ptr<Texture> texture = (*i);
											if (texture) {
												TextureType type = texture->GetType();

												GLuint tID = context->GetTextureID(texture);
												if (textureID != tID) {
													textureID = tID;
													std::uint32_t typeNo = (std::uint32_t)type;
													glActiveTexture(GL_TEXTURE0 + typeNo);
													glBindTexture(GL_TEXTURE_2D, textureID);
													location = glGetUniformLocation(shaderID, "textureUnit0");
													glUniform1i(location, 0 + typeNo);
												}
											}
										}
									}
									else {
										useTextures = false;
										location = glGetUniformLocation(shaderID, "useTexture");
										if (location >= 0) glUniform1i(location, useTextures);
										glBindTexture(GL_TEXTURE_2D, 0);
										textureID = 0;
									}
									GLuint mID = context->GetMeshID(mesh);
									if (meshID != mID) meshID = mID;
									if (meshID) {
										glBindVertexArray(meshID);
										glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
										//Debug::PrintImmediately("Rendered Mesh " + std::to_string(meshID), DEBUG_TYPE_SUCCESS);
										glBindVertexArray(0);
									}
								}
							}
							meshCount++;
						}
					}
				}
				//debugDraw.Render(cam->GetViewProjection());
			}
		}
	}

	void RendererOpenGL3::BeginRendering() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void RendererOpenGL3::EndRendering() {
		MessagingSystem::GetInstance().SendEvent(Event(EventType::EVENT_TYPE_SWAP, EventTarget::TARGET_WINDOWING_SYSTEM));
	}

	void RendererOpenGL3::ProcessEvent(const Event & toProcess) {
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

	void RendererOpenGL3::SetCameraUniforms(const GLuint & shaderID, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & camPosition) {
		glUseProgram(shaderID);
		GLint location = glGetUniformLocation(shaderID, "view");
		if(location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
		location = glGetUniformLocation(shaderID, "projection");
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));
		location = glGetUniformLocation(shaderID, "viewProjection");
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection * view));
		location = glGetUniformLocation(shaderID, "viewPosition");
		if (location >= 0) glUniform3fv(location, 1, glm::value_ptr(camPosition));
	}

	void RendererOpenGL3::SetLightingUniforms(const GLuint & shaderID, std::vector<std::shared_ptr<Node>> lights) {
		const unsigned int noOfLights = lights.size() > MAX_LIGHTS ? MAX_LIGHTS : lights.size();
		GLint location = 0;
		location = glGetUniformLocation(shaderID, "numberOfLights");
		if (location >= 0) glUniform1i(location, noOfLights);
		for (unsigned int i = 0; i < noOfLights; i++) {
			std::shared_ptr<Light> lightComponent = lights[i]->GetComponent<Light>();
			std::shared_ptr<LightStruct> light = lightComponent->GetLight();
			light->position = glm::vec4(lights[i]->GetPosition(), (lightComponent->GetLightType() == LightType::POINT_LIGHT ? 1.0f : 0.0f));
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].position").c_str());
			if (location >= 0) glUniform4fv(location, 1, glm::value_ptr(light->position));
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].ambient").c_str());
			if (location >= 0) glUniform3fv(location, 1, glm::value_ptr(light->ambient));
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].diffuse").c_str());
			if (location >= 0) glUniform3fv(location, 1, glm::value_ptr(light->diffuse));
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].specular").c_str());
			if (location >= 0) glUniform3fv(location, 1, glm::value_ptr(light->specular));
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].constant").c_str());
			if (location >= 0) glUniform1f(location, light->constant);
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].linear").c_str());
			if (location >= 0) glUniform1f(location, light->linear);
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].quadratic").c_str());
			if (location >= 0) glUniform1f(location, light->quadratic);
			location = glGetUniformLocation(shaderID, ("lights[" + std::to_string(i) + "].intensity").c_str());
			if (location >= 0) glUniform1f(location, light->intensity);

		}
	}

	void RendererOpenGL3::ProcessDeregistrationEvent(const Event & toProcess) {
		switch (toProcess.GetDataType()) {
		case EventDataType::EVENT_DATA_MESH:
			contextManager->UnregisterMesh(std::any_cast<ObjectContainer<std::shared_ptr<Mesh>>>(toProcess.GetData()).GetObject());
			break;

		case EventDataType::EVENT_DATA_TEXTURE:
			contextManager->UnregisterTexture(std::any_cast<ObjectContainer<std::shared_ptr<Texture>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_SHADER_PROGRAM:
			contextManager->UnregisterShader(std::any_cast<ObjectContainer<std::shared_ptr<ShaderProgram>>>(toProcess.GetData()).GetObject());
			break;
		default:
			break;
		}
	}
	void RendererOpenGL3::ProcessRegistrationEvent(const Event & toProcess) {
		switch (toProcess.GetDataType()) {
		case EventDataType::EVENT_DATA_MESH:
			contextManager->RegisterMesh(std::any_cast<ObjectContainer<std::shared_ptr<Mesh>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_TEXTURE:
			contextManager->RegisterTexture(std::any_cast<ObjectContainer<std::shared_ptr<Texture>>>(toProcess.GetData()).GetObject());
			break;
		case EventDataType::EVENT_DATA_SHADER_PROGRAM:
			contextManager->RegisterShader(std::any_cast<ObjectContainer<std::shared_ptr<ShaderProgram>>>(toProcess.GetData()).GetObject());
			break;
		default:
			break;
		}
	}
}
