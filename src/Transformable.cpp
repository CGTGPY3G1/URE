#include "Transformable.h"
#include <algorithm>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Bounds.h"
#include "IDGenerator.h"
namespace B00289996 {
	Transformable::Transformable(const std::string & nodeName) : SGObject(), name(nodeName), dirty(true), scaleDirty(true),
		transformNotification(std::make_shared<Emitter<const bool>>("TransformNotification")),
		localPosition(glm::vec3(0.0f)), worldPosition(glm::vec3(0.0f)), localScale(glm::vec3(1.0f)), worldScale(glm::vec3(1.0f)), forward(glm::vec3(0.0f, 0.0f, 1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)),
		localRotation(glm::quat()), worldRotation(glm::quat()), worldTransform(glm::mat4(1.0f)), localTransform(glm::mat4(1.0f)), world2Local(glm::mat4(1.0f)), normalmatrix(glm::mat3(1.0f)) {
		id = IDGenerator<Transformable, std::uint32_t>::GetInstance().GetNewID();
	}

	Transformable::~Transformable() {
		IDGenerator<Transformable, std::uint32_t>::GetInstance().RetireID(id);
	}

	const glm::vec3 Transformable::GetPosition() {
		UpdateTransform();
		return worldPosition;
	}

	const glm::vec3 Transformable::GetLocalPosition() const {
		return localPosition;
	}

	void Transformable::SetPosition(const glm::vec3 newPosition) {
		SetPosition(newPosition.x, newPosition.y, newPosition.z);
	}

	void Transformable::SetPosition(const float & x, const float & y, const float & z) {
		if (parent.use_count() > 0) {
			localPosition = parent.lock()->TransformToLocalPoint(x, y, z);
		}
		else {
			localPosition.x = x; localPosition.y = y; localPosition.z = z;
		}
		SetDirty();
	}

	void Transformable::SetLocalPosition(const glm::vec3 newPosition) {
		SetLocalPosition(newPosition.x, newPosition.y, newPosition.z);
	}

	void Transformable::SetLocalPosition(const float & x, const float & y, const float & z) {
		localPosition.x = x, localPosition.y = y, localPosition.z = z;
		SetDirty();
	}

	void Transformable::SetRotation(const glm::quat & newRotation) {
		if (parent.use_count() > 0) localRotation = parent.lock()->GetRotation() * glm::inverse(newRotation);
		else localRotation = newRotation;
		SetDirty();
	}

	void Transformable::SetLocalRotation(const glm::quat & newRotation) {
		localRotation = newRotation;
		SetDirty();
	}

	void Transformable::Translate(const float & x, const float & y, const float & z, const bool & worldSpace) {
		UpdateTransform();
		if (worldSpace) {
			localPosition.x += x;
			localPosition.y += y;
			localPosition.z += z;
		}
		else localPosition += glm::vec3(-glm::rotate(localRotation, (glm::vec4(x, y, z, 0.0f))));
		SetDirty();
	}

	void Transformable::Translate(const glm::vec3 & displacement, const bool & worldSpace) {
		Translate(displacement.x, displacement.y, displacement.z, worldSpace);
	}

	void Transformable::Rotate(const glm::vec3 & axis, const float & angle) {
		localRotation = glm::normalize(glm::rotate(localRotation, glm::radians(angle), axis));
		SetDirty();
	}

	const glm::quat Transformable::GetRotation() {
		UpdateTransform();
		return worldRotation;
	}

	const glm::quat Transformable::GetLocalRotation() const {
		return localRotation;
	}

	void Transformable::Scale(const float & scale) {
		this->localScale *= scale;
		SetDirty();
	}

	void Transformable::SetScale(const float & x, const float & y, const float & z) {
		if (parent.use_count() > 0) {
			const glm::vec3 pScale = parent.lock()->GetScale();
			localScale.x = x / pScale.x; localScale.y = y / pScale.y; localScale.z = z / pScale.z;
		}
		else {
			localScale.x = x; localScale.y = y; localScale.z = z;
		}
		SetDirty();
	}

	void Transformable::SetScale(const glm::vec3 & scale) {
		SetScale(scale.x, scale.y, scale.z);
	}

	void Transformable::SetLocalScale(const float & x, const float & y, const float & z) {
		localScale.x = x; localScale.y = y; localScale.z = z;
		SetDirty();
	}

	void Transformable::SetLocalScale(const glm::vec3 & scale) {
		localScale = scale;
		SetDirty();
	}

	const glm::vec3 Transformable::GetScale() {
		UpdateTransform();
		return worldScale;
	}

	const glm::vec3 Transformable::GetLocalScale() const {
		return localScale;
	}

	const glm::vec3 Transformable::GetForward() {
		UpdateTransform();
		return forward;
	}

	const glm::vec3 Transformable::GetUp() {
		UpdateTransform();
		return up;
	}

	const glm::vec3 Transformable::GetRight() {
		UpdateTransform();
		return right;
	}

	const glm::vec3 Transformable::TransformToWorldPoint(const glm::vec3 & point) {
		return TransformToWorldPoint(point.x, point.y, point.z);
	}

	const glm::vec3 Transformable::TransformToWorldPoint(const float & x, const float & y, const float & z) {
		UpdateTransform();
		return glm::vec3(worldTransform * glm::vec4(x, y, z, 1.0f));
	}

	const glm::vec3 Transformable::TransformToLocalPoint(const glm::vec3 & point) {
		return TransformToLocalPoint(point.x, point.y, point.z);
	}

	const glm::vec3 Transformable::TransformToLocalPoint(const float & x, const float & y, const float & z) {
		UpdateTransform();
		return glm::vec3(world2Local * glm::vec4(x, y, z, 1.0f));
	}

	const glm::vec3 Transformable::TransformToWorldNormal(const glm::vec3 & normal) {
		return TransformToWorldNormal(normal.x, normal.y, normal.z);
	}

	const glm::vec3 Transformable::TransformToWorldNormal(const float & x, const float & y, const float & z) {
		UpdateTransform();
		return glm::vec3(worldTransform * glm::vec4(x, y, z, 0.0f));
	}

	const glm::vec3 Transformable::TransformToLocalNormal(const glm::vec3 & normal) {
		UpdateTransform();
		return TransformToLocalNormal(normal.x, normal.y, normal.z);
	}

	const glm::vec3 Transformable::TransformToLocalNormal(const float & x, const float & y, const float & z) {
		UpdateTransform();
		return glm::vec3(world2Local * glm::vec4(x, y, z, 0.0f));
	}

	void Transformable::SetParent(const std::weak_ptr<Transformable> & newParent) {
		if (parent.use_count() > 0) {
			parent.lock()->RemoveChild(std::static_pointer_cast<Transformable>(shared_from_this()));
		}
		parent = newParent;
		if (parent.use_count() > 0) {
			parent.lock()->AddChild(std::static_pointer_cast<Transformable>(shared_from_this()));
		}
		SetDirty();
	}

	const std::weak_ptr<Transformable> Transformable::GetParent() const {
		return parent;
	}

	void Transformable::SlerpRotation(const glm::quat & slerpTo, const float & alpha) {
		localRotation = glm::slerp(localRotation, slerpTo, alpha);
		SetDirty();
	}

	glm::quat Transformable::LookAt(const glm::vec3 & eye, const glm::vec3 & centre, const glm::vec3 & up) {
		const glm::mat3 look = glm::mat3(glm::lookAt(eye, centre, up));
		return glm::normalize(glm::quat_cast(look));
	}

	glm::quat Transformable::SlerpQuaternion(const glm::quat & slerpFrom, const glm::quat & slerpTo, const float & alpha) {
		return glm::slerp(slerpFrom, slerpTo, alpha);;
	}

	void Transformable::LerpPosition(const glm::vec3 & lerpTo, const float & alpha) {
		localPosition = glm::lerp(localPosition, lerpTo, alpha);
		SetDirty();
	}

	glm::vec3 Transformable::SlerpVec3(const glm::vec3 & slerpFrom, const glm::vec3 & slerpTo, const float & alpha) {
		return  glm::slerp(slerpFrom, slerpTo, alpha);
	}


	Sphere Transformable::TransformSphere(const Sphere & sphere) {
		Sphere toReturn; // create a new Sphere to return
						 // translate and scale the Sphere
		toReturn.centre = TransformToWorldPoint(sphere.centre);
		// scale the radius by the largest of the scales components 
		glm::vec3 scale = GetScale();
		toReturn.radius = sphere.radius * std::max(std::abs(scale.x), std::max(std::abs(scale.y), std::abs(scale.z)));
		return toReturn; // return the new Sphere
	}

	const std::uint32_t Transformable::GetID() const {
		return id;
	}



	void Transformable::RemoveChild(std::weak_ptr<Transformable> child) {
		std::vector<std::weak_ptr<Transformable>>::iterator it = std::remove_if(children.begin(), children.end(), [&](const std::weak_ptr<Transformable>& other) { return other.expired() && other.lock()->id == child.lock()->id; });
		if (it != children.end()) children.erase(it);
	}

	void Transformable::RemoveChild(const std::size_t index) {
		if (index < children.size()) children.erase(children.begin() + index);
	}

	const size_t Transformable::NumberOfChildren() const {
		return children.size();
	}

	void Transformable::AddChild(const std::shared_ptr<Transformable>& child) {
		if (std::find_if(children.begin(), children.end(), [&](const std::weak_ptr<Transformable>& other) { return !other.expired() && other.lock()->id == child->id; }) == children.end()) {
			children.push_back(child);
		};
		SetDirty();
	}

	const std::weak_ptr<Transformable> Transformable::GetChild(const std::size_t index) const {
		if (index < children.size()) return children[index];
		return std::weak_ptr<Transformable>();
	}

	const std::vector<std::weak_ptr<Transformable>> Transformable::GetChildren() const {
		return children;
	}

	const glm::mat4 Transformable::GetLocalTransform() {
		if (dirty) UpdateTransform();
		return localTransform;
	}


	const glm::mat4 Transformable::GetWorldTransform() {
		if (dirty) UpdateTransform();
		return worldTransform;
	}

	const glm::mat3 Transformable::GetInverseTransposed() {
		if (dirty) UpdateTransform();
		return inverseTranspose;
	}

	const glm::mat3 Transformable::GetNormalMatrix() {
		if (dirty) UpdateTransform();
		return normalmatrix;
	}

	std::shared_ptr<Connection> Transformable::AttachTransformUpdateObserver(const std::function<void(const bool)> & reqest) {
		return transformNotification->Connect(reqest);
	}

	AABB Transformable::TransformAABB(const AABB & aabb) {
		// relies on implicit conversion from aabb to obb, then back to aabb
		return TransformOBB(aabb);
	}

	OBB Transformable::TransformOBB(const OBB & obb) {
		OBB toReturn;
		toReturn.centre = TransformToWorldPoint(obb.centre);
		glm::vec3 scale = GetScale();
		toReturn.extents = glm::vec3(std::abs(obb.extents.x * scale.x), std::abs(obb.extents.y * scale.y), std::abs(obb.extents.z * scale.z));
		toReturn.rotation = (obb.rotation * GetRotation());
		return toReturn;
	}

	void Transformable::UpdateTransform() {
		if (dirty) {
			localRotation = glm::normalize(localRotation);
			const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), localPosition);
			const glm::mat4 rotationMatrix = glm::mat4_cast(localRotation);
			const glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), localScale);
			localTransform = translationMatrix * rotationMatrix * scaleMatrix;
			if (parent.use_count() > 0) {
				std::shared_ptr<Transformable> p = parent.lock();
				if (p) {
					p->UpdateTransform();
					worldTransform = p->worldTransform * localTransform;
					worldRotation = glm::normalize(p->worldRotation * localRotation);
				}
				else {
					worldTransform = localTransform;
					worldRotation = localRotation;
				}
			}
			else {
				worldTransform = localTransform;
				worldRotation = localRotation;
			}
			normalmatrix = glm::mat3_cast(worldRotation);
			inverseTranspose = glm::inverseTranspose(glm::mat3(worldTransform));
			world2Local = glm::inverse(worldTransform);
			worldPosition = worldTransform[3];
			worldScale = glm::vec3(glm::length(glm::vec3(worldTransform[0])), glm::length(glm::vec3(worldTransform[1])), glm::length(glm::vec3(worldTransform[2])));
			SetDirty(false);
			forward = TransformToWorldNormal(glm::vec3(0.0f, 0.0f, -1.0f)), up = TransformToWorldNormal(glm::vec3(0.0f, 1.0f, 0.0f)), right = TransformToWorldNormal(glm::vec3(1.0f, 0.0f, 0.0f));
			
		}
	}

	void Transformable::SetDirty(const bool & newValue) {
		if (newValue) transformNotification->Emit(std::forward<const bool>(true));
		if (dirty != newValue) {
			dirty = newValue;
			if (newValue) {
				for (size_t i = 0; i < children.size(); i++) {
					std::shared_ptr<Transformable> c = children[i].lock();
					if (c) c->SetDirty();
				}
			}
		}
	}
}
