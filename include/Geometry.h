#ifndef UNNAMED_MESH_H
#define UNNAMED_MESH_H
#include "RenderableResource.h"
#include "MessagingSystem.h"
#include "Bounds.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

namespace B00289996 {
	constexpr uint32_t MAX_BONE_WEIGHTS = 4, MAX_BONES = 64;
	/// <summary> Vertex Specification </summary>
	struct Vertex {
		Vertex() : textureCoords(glm::vec2(1.0f)), position(glm::vec3(0.0f)), normal(glm::vec3(0.0f)), tangent(glm::vec3(0.0f)), biTangent(glm::vec3(0.0f)), boneIDs(glm::uvec4(0,0,0,0)), boneWeights{ 0.0f, 0.0f, 0.0f, 0.0f }, colour(glm::vec4(1.0f)) {}
		glm::vec2 textureCoords;
		glm::vec3 position, normal, tangent, biTangent;
		glm::vec4 colour;
		glm::uvec4 boneIDs;
		float boneWeights[MAX_BONE_WEIGHTS];
	};
	class Mesh : public RenderableResource {
		friend struct std::hash<Mesh>;
		friend struct std::hash<std::shared_ptr<Mesh>>;
		friend class FileInput;
	public:
		/// <summary>Initializes a new instance of the <see cref="Mesh"/> class.</summary>
		Mesh(const std::string & name = "UnnamedMesh");
		/// <summary>Finalizes an instance of the <see cref="Mesh"/> class.</summary>
		~Mesh();
		/// <summary>Gets the array of vertices that define this mesh.</summary>
		/// <returns>the array of vertices</returns>
		const std::vector<Vertex> GetVertices() const;
		/// <summary>Adds a vertex.</summary>
		/// <param name="verts">the vertex.</param>
		void AddVertex(const Vertex & vert);
		/// <summary>Adds an array of vertices.</summary>
		/// <param name="verts">the array of vertices.</param>
		void AddVertices(const std::vector<Vertex> & verts);
		/// <summary>Sets the array of vertices. (replaces any that already exist)</summary>
		/// <param name="verts">the array of vertices.</param>
		void SetVertices(const std::vector<Vertex> & verts);

		/// <summary>Gets the index count.</summary>
		/// <returns>the index count</returns>
		const size_t GetIndexCount() const;
		/// <summary>Gets the indices.</summary>
		/// <returns>The Indices</returns>
		const std::vector<std::uint32_t> GetIndices() const;
		/// <summary>Adds indices.</summary>
		/// <param name="meshIndices">an array of indices.</param>
		void AddIndices(const std::vector<std::uint32_t> & meshIndices);
		/// <summary>Sets the indices of this mesh. (replaces any that already exist)</summary>
		/// <param name="verts">the array indices.</param>
		void SetIndices(const std::vector<std::uint32_t> & meshIndices);
		/// <summary>Gets the Axis-Aligned Bounding Box that encoses this mesh.</summary>
		/// <returns>the Axis-Aligned Bounding Box</returns>
		const AABB GetAABB();
		/// <summary>Gets the sphere enclosing this mesh.</summary>
		/// <returns>the sphere</returns>
		const Sphere GetSphere();
		const std::uint16_t GetID() const;
		void SetDirty(const bool & setDirty = true);
		const bool UsesBoneData() const;

		void SeOffset(const glm::mat4 & offset);
		const glm::mat4 GetOffset() const;
	private:
		/// <summary>Recalculates the mesh Bounds.</summary>
		void RecalculateBounds();
		std::vector<std::uint32_t> indices;
		std::vector<Vertex> vertices;
		std::string meshName;
		std::uint16_t id;
		glm::mat4 offset;
		float radius;
		glm::vec3 centre;
		AABB aabb;
		bool dirty, useBoneData;
	};
}

namespace std {
	template <>
	struct hash<B00289996::Mesh> {
		std::size_t operator()(const B00289996::Mesh& k) const {
			return k.id;
		}
	};

	template <>
	struct hash<std::shared_ptr<B00289996::Mesh>> {
		std::size_t operator()(const std::shared_ptr<B00289996::Mesh>& k) const {
			return k->id;
		}
	};
}
#endif //!UNNAMED_MESH_H
