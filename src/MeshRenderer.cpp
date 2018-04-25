#include "MeshRenderer.h"
#include "Node.h"
#include "MessagingSystem.h"
#include "Geometry.h"
#include "Material.h"
#include "Texture.h"
#include "ShaderProgram.h"
namespace B00289996 {
	MeshRenderer::MeshRenderer() : Component(), originalAABB(AABB()), transformedAABB(AABB()), originalSphere(), transformedSphere(),
		meshes(std::vector<std::shared_ptr<Mesh>>()), materials(std::vector<Material>()), dirtyTransform(true), dirtyVertices(true), registeredWithCulling(false) {
	}

	MeshRenderer::~MeshRenderer() {
		RegisterWithCullingSystem(false);
	}

	void MeshRenderer::SetMesh(const std::shared_ptr<Mesh>& newMesh, const std::size_t index) {
		if(index >= meshes.size()) meshes.resize(index + 1);
		meshes[index] = newMesh;
		SetVerticesDirty();
	}

	void MeshRenderer::EnsuseMeshArraySize(const std::size_t size) {
		if (size >= meshes.size()) meshes.resize(size);
	}

	void MeshRenderer::ResizeMeshArray(const std::size_t size) {
		meshes.resize(size);
	}

	void MeshRenderer::AddMesh(const std::shared_ptr<Mesh>& toAdd) {
		meshes.push_back(toAdd);
		SetVerticesDirty();
	}

	const std::shared_ptr<Mesh> MeshRenderer::GetMesh(const std::size_t index) const {
		if (index >= meshes.size()) return std::shared_ptr<Mesh>();
		return meshes[index];
	}
	const std::vector<std::shared_ptr<Mesh>> MeshRenderer::GetMeshes() const {
		return meshes;
	}

	void MeshRenderer::SetMaterial(const Material & newMaterial, const std::size_t index) {
		if (index >= materials.size()) materials.resize(index + 1);
		if (materials.size() > 0) materials[0] = newMaterial;
	}
	void MeshRenderer::AddMaterial(const Material & newMaterial) {
		materials.push_back(newMaterial);
	}
	Material MeshRenderer::GetMaterial() {
		return (materials.size() > 0) ? materials[0] : Material();
	}
	const Material MeshRenderer::GetMaterial(const std::size_t index) const {
		if (index >= materials.size()) return Material();
		return materials[index];
	}
	const std::size_t MeshRenderer::GetNumberOfMaterials() const {
		return materials.size();
	}
	const AABB MeshRenderer::GetAABB() {
		if (dirtyTransform || dirtyVertices) RecalculateBounds();
		return transformedAABB;
	}
	const Sphere MeshRenderer::GetSphere() {
		if (dirtyTransform || dirtyVertices) RecalculateBounds();
		return transformedSphere;
	}

	const OBB MeshRenderer::GetOBB() {
		if (dirtyTransform || dirtyVertices) RecalculateBounds();
		return transformedOBB;
	}

	void MeshRenderer::SetOwner(const std::weak_ptr<Node>& owner) {
		if (owner.use_count() > 0) {
			RegisterWithCullingSystem(false);
		}
		Component::SetOwner(owner);
		if (owner.use_count() > 0) {
			RegisterWithCullingSystem(true);
			AddConnection(owner.lock()->AttachTransformUpdateObserver(std::bind(&MeshRenderer::SetTransformDirty, this, std::placeholders::_1)));
			SetTransformDirty();
		}
	}

	void MeshRenderer::RegisterWithCullingSystem(const bool & connect) {
		if (!registeredWithCulling && connect) {
			std::shared_ptr<Node> node = GetOwner().lock();
			if (node) {
				std::any data;
				data.emplace<ObjectContainer<std::shared_ptr<Node>>>(ObjectContainer<std::shared_ptr<Node>>(node));
				Event e = Event(EventType::EVENT_TYPE_REGISTER, EventTarget::TARGET_CULLING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_NODE, data);
				MessagingSystem::GetInstance().SendEvent(e);
				registeredWithCulling = true;
			}
		}
		else if (registeredWithCulling && !connect) {
			std::shared_ptr<Node> node = GetOwner().lock();
			if (node) {
				std::any data;
				data.emplace<ObjectContainer<std::shared_ptr<Node>>>(ObjectContainer<std::shared_ptr<Node>>(node));
				Event e = Event(EventType::EVENT_TYPE_UNREGISTER, EventTarget::TARGET_CULLING_SYSTEM, EventPriority::EVENT_SEND_IMMEDIATELY, EventDataType::EVENT_DATA_NODE, data);
				MessagingSystem::GetInstance().SendEvent(e);
				registeredWithCulling = false;
			}
		}
	}

	void MeshRenderer::SetTransformDirty(const bool & newValue) {
		if (dirtyTransform != newValue) dirtyTransform = newValue;
	}

	void MeshRenderer::SetVerticesDirty(const bool & newValue) {
		if (dirtyVertices != newValue) dirtyVertices = newValue;
	}

	void MeshRenderer::RecalculateBounds() {
		if (dirtyVertices) {
			if (meshes.size() > 0) {
				glm::vec3 min(std::numeric_limits<float>().max()), max(std::numeric_limits<float>().min());
				Sphere newSphere;
				AABB mAABB;
				Sphere mSphere;
				bool first = true;
				for (std::vector<std::shared_ptr<Mesh>>::iterator i = meshes.begin(); i != meshes.end(); i++) {
					mAABB = (*i)->GetAABB();
					mSphere = (*i)->GetSphere();
					if (first) {
						first = false;
						newSphere = mSphere;
						min = mAABB.min;
						max = mAABB.max;
					}
					else {
						if (mAABB.min.x < min.x) min.x = mAABB.min.x;
						if (mAABB.max.x > max.x) max.x = mAABB.max.x;
						if (mAABB.min.y < min.y) min.y = mAABB.min.y;
						if (mAABB.max.y > max.y) max.y = mAABB.max.y;
						if (mAABB.min.z < min.z) min.z = mAABB.min.z;
						if (mAABB.max.z > max.z) max.z = mAABB.max.z;
						const glm::vec3 displacement = mSphere.centre - newSphere.centre;
						const glm::vec3 direction = glm::normalize(displacement);
						const glm::vec3 point = mSphere.centre + (direction * mSphere.radius);
						if (!newSphere.Contains(point)) {
							const glm::vec3 back = newSphere.centre - (direction * newSphere.radius);
							newSphere.radius = glm::length(point - back) * 0.5f;
							newSphere.centre = back + (direction * newSphere.radius);
						}
					}
				}
				originalAABB.min = min; originalAABB.max = max;
				originalSphere = newSphere;
				dirtyVertices = false;
			}
			std::shared_ptr<Node> node = owner.lock();
			if (node) {
				transformedOBB = node->TransformOBB(originalAABB);
				transformedAABB = transformedOBB;
				transformedSphere = node->TransformSphere(originalSphere);
				dirtyTransform = false;
			}
		}
		else if (dirtyTransform) {
			std::shared_ptr<Node> node = owner.lock();
			if (node) {
				transformedOBB = node->TransformOBB(originalAABB);
				transformedAABB = transformedOBB;
				transformedSphere = node->TransformSphere(originalSphere);
				dirtyTransform = false;
			}
		}
	}
}
