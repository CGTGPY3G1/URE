#ifndef UNNAMED_BOUNDS_H
#define UNNAMED_BOUNDS_H
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
namespace B00289996 {
	struct Sphere;
	struct OBB;
	// Axis-Aligned bounding box
	struct AABB {
		AABB(const glm::vec3 & minimum = glm::vec3(0.0f), const glm::vec3 & maximum = glm::vec3(0.0f));
		AABB(const Sphere & sphere);
		AABB(const OBB & obb);
		const bool Intersects(const AABB & other) const;
		const bool Contains(const AABB & other) const;
		const AABB Combine(const AABB & other) const;
		glm::vec3 min, max;
	};

	// (Object) Oriented bounding box
	struct OBB {
		OBB(const glm::vec3 & position = glm::vec3(0.0f), const glm::vec3 & halfSize = glm::vec3(0.0f), const glm::quat & direction = glm::quat());
		OBB(const AABB & aabb);
		glm::vec3 centre, extents;
		glm::quat rotation;
	};

	struct Sphere {
		Sphere(const glm::vec3 & position = glm::vec3(0.0f), const float & radius = 0.0f);
		glm::vec3 centre;
		float radius;
		const bool Intersects(const Sphere & other) const;
		const bool Contains(const Sphere & other) const;
		const Sphere Combine(const Sphere & other) const;
	};

	
}
#endif // !UNNAMED_BOUNDS_H