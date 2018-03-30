#ifndef UNNAMED_FILE_INPUT_H
#define UNNAMED_FILE_INPUT_H
#include <unordered_map>
#include <vector>
#include <assimp/Importer.hpp>
#include "Texture.h"
struct aiScene;
struct aiNode;
struct aiNodeAnim;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;

namespace B00289996 {
	class ShaderProgram;
	class TextureLoader;
	struct Shader;
	class Node;
	class Mesh;
	class Material;
	class Skeleton;
	class Bone;
	class Animation;
	struct MeshConstructionInfo;
	struct AnimationData {
		AnimationData();
		std::shared_ptr <Skeleton> skeleton;
	};
	class FileInput {
	public:	
		FileInput();
		/// <summary>Finalizes an instance of the <see cref="FileLoader"/> class.</summary>
		~FileInput();
		/// <summary>Loads a model (or the meshes that make a model).</summary>
		/// <param name="filePath">The models file path.</param>
		/// <param name="singleInstance">should the mesh be instanced.</param>
		/// <returns>an array of meshes, if the model exist, else an empty array</returns>
		std::shared_ptr<Node> LoadModel(const std::string & directory, const std::string & fileName, const bool & singleInstance = true, const bool & flipWinding = false);
		std::shared_ptr<Node> LoadAnimatedModel(const std::string & directory, const std::string & fileName, const bool & flipWinding = false);
		/// <summary>Loads a texture.</summary>
		/// <param name="filePath">The file path of the texture.</param>
		/// <param name="type">The type.</param>
		/// <returns>a texture, if it exist, else an empty shared pointer</returns>
		std::shared_ptr<Texture> LoadTexture(const std::string & directory, const std::string & fileName, const TextureType & type = TextureType::DIFFUSE);
		void SetTextureLoader(std::unique_ptr<TextureLoader> & loader);
		/// <summary>Loads a shader program from an array of shaders.</summary>
		/// <param name="vert">The shaders (vertex and fragment at minimum).</param>
		/// <returns>a shader program, if sucessful, else an empty shared pointer</returns>
		std::shared_ptr<ShaderProgram> LoadShaderProgram(const std::string & name);
		const std::uint64_t NumberOfLoadedVertices() const;
	private:
		const std::shared_ptr<Node> ProcessNode(const std::string & directory, const std::string & fileName, const aiScene * scene, const aiNode * node, std::shared_ptr<Node> parent = std::shared_ptr<Node>());
		const std::shared_ptr<Node> ProcessScene(const std::string & directory, const std::string & fileName, const aiScene * scene);
		void AddMeshesAndMaterials(const aiScene * scene, const std::string & directory, const std::string & fileName, std::shared_ptr<Node> & node, const MeshConstructionInfo & info);
		std::unique_ptr<TextureLoader> textureLoader;
		std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;
		std::unordered_map<std::string, Material> materials;
		std::unordered_map<std::string, std::shared_ptr<Texture>> textures;
		std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders;
		std::unordered_map<std::string, Assimp::Importer> loadedModels;
		
		std::uint64_t vertexCount = 0, unnamedAnimCount = 0;
		
	};
}

#endif // !UNNAMED_FILE_INPUT_H
