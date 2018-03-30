#include "FileInput.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include "Node.h"
#include "Geometry.h"
#include "MeshRenderer.h"
#include "Utility.h"
#include "MessagingSystem.h"
#include <filesystem>
#include "Engine.h"
#include "TextureLoaderDefault.h"
#include "ShaderProgram.h"
#include "AnimatedModel.h"
#include "Camera.h"
#include "Light.h"
#include "Animator.h"
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
constexpr float MAX_MATERUAL_ALPHA = 1.0f;
namespace B00289996 {
	
	const std::uint64_t FileInput::NumberOfLoadedVertices() const {
		return vertexCount;
	}

	FileInput::FileInput() : textureLoader(std::move(std::make_unique<TextureLoaderDefault>())), meshes(std::unordered_map<std::string, std::shared_ptr<Mesh>>()),
		materials(std::unordered_map<std::string, Material>()), textures(std::unordered_map<std::string, std::shared_ptr<Texture>>()), shaders(std::unordered_map<std::string, std::shared_ptr<ShaderProgram>>()), unnamedAnimCount(0), vertexCount(0){

	}

	FileInput::~FileInput() {
	
	}
	const bool FileExists(const std::string & filePath) {
		return std::experimental::filesystem::exists(std::experimental::filesystem::path(filePath));
	}

	struct FindAINodeByName {
		FindAINodeByName(const std::string & name) : n(name) {}
			bool operator()(const aiNodeAnim * other) { return other && n.compare(std::string(other->mNodeName.C_Str())) == 0; }
			bool operator()(const aiNode * other) { return other && n.compare(std::string(other->mName .C_Str())) == 0; }
		private:
			std::string n;
	};

	const std::shared_ptr<Node> FileInput::ProcessNode(const std::string & directory, const std::string & fileName, const aiScene * scene, const aiNode * node, std::shared_ptr<Node> parent) {
		const std::size_t numberOfMeshes = node->mNumMeshes;	
		const std::string nodeName = node->mName.C_Str();
		std::shared_ptr<Node> toReturn = Engine::GetInstance().CreateNode(nodeName);
		aiMatrix4x4 transform = node->mTransformation;

		aiVector3D position(0.0f), scale(1.0f);
		aiQuaternion rotation = aiQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
		transform.Decompose(scale, rotation, position);
		toReturn->SetPosition(AIVec3ToVec3(position));
		toReturn->SetRotation(AIQuaternionToQuat(rotation));
		toReturn->SetScale(AIVec3ToVec3(scale));

		if (parent) toReturn->SetParent(parent);
		bool useAnimation = false;
		if (numberOfMeshes > 0) {
			bool first = true;
			std::shared_ptr<MeshRenderer> mRenderer = toReturn->AddComponent<MeshRenderer>();
			for (std::size_t i = 0; i < numberOfMeshes; ++i) {
				const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				std::string key = directory + fileName + mesh->mName.C_Str();

				if (this->meshes.count(key) != 0) {
					if (!mRenderer) mRenderer = toReturn->AddComponent<MeshRenderer>();
					if (mRenderer) {
						mRenderer->AddMesh(this->meshes[key]);
						mRenderer->AddMaterial(this->materials[key]);
					}
				}
				else {
					if (mesh) {

						const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

						std::vector<Vertex> verts(mesh->mNumVertices, Vertex());
						std::vector<std::uint32_t> indices;
						std::string name = mesh->mName.C_Str();
						const bool useNormals = mesh->HasNormals();
						const bool useTangents = mesh->HasTangentsAndBitangents();
						const bool useBones = mesh->HasBones();
						for (std::size_t j = 0; j < mesh->mNumVertices; ++j) {
							vertexCount++;
							verts[j].position = AIVec3ToVec3(mesh->mVertices[j]);

							if (useNormals) verts[j].normal = AIVec3ToVec3(mesh->mNormals[j]);
							if (useTangents) {
								verts[j].tangent = AIVec3ToVec3(mesh->mTangents[j]);
								verts[j].biTangent = AIVec3ToVec3(mesh->mBitangents[j]);
							}
							bool found = false;
							for (unsigned int k = 0; k < mesh->GetNumUVChannels() && !found; k++) {
								if (mesh->HasTextureCoords(k)) {
									verts[j].textureCoords = AIVec3ToVec2(mesh->mTextureCoords[k][j]);
									found = true;
								}
							}
							if (mesh->HasTextureCoords(0)) verts[j].textureCoords = AIVec3ToVec2(mesh->mTextureCoords[0][j]);
						}

						for (std::size_t j = 0; j < mesh->mNumFaces; ++j) {
							const aiFace& face = mesh->mFaces[j];
							for (std::size_t k = 0; k < face.mNumIndices; k++) {
								indices.push_back(face.mIndices[k]);
							}
						}


						std::shared_ptr<Mesh> mMesh = std::make_shared<Mesh>(name);
						mMesh->useBoneData = useAnimation;
						mMesh->SetVertices(verts);
						mMesh->SetIndices(indices);
						Material rMaterial = Material();
						aiColor3D newColour(0.0f, 0.0f, 0.0f);
						float shininess = 0.0f;
						if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, newColour))
							rMaterial.SetDiffuse(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
						if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, newColour))
							rMaterial.SetAmbient(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
						if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, newColour))
							rMaterial.SetSpecular(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
						if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
							rMaterial.SetShininess(shininess);

						rMaterial.SetShader(LoadShaderProgram("Default"));
						// Add Texture Loader
						for (unsigned int k = 0; k < material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE); k++) {
							aiString path;
							material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, k, &path);
							if (path.length > 0) {
								std::shared_ptr<Texture> t = LoadTexture(directory, path.C_Str(), TextureType::DIFFUSE);
								if (t) rMaterial.AddTexture(t);
							}
						}
						mRenderer->AddMesh(mMesh);
						mRenderer->AddMaterial(rMaterial);
						this->meshes[key] = mMesh;
						this->materials[key] = rMaterial;
						Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_RENDERING_SYSTEM, EventPriority::EVENT_ADD_QUEUE, EventDataType::EVENT_DATA_MESH, std::any(ObjectContainer<std::shared_ptr<Mesh>>(mMesh)));
						mMesh->eventData = e;
						mMesh->PushToRenderer();
						mMesh->ObserveRenderSwitch();
					}
					else std::cout << "Couldn't Load Mesh " << i << std::endl;
				}
			}
		}
		for (size_t i = 0; i < node->mNumChildren; i++) {
			ProcessNode(directory, fileName, scene, node->mChildren[i], toReturn);
		}
		return toReturn;
	}

	const std::shared_ptr<Node> ProcessLights(const std::string & directory, const aiScene * scene) {
		if (scene->HasCameras()) {
			std::shared_ptr<Node> toReturn = std::make_shared<Node>("Lights");
			for (size_t i = 0; i < scene->mNumLights; i++) {
				aiLight * light = scene->mLights[i];
				
				if (light->mType = aiLightSourceType::aiLightSource_SPOT) {
					std::shared_ptr<Node> lightNode = std::make_shared<Node>("Light");
					std::shared_ptr<Light> mLight = lightNode->AddComponent<Light>();
					mLight->SetLightType(LightType::POINT_LIGHT);
					std::shared_ptr<LightStruct> p = mLight->GetLight();
					p->ambient = AIColor3DToVec4(light->mColorAmbient, 1.0f);
					p->diffuse = AIColor3DToVec4(light->mColorDiffuse, 1.0f);
					p->specular = AIColor3DToVec4(light->mColorSpecular, 1.0f);
					//p->position = AIVec3ToVec3(light->mPosition);
					p->constant = light->mAttenuationConstant;
					p->linear = light->mAttenuationLinear;
					p->quadratic = light->mAttenuationQuadratic;
					lightNode->SetParent(toReturn);
				}
				else if (light->mType = aiLightSourceType::aiLightSource_DIRECTIONAL) {
					std::shared_ptr<Node> lightNode = std::make_shared<Node>("Light");
					std::shared_ptr<Light> mLight = lightNode->AddComponent<Light>();
					mLight->SetLightType(LightType::DIRECTION_LIGHT);
					std::shared_ptr<LightStruct> p = mLight->GetLight();
					p->ambient = AIColor3DToVec4(light->mColorAmbient, 1.0f);
					p->diffuse = AIColor3DToVec4(light->mColorDiffuse, 1.0f);
					p->specular = AIColor3DToVec4(light->mColorSpecular, 1.0f);
					lightNode->SetParent(toReturn);
				}
			}
			return toReturn;
		}
		
		return std::shared_ptr<Node>();
	}

	void FileInput::AddMeshesAndMaterials(const aiScene * scene, const std::string & directory, const std::string & fileName, std::shared_ptr<Node> & node, const MeshConstructionInfo & info) {
		std::string key = directory + fileName + info.meshName;

		if (meshes.count(key) != 0) {
			std::shared_ptr<MeshRenderer> renderer = node->GetComponent<MeshRenderer>();
			if (!renderer) renderer = node->AddComponent<MeshRenderer>();
			if (renderer) {
				renderer->AddMesh(meshes[key]);
				renderer->AddMaterial(materials[key]);
			}
		}
		else {
			std::shared_ptr<Mesh> mMesh = std::make_shared<Mesh>(info.meshName);

			mMesh->useBoneData = scene->mMeshes[info.meshIndex]->HasBones();
			mMesh->SetVertices(info.verts);
			mMesh->SetIndices(info.indices);
			aiMaterial * material = scene->mMaterials[info.materialIndex];
			Material rMaterial = Material();
			aiColor3D newColour(0.0f, 0.0f, 0.0f);
			float shininess = 0.0f;
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, newColour))
				rMaterial.SetDiffuse(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, newColour))
				rMaterial.SetAmbient(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, newColour))
				rMaterial.SetSpecular(AIColor3DToVec4(newColour, MAX_MATERUAL_ALPHA));
			if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess))
				rMaterial.SetShininess(shininess);

			rMaterial.SetShader(LoadShaderProgram("Default"));
			// Add Texture Loader
			for (unsigned int k = 0; k < material->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE); k++) {
				aiString path;
				material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, k, &path);
				if (path.length > 0) {
					std::shared_ptr<Texture> t = LoadTexture(directory, path.C_Str(), TextureType::DIFFUSE);
					if (t) rMaterial.AddTexture(t);
				}
			}
			std::shared_ptr<MeshRenderer> renderer = node->GetComponent<MeshRenderer>();
			if (!renderer) renderer = node->AddComponent<MeshRenderer>();
			if (renderer) {
				renderer->AddMesh(mMesh);
				renderer->AddMaterial(rMaterial);
				meshes[key] = mMesh;
				materials[key] = rMaterial;
				Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_RENDERING_SYSTEM, EventPriority::EVENT_ADD_QUEUE, EventDataType::EVENT_DATA_MESH, std::any(ObjectContainer<std::shared_ptr<Mesh>>(mMesh)));
				mMesh->eventData = e;
				mMesh->PushToRenderer();
				mMesh->ObserveRenderSwitch();
			}
		}
	}

	const std::shared_ptr<Node> FileInput::ProcessScene(const std::string & directory, const std::string & fileName, const aiScene * scene) {
		
		std::shared_ptr<Node> toReturn = ProcessNode(directory, fileName, scene, scene->mRootNode);
		
		return toReturn;
	}

	std::shared_ptr<Node> FileInput::LoadModel(const std::string & directory, const std::string & fileName, const bool & singleInstance, const bool & flipWinding) {
		std::shared_ptr<Node> toReturn = std::shared_ptr<Node>();
		const std::string filePath = directory + "/" + fileName;
		if (!FileExists(filePath)) {
			std::cout << "File " << filePath << " Doesn't exist!" << std::endl;
		}
		else {
			
			Assimp::Importer importer;
			;
			bool store = false;
			//importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, 4);
			if (loadedModels.count(filePath) != 0) {
				importer = loadedModels[filePath];
				const aiScene *scene = importer.GetScene();
				if (!scene) {
					std::cout << "Unable to load model: " << importer.GetErrorString() << std::endl;
				}
				else {
					if (scene->HasAnimations()) toReturn = LoadAnimatedModel(directory, fileName, flipWinding);
					else toReturn = ProcessScene(directory, fileName, scene);
				}
			}
			else {
				importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
				unsigned int flags = aiProcess_SortByPType | aiProcess_FindInvalidData | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality | aiProcess_GenSmoothNormals;
				if (flipWinding) flags |= aiProcess_FlipWindingOrder;
				const aiScene *scene = importer.ReadFile(filePath, flags);
				if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE) std::cout << importer.GetErrorString() << std::endl;

				if (!scene) {
					std::cout << "Unable to load model: " << importer.GetErrorString() << std::endl;
				}
				else {
					if (scene->HasAnimations()) toReturn = LoadAnimatedModel(directory, fileName, flipWinding);
					else toReturn = ProcessScene(directory, fileName, scene);
					if (store) loadedModels[filePath] = importer;
				}
			}
			
			
		}
		return toReturn;
	}

	std::shared_ptr<Node> FileInput::LoadAnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding) {
		std::shared_ptr<Node> toReturn;
		std::shared_ptr<AnimatedModel> model = std::make_shared<AnimatedModel>(directory, fileName, flipWinding);
		const aiScene * scene = model->GetScene();
		if (scene && scene->mRootNode) {
			aiNode * node = scene->mRootNode;
			const std::string nodeName = scene->mRootNode->mName.C_Str();
			toReturn = Engine::GetInstance().CreateNode(nodeName);
			
			std::shared_ptr<Animator> animator = toReturn->AddComponent<Animator>();
			animator->SetModel(model);
			std::vector<MeshConstructionInfo> info = model->GetMeshConstructionInfo();
			aiMatrix4x4 transform = node->mTransformation;
			aiVector3D position(0.0f), scale(1.0f);
			aiQuaternion rotation = aiQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
			transform.Decompose(scale, rotation, position);
			toReturn->SetPosition(AIVec3ToVec3(position));
			toReturn->SetRotation(AIQuaternionToQuat(rotation));
			toReturn->SetScale(AIVec3ToVec3(scale));
			for (size_t i = 0; i < info.size(); i++) {
				AddMeshesAndMaterials(scene, directory, fileName, toReturn, info[0]);
			}
		}
		return toReturn;
	}

	std::shared_ptr<Texture> FileInput::LoadTexture(const std::string & directory, const std::string & fileName, const TextureType & type) {
		std::shared_ptr<Texture> toReturn = std::shared_ptr<Texture>();

		const std::string filePath = directory + "/" + fileName;
		if (!FileExists(filePath)) {
			std::cout << "File " << filePath << " Doesn't exist!" << std::endl;
			return toReturn;
		}
		else {
			if (textures.count(filePath) != 0) toReturn = textures[filePath];
			else {
				if (textureLoader) {
					unsigned char * imageData = nullptr;
					std::int32_t width = 0, height = 0, channels = 4;
					std::uint32_t size = 0;
					textureLoader->GetTextureData(filePath, &imageData, width, height, channels, size);
					toReturn = std::make_shared<Texture>();
					toReturn->filePath = filePath;
					toReturn->mtype = type;
					toReturn->width = (std::size_t)std::max(0, width);
					toReturn->height = (std::size_t)std::max(0, height);
					toReturn->channels = (std::size_t)std::max(0, channels);
					toReturn->dataSize = size;
					toReturn->textureData = imageData;
					const bool validTexture = imageData == nullptr;
					if (validTexture) std::cout << "Image Data is Null" << std::endl;
					else {
						std::cout << "Loaded texture " << fileName << std::endl;
						Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_RENDERING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_TEXTURE, std::any(ObjectContainer<std::shared_ptr<Texture>>(toReturn)));
						toReturn->eventData = e;
						textures[filePath] = toReturn;
						toReturn->PushToRenderer();
						toReturn->ObserveRenderSwitch();
					}
				}
			}
		}
		return toReturn;
	}

	void FileInput::SetTextureLoader(std::unique_ptr<TextureLoader> & loader) {
		textureLoader = std::move(loader);
	}

	std::shared_ptr<ShaderProgram> FileInput::LoadShaderProgram(const std::string & name) {
		if (shaders.count(name) > 0) return shaders[name];
		std::shared_ptr<ShaderProgram> toReturn = std::make_shared<ShaderProgram>(name);
		if (name.compare("Default") == 0) {

			toReturn->AddShader(Shader(ShaderType::VERTEX_SHADER, "Default"));
			toReturn->AddShader(Shader(ShaderType::FRAGMENT_SHADER, "Default"));
			for (size_t i = 0; i < RESERVED_ATTRIBUTES.size(); i++) {
				toReturn->AddAttribute(RESERVED_ATTRIBUTES[i]);
			}
			toReturn->AddUniform(UniformValue("model", ShaderDataFormat::MAT4, { ShaderStage::VERTEX }));
			toReturn->AddUniform(UniformValue("normalMatrix", ShaderDataFormat::MAT3, { ShaderStage::VERTEX }));
			toReturn->AddUniform(UniformValue("viewProjection", ShaderDataFormat::MAT4, { ShaderStage::VERTEX }));
			toReturn->AddUniform(UniformValue("viewPosition", ShaderDataFormat::VEC3, { ShaderStage::VERTEX }));
			toReturn->AddUniform(UniformValue("material", ShaderDataFormat::MATERIAL, { ShaderStage::FRAGMENT }));
			toReturn->AddUniform(UniformValue("textureUnit0", ShaderDataFormat::IMAGE2D, { ShaderStage::FRAGMENT }));
			toReturn->AddUniform(UniformValue("pointLights", ShaderDataFormat::POINTLIGHT, { ShaderStage::FRAGMENT }, 5));
			Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_RENDERING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_SHADER_PROGRAM, std::any(ObjectContainer<std::shared_ptr<ShaderProgram>>(toReturn)));
			toReturn->eventData = e;
		}
		
		shaders[name] = toReturn;
		toReturn->PushToRenderer();
		toReturn->ObserveRenderSwitch();
		return toReturn;
	}
	AnimationData::AnimationData() : skeleton(std::shared_ptr <Skeleton>()) {
	}
}
