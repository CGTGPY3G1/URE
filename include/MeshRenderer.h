#ifndef UNNAMED_MESH_RENDERER_H
#define UNNAMED_MESH_RENDERER_H
#include "Component.h"
#include "Bounds.h"
#include "Material.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

namespace B00289996 {
	class Mesh;

	class MeshRenderer : public Component {
	public:
		/// <summary>Initializes a new instance of the <see cref="MeshRenderer"/> class.</summary>
		MeshRenderer();
		/// <summary>Finalizes an instance of the <see cref="MeshRenderer"/> class.</summary>
		~MeshRenderer();
		/// <summary>Adds a sub-mesh.</summary>
		/// <param name="toAdd">the new sub-mesh.</param>
		void AddMesh(const std::shared_ptr<Mesh> & toAdd);
		void SetMesh(const std::shared_ptr<Mesh> & toAdd, const std::size_t index = 0);
		void EnsuseMeshArraySize(const std::size_t size);
		void ResizeMeshArray(const std::size_t size);
		virtual const std::shared_ptr<Mesh> GetMesh(const std::size_t index = 0) const;
		/// <summary>Gets all submeshes attached to this.</summary>
		/// <returns>an array of submeshes, if there are meshes attached, else an empty array</returns>
		const std::vector<std::shared_ptr<Mesh>> GetMeshes() const;
		/// <summary>Sets this mesh renderers material.</summary>
		/// <param name="newMaterial">The new material.</param>
		void SetMaterial(const Material & newMaterial, const std::size_t index = 0);
		void AddMaterial(const Material & newMaterial);
		/// <summary>Gets the material used by this.</summary>
		/// <returns>the material</returns>
		Material GetMaterial();
		const Material GetMaterial(const std::size_t index) const;
		const std::size_t GetNumberOfMaterials() const;
		/// <summary>Gets the Axis-Aligned Bounding Box that encoses this, will be calculated using all attached meshes.</summary>
		/// <returns>the Axis-Aligned Bounding Box</returns>
		const AABB GetAABB();
		/// <summary>Gets bounding sphere of this mesh renderer, will be calculated using all attached meshes.</summary>
		/// <returns>the radius</returns>
		const Sphere GetSphere();
		const OBB GetOBB();
		virtual const BitMask GetComponentID() const override {
			return TypeInfo::GetComponentID<MeshRenderer>();
		}
		virtual const std::shared_ptr<Component> Clone() const override {
			std::shared_ptr<MeshRenderer> toReturn = std::make_shared<MeshRenderer>(*this);
			return std::static_pointer_cast<Component>(toReturn);
		}
		void SetOwner(const std::weak_ptr<Node> & owner) override;
	private:
		void RegisterWithCullingSystem(const bool & connect);
		void SetTransformDirty(const bool & newValue = true);
		void SetVerticesDirty(const bool & newValue = true);
		/// <summary>Recalculates the Bounds.</summary>
		void RecalculateBounds();
		AABB originalAABB, transformedAABB;
		Sphere originalSphere, transformedSphere;
		OBB transformedOBB;
		std::vector<std::shared_ptr<Mesh>> meshes;
		std::vector<Material> materials;
		bool dirtyTransform, dirtyVertices, registeredWithCulling;
	};

}

#endif // !UNNAMED_MESH_RENDERER_H