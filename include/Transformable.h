#ifndef UNNAMED_TRANSFORMABLE_H
#define UNNAMED_TRANSFORMABLE_H
#include <string>
#include <memory>
#include <vector>
#include "Object.h"
#include <glm/mat4x4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
namespace B00289996 {
	struct Sphere;
	struct AABB;
	struct OBB;
	class Transformable : public SGObject {
	public:

		Transformable(const std::string & name = "Unnamed");
		~Transformable();

		const glm::vec3 GetPosition();
		const glm::vec3 GetLocalPosition() const;
		void SetPosition(const glm::vec3 newPosition);
		void SetPosition(const float & x, const float & y, const float & z);
		void SetLocalPosition(const glm::vec3 newPosition);
		void SetLocalPosition(const float & x, const float & y, const float & z);
		const glm::quat GetRotation();
		const glm::quat GetLocalRotation() const;
		void SetRotation(const glm::quat & newRotation);
		void SetLocalRotation(const glm::quat & newRotation);
		void Translate(const float & x, const float & y, const float & z, const bool & worldSpace = false);
		void Translate(const glm::vec3 & displacement, const bool & worldSpace = false);
		void Rotate(const glm::vec3 & axis, const float & angle);
		void Scale(const float & scale);
		void SetScale(const float & x, const float & y, const float & z);
		void SetScale(const glm::vec3 & scale);
		void SetLocalScale(const float & x, const float & y, const float & z);
		void SetLocalScale(const glm::vec3 & scale);
		const glm::vec3 GetScale();
		const glm::vec3 GetLocalScale() const;
		const glm::vec3 GetForward();
		const glm::vec3 GetUp();
		const glm::vec3 GetRight();
		const glm::vec3 TransformToWorldPoint(const glm::vec3 & point);
		const glm::vec3 TransformToWorldPoint(const float & x, const float & y, const float & z);
		const glm::vec3 TransformToLocalPoint(const glm::vec3 & point);
		const glm::vec3 TransformToLocalPoint(const float & x, const float & y, const float & z);
		const glm::vec3 TransformToWorldNormal(const glm::vec3 & normal);
		const glm::vec3 TransformToWorldNormal(const float & x, const float & y, const float & z);
		const glm::vec3 TransformToLocalNormal(const glm::vec3 & normal);
		const glm::vec3 TransformToLocalNormal(const float & x, const float & y, const float & z);
		void SetParent(const std::weak_ptr<Transformable> & newParent);
		const std::weak_ptr<Transformable> GetParent() const;
		void SlerpRotation(const glm::quat & slerpTo, const float & alpha);
		static glm::quat LookAt(const glm::vec3 & eye, const glm::vec3 & centre, const glm::vec3 & up = glm::vec3(0.0f, 1.0f, 0.0f));
		static glm::quat SlerpQuaternion(const glm::quat & slerpFrom, const glm::quat & slerpTo, const float & alpha);
		void LerpPosition(const glm::vec3 & lerpTo, const float & alpha);
		static glm::vec3 SlerpVec3(const glm::vec3 & slerpFrom, const glm::vec3 & slerpTo, const float & alpha);
		const size_t NumberOfChildren() const;
		void AddChild(const std::shared_ptr<Transformable> & child);
		const std::weak_ptr<Transformable> GetChild(const std::size_t index) const;
		const std::vector<std::weak_ptr<Transformable>> GetChildren() const;
		virtual const glm::mat4 GetLocalTransform();
		virtual const glm::mat4 GetWorldTransform();
		const glm::mat3 GetInverseTransposed();
		const glm::mat3 GetNormalMatrix();
		std::shared_ptr<Connection> AttachTransformUpdateObserver(const std::function<void(const bool)> & reqest);
		AABB TransformAABB(const AABB & aabb);
		OBB TransformOBB(const OBB & obb);
		Sphere TransformSphere(const Sphere & sphere);
		const std::uint32_t GetID() const;
	protected:
		virtual void UpdateTransform();
		std::shared_ptr<Emitter<const bool>> transformNotification;
		void RemoveChild(std::weak_ptr<Transformable> child);
		void RemoveChild(const std::size_t index);
		void SetDirty(const bool & newValue = true);
		glm::vec3 localPosition = glm::vec3(0.0f), worldPosition, localScale = glm::vec3(1.0f), worldScale = glm::vec3(1.0f), forward, up, right;
		glm::quat localRotation, worldRotation;
		glm::mat4 worldTransform, localTransform, world2Local;
		glm::mat3 normalmatrix, inverseTranspose;
		bool dirty, scaleDirty;
		std::weak_ptr<Transformable> parent;
		std::vector<std::weak_ptr<Transformable>> children;
	private:
		std::uint32_t id;
		
		static const glm::mat4 IDENTITY;
		std::string name;
	};

	struct FindTransformById {
		FindTransformById(const std::uint32_t & toFind) : id(toFind) {}
		FindTransformById(const std::shared_ptr<Transformable> & toFind) : id(toFind->GetID()) {}
		FindTransformById(const std::weak_ptr<Transformable> & toFind) : id(toFind.lock()->GetID()) {}
		bool operator()(const std::shared_ptr<Transformable> & other) { return other && other->GetID() == id; }
		bool operator()(const std::weak_ptr<Transformable> & other) { return other.use_count() > 0 && other.lock()->GetID() == id; }
		std::uint32_t id;
	};
}
#endif // !UNNAMED_NODE_H
