#include "AnimatedModel.h"
#include <glm/glm.hpp>
#include <iostream>
#include <assimp/postprocess.h>
#include <glm/ext.hpp>
namespace B00289996 {

	AnimatedModel::AnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding) : std::enable_shared_from_this<AnimatedModel>(), animationSpeed(0.1f) {
		Init(directory, fileName, flipWinding);
	}

	void AnimatedModel::SetAnimation(const std::uint32_t & animationIndex) {
		if(animationIndex < scene->mNumAnimations) pAnimation = scene->mAnimations[animationIndex];
	}

	void AnimatedModel::LoadBones(std::uint32_t meshIndex, const aiMesh * pMesh) {
		for (uint32_t i = 0; i < pMesh->mNumBones; i++) {
			uint32_t index = 0;
			assert(pMesh->mNumBones <= MAX_BONES);
			std::string name = pMesh->mBones[i]->mName.C_Str();
			if (boneMapping.find(name) == boneMapping.end()) {
				// Bone not present, add new one
				index = numBones;
				numBones++;
				BoneTransform bone;
				boneInfo.push_back(bone);
				boneInfo[index].offset = pMesh->mBones[i]->mOffsetMatrix;
				boneMapping[name] = index;
			}
			else {
				index = boneMapping[name];
			}

			for (uint32_t j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
				uint32_t vertexID = meshData[meshIndex].vertexBase + pMesh->mBones[i]->mWeights[j].mVertexId;
				bones[vertexID].Add(index, pMesh->mBones[i]->mWeights[j].mWeight);
			}
		}
		boneTransforms.resize(numBones);
	}

	void AnimatedModel::Update(const float & time) {
		float TicksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 24.0f);
		float TimeInTicks = time * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);
		aiMatrix4x4 identity;
		//transformTree = std::make_shared<BoneTransformTree>();
		ReadNodeHierarchy(AnimationTime, scene->mRootNode, identity);
		for (uint32_t i = 0; i < boneTransforms.size(); i++) {
			boneTransforms[i] = boneInfo[i].finalTransformation;
		}
	}

	std::vector<MeshConstructionInfo> AnimatedModel::GetMeshConstructionInfo() {
		std::vector<MeshConstructionInfo> toReturn;
		toReturn.resize(meshData.size());
		for (size_t m = 0; m < meshData.size(); m++) {
			const std::size_t numberOfverts = meshData[m].vertices.size();
			Vertex vertex;
			std::uint32_t boneStart = meshData[m].vertexBase;
			for (size_t i = 0; i < numberOfverts; i++) {
				vertex.position = meshData[m].vertices[i].position;
				vertex.textureCoords = meshData[m].vertices[i].textureCoords;
				vertex.normal = meshData[m].vertices[i].normal;
				vertex.tangent = meshData[m].vertices[i].tangent;
				vertex.biTangent = meshData[m].vertices[i].biTangent;
				vertex.colour = meshData[m].vertices[i].colour;
				for (size_t j = 0; j < MAX_BONE_WEIGHTS; j++) {
					std::uint32_t n = boneStart + i;
					vertex.boneIDs[j] = bones[n].IDs[j];
					vertex.boneWeights[j] = bones[n].weights[j];
				}
				toReturn[m].verts.push_back(vertex);			
				toReturn[m].indices = meshData[m].Indices;
				toReturn[m].materialIndex = meshData[m].materialIndex;
				toReturn[m].meshName = meshData[m].name;
			}
		}
		return toReturn;
	}

	bool AnimatedModel::Init(const std::string & directory, const std::string & fileName, const bool & flipWinding) {
		const std::string filePath = directory + "/" + fileName;
		importer.SetPropertyInteger(AI_CONFIG_PP_SBBC_MAX_BONES, MAX_BONES);
		importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, MAX_BONE_WEIGHTS);
		importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS | aiComponent_LIGHTS);
		unsigned int flags = aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_RemoveComponent | aiProcess_SplitByBoneCount;
		if (flipWinding) flags |= aiProcess_FlipWindingOrder;
		scene = importer.ReadFile(filePath, flags);
		std::cout << importer.GetErrorString() << std::endl;
		if (!scene) {
			std::cout << "Unable to load model: " << importer.GetErrorString() << std::endl;
		}
		else {
			globalInverseTransform = scene->mRootNode->mTransformation;
			globalInverseTransform.Inverse();
			meshData.resize(scene->mNumMeshes);
			// Counters
			for (unsigned int i = 0; i < meshData.size(); i++) {
				meshData[i].vertexBase = numVertices;
				meshData[i].meshIndex = i;

				numVertices += scene->mMeshes[i]->mNumVertices;
			}

			// Initialize the meshes in the scene one by one
			for (unsigned int i = 0; i < meshData.size(); i++) {
				const aiMesh* paiMesh = scene->mMeshes[i];
				InitMesh(i, paiMesh, scene);
			}
			bones.resize(numVertices, VertexBoneData());
			for (uint32_t i = 0; i < meshData.size(); i++) {
				aiMesh *paiMesh = scene->mMeshes[i];
				if (paiMesh->mNumBones > 0) {
					LoadBones(i, paiMesh);
				}
			}
			SetAnimation(0);
		}
		return true;
	}

	void AnimatedModel::InitMesh(unsigned int index, const aiMesh * paiMesh, const aiScene * pScene) {
		meshData[index].materialIndex = paiMesh->mMaterialIndex;

		aiColor3D pColor(0.f, 0.f, 0.f);
		pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

		aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
			aiVector3D* pPos = &(paiMesh->mVertices[i]);
			aiVector3D* pNormal = &(paiMesh->mNormals[i]);
			aiVector3D *pTexCoord;
			if (paiMesh->HasTextureCoords(0)) {
				pTexCoord = &(paiMesh->mTextureCoords[0][i]);
			}
			else {
				pTexCoord = &Zero3D;
			}
			aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[i]) : &Zero3D;
			aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[i]) : &Zero3D;

			VertexData v;
			v.position = glm::vec3(pPos->x, pPos->y, pPos->z);
			v.textureCoords = glm::vec2(pTexCoord->x, pTexCoord->y);
			v.normal = glm::vec3(pNormal->x, pNormal->y, pNormal->z);
			v.tangent = glm::vec3(pTangent->x, pTangent->y, pTangent->z);
			v.biTangent = glm::vec3(pBiTangent->x, pBiTangent->y, pBiTangent->z);
			v.colour = glm::vec4(pColor.r, pColor.g, pColor.b, 1.0f);
			meshData[index].vertices.push_back(v);
		}

		for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
			const aiFace& Face = paiMesh->mFaces[i];
			if (Face.mNumIndices != 3)
				continue;
			meshData[index].Indices.push_back(Face.mIndices[0]);
			meshData[index].Indices.push_back(Face.mIndices[1]);
			meshData[index].Indices.push_back(Face.mIndices[2]);
		}
	} 

	const aiNodeAnim * AnimatedModel::FindNodeAnim(const aiAnimation * animation, const std::string & nodeName) {
		if (animation) {
			for (uint32_t i = 0; i < animation->mNumChannels; i++) {
				const aiNodeAnim* nodeAnim = animation->mChannels[i];
				if (std::string(nodeAnim->mNodeName.C_Str()).compare(nodeName) ==  0){
					return nodeAnim;
				}
			}
		}
		return nullptr;
	}

	aiMatrix4x4 AnimatedModel::InterpolateTranslation(const float & time, const aiNodeAnim * pNodeAnim) {
		aiVector3D translation;

		if (pNodeAnim->mNumPositionKeys == 1) {
			translation = pNodeAnim->mPositionKeys[0].mValue;
		}
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
				if (time < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = pNodeAnim->mPositionKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mPositionKeys[(frameIndex + 1) % pNodeAnim->mNumPositionKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

			const glm::vec3 start = AIVec3ToVec3(currentFrame.mValue);
			const glm::vec3 end = AIVec3ToVec3(nextFrame.mValue);

			translation = Vec3ToAIVec3(glm::lerp(start, end, delta));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Translation(translation, mat);
		return mat;
	}

	aiMatrix4x4 AnimatedModel::InterpolateScale(const float & time, const aiNodeAnim * pNodeAnim) {
		aiVector3D scale;

		if (pNodeAnim->mNumScalingKeys == 1) {
			scale = pNodeAnim->mScalingKeys[0].mValue;
		}
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
				if (time < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiVectorKey currentFrame = pNodeAnim->mScalingKeys[frameIndex];
			aiVectorKey nextFrame = pNodeAnim->mScalingKeys[(frameIndex + 1) % pNodeAnim->mNumScalingKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);
			const glm::vec3 start = AIVec3ToVec3(currentFrame.mValue);
			const glm::vec3 end = AIVec3ToVec3(nextFrame.mValue);
			
			scale = Vec3ToAIVec3(glm::lerp(start, end, delta));
		}

		aiMatrix4x4 mat;
		aiMatrix4x4::Scaling(scale, mat);
		return mat;
	}

	void AnimatedModel::ReadNodeHierarchy(const float & AnimationTime, const aiNode * pNode, const aiMatrix4x4 & ParentTransform) {
		std::string NodeName = pNode->mName.C_Str();

		aiMatrix4x4 NodeTransformation(pNode->mTransformation);

		/*std::shared_ptr<BoneTransformTree> btt = std::make_shared<BoneTransformTree>();
		btt->parent = transformTree;
		transformTree->children.push_back(btt);*/
		
		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Get interpolated matrices between current and next frame
			aiMatrix4x4 matScale = InterpolateScale(AnimationTime, pNodeAnim);
			aiMatrix4x4 matRotation = InterpolateRotation(AnimationTime, pNodeAnim);
			aiMatrix4x4 matTranslation = InterpolateTranslation(AnimationTime, pNodeAnim);

			NodeTransformation = matTranslation * matRotation * matScale;
		}

		aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;
		//btt->transform = AIMat4ToMat4(globalInverseTransform * GlobalTransformation);
		if (boneMapping.find(NodeName) != boneMapping.end()) {
			uint32_t BoneIndex = boneMapping[NodeName];
			boneInfo[BoneIndex].finalTransformation = globalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].offset;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}

	aiMatrix4x4 AnimatedModel::InterpolateRotation(const float & time, const aiNodeAnim * pNodeAnim) {
		aiQuaternion rotation;

		if (pNodeAnim->mNumRotationKeys == 1) {
			rotation = pNodeAnim->mRotationKeys[0].mValue;
		}
		else {
			uint32_t frameIndex = 0;
			for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
				if (time < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
					frameIndex = i;
					break;
				}
			}

			aiQuatKey currentFrame = pNodeAnim->mRotationKeys[frameIndex];
			aiQuatKey nextFrame = pNodeAnim->mRotationKeys[(frameIndex + 1) % pNodeAnim->mNumRotationKeys];

			float delta = (time - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);


			const glm::quat start = AIQuaternionToQuat(currentFrame.mValue);
			const glm::quat end = AIQuaternionToQuat(nextFrame.mValue);

			rotation = QuatToAIQuaternion(glm::normalize(glm::slerp(start, start, delta)));
		}

		aiMatrix4x4 mat(rotation.GetMatrix());
		return mat;
	}

}
