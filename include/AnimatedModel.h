#ifndef UNNAMED_ANIMATED_MODEL_H
#define UNNAMED_ANIMATED_MODEL_H
#include "Object.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <memory>
#include <map>
#include <array>
#include "Utility.h"
#include "Geometry.h"
namespace B00289996 {
	struct BoneTransformTree {
		BoneTransformTree() :transform(glm::mat4(1.0f)), children(std::vector<std::shared_ptr<BoneTransformTree>>()) {}
		~BoneTransformTree() {}
		std::shared_ptr<BoneTransformTree> parent;
		std::vector<std::shared_ptr<BoneTransformTree>> children;
		glm::mat4 transform;
	};
	struct BoneTransform {
		aiMatrix4x4 offset;
		aiMatrix4x4 finalTransformation;

		BoneTransform() {
			offset = aiMatrix4x4();
			finalTransformation = aiMatrix4x4();
		};
	};

	struct VertexData {
		VertexData() : textureCoords(glm::vec2(1.0f)), position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), tangent(glm::vec3(0.0f)), biTangent(glm::vec3(0.0f)), colour(glm::vec4(1.0f)) {}
		glm::vec2 textureCoords;
		glm::vec3 position, normal, tangent, biTangent;
		glm::vec4 colour;

	};

	struct MeshData {
		std::uint32_t materialIndex;
		std::uint32_t vertexBase;
		std::uint32_t meshIndex;
		std::string name;
		std::vector<VertexData> vertices;
		std::vector<unsigned int> Indices;

		void AddVertexData(const VertexData & vert) {
			vertices.push_back(vert); 
		}
	};
	// Per-vertex bone IDs and weights
	struct VertexBoneData {
		std::array<std::uint32_t, MAX_BONE_WEIGHTS> IDs{ 0, 0, 0, 0, };;
		std::array<float, MAX_BONE_WEIGHTS> weights{ 0.0f, 0.0f, 0.0f, 0.0f, };

		// Ad bone weighting to vertex info
		void Add(uint32_t boneID, float weight) {
			for (uint8_t i = 0; i < MAX_BONE_WEIGHTS; i++) {
				if (weights[i] < 0.00001f) {
					IDs[i] = boneID;
					weights[i] = weight;
					return;
				}
			}
		}
	};

	struct MeshConstructionInfo {
		std::string meshName;
		std::uint32_t meshIndex;
		std::uint32_t materialIndex;
		std::vector<Vertex> verts;
		std::vector<unsigned int> indices;
	};
	class AnimatedModel : public std::enable_shared_from_this<AnimatedModel> {
		friend class FileInput;
	public:
		AnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding = false);
		// Bone related stuff
		// Maps bone name with index
		std::map<std::string, uint32_t> boneMapping;
		// Bone details
		std::vector<BoneTransform> boneInfo;
		// Number of bones present
		uint32_t numBones = 0;
		// Root inverese transform matrix
		aiMatrix4x4 globalInverseTransform;
		// Per-vertex bone info
		std::vector<VertexBoneData> bones;
		// Bone transformations
		std::vector<aiMatrix4x4> boneTransforms;

		// Modifier for the animation 
		float animationSpeed = 1.0f;
		// Currently active animation
		aiAnimation* pAnimation;
		std::vector<MeshData> meshData;
		// Set active animation by index
		void SetAnimation(const std::uint32_t & animationIndex);
		
		// Load bone information from ASSIMP mesh
		void LoadBones(std::uint32_t meshIndex, const aiMesh* pMesh);

		// Recursive bone transformation for given animation time
		void Update(const float & time);
		std::vector<MeshConstructionInfo> GetMeshConstructionInfo();
		const aiScene* GetScene() const { return scene; };
		//std::shared_ptr<BoneTransformTree> GetBoneTransformTree();

		//std::shared_ptr<BoneTransformTree> transformTree;
	protected:
		const aiScene* scene{ nullptr };
		const aiScene* preTransformedScene{ nullptr };
		bool Init(const std::string & directory, const std::string & fileName, const bool & flipWinding);
		void InitMesh(unsigned int index, const aiMesh* mesh, const aiScene* pScene);
		// Find animation for a given node
		const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string & nodeName);
		// Returns a 4x4 matrix with interpolated translation between current and next frame
		aiMatrix4x4 InterpolateTranslation(const float & time, const aiNodeAnim* pNodeAnim);
		// Returns a 4x4 matrix with interpolated rotation between current and next frame
		aiMatrix4x4 InterpolateRotation(const float & time, const aiNodeAnim* pNodeAnim);
		// Returns a 4x4 matrix with interpolated scaling between current and next frame
		aiMatrix4x4 InterpolateScale(const float & time, const aiNodeAnim* pNodeAnim);
		// Get node hierarchy for current animation time
		void ReadNodeHierarchy(const float & AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
		Assimp::Importer importer;
		std::uint32_t numVertices = 0;
	};
}
#endif //!UNNAMED_ANIMATED_MODEL_H
