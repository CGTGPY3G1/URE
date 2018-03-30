#include "Bounds.h"
#include <glm/gtx/norm.hpp>
namespace B00289996 {

	AABB::AABB(const glm::vec3 & minimum, const glm::vec3 & maximum) : min(minimum), max(maximum) {
	}

	AABB::AABB(const Sphere & sphere) {
		const glm::vec3 offset = glm::vec4(sphere.radius);
		min = sphere.centre - offset;
		max = sphere.centre + offset;
	}

	AABB::AABB(const OBB & obb) {
		min = max = obb.centre;
		const glm::mat3 rotationMatrix = glm::mat3_cast(obb.rotation); // create a rotation matrix from the obbs rotation
																	   // use the rotation matrix to construct the obbs local up, right and forward vectors
		const glm::vec3 lRight = rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), lForward = rotationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), lUp = rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f);
		float width = obb.extents.x * 2.0f, height = obb.extents.y * 2.0f, length = obb.extents.z * 2.0f; // get the width height and length of the obb

																													  // construct all 8 vertices of the obb and store them in an array
		const glm::vec3 leftBottomBack = rotationMatrix * -obb.extents + obb.centre, rightBottomBack = leftBottomBack + lRight * width, rightTopBack = rightBottomBack + lUp * height, leftTopBack = rightTopBack + lRight * -width,
			leftBottomFront = leftBottomBack + lForward * length, rightBottomFront = leftBottomFront + lRight * width, rightTopFront = rightBottomFront + lUp * height, leftTopFront = rightTopFront + lRight * -width;
		const glm::vec3 corners[8]{ leftBottomBack, rightBottomBack, rightTopBack, leftTopBack, leftBottomFront, rightBottomFront, rightTopFront, leftTopFront };
		for (size_t i = 0; i < 8; i++) {
			const glm::vec3 corner = corners[i];
			if (corner.x < min.x) min.x = corner.x;
			else if (corner.x > max.x) max.x = corner.x;
			if (corner.y < min.y) min.y = corner.y;
			else if (corner.y > max.y) max.y = corner.y;
			if (corner.z < min.z) min.z = corner.z;
			else if (corner.z > max.z) max.z = corner.z;
		}
	}

	const bool AABB::Intersects(const AABB & other) const {
		return (min.x > other.max.x || max.x < other.min.x) 
			|| (min.y > other.max.y || max.y < other.min.y) 
			|| (min.z > other.max.z || max.x < other.min.z);
	}

	const bool AABB::Contains(const AABB & other) const {
		return (min.x < other.max.x && max.x > other.min.x)
			&& (min.y < other.max.y && max.y > other.min.y)
			&& (min.z < other.max.z && max.x > other.min.z);
	}

	const AABB AABB::Combine(const AABB & other) const {
		return AABB(glm::vec3(glm::min(min.x, other.min.x), glm::min(min.y, other.min.y), glm::min(min.z, other.min.z)), glm::vec3(glm::max(max.x, other.max.x), glm::max(max.y, other.max.y), glm::max(max.z, other.max.z)));
	}

	OBB::OBB(const glm::vec3 & position, const glm::vec3 & halfSize, const glm::quat & direction) : centre(position), extents(halfSize), rotation(direction) {
	}

	OBB::OBB(const AABB & aabb) {
		extents = (aabb.max - aabb.min) * 0.5f;
		centre = aabb.min + extents;
		rotation = glm::quat();
	}

	Sphere::Sphere(const glm::vec3 & position, const float & rad) : centre(position), radius(rad) {
	}
	static const float Square(const float & value) { return value * value; }
	const bool Sphere::Intersects(const Sphere & other) const {
		return (glm::length2(other.centre - centre) <= Square(radius + other.radius));
	}

	const bool Sphere::Contains(const Sphere & other) const {
		return (glm::length2(other.centre - centre) + Square(other.radius) <= Square(radius));
	}

	const Sphere Sphere::Combine(const Sphere & other) const {
		const glm::vec3 rad1 = glm::vec3(radius), rad2 = glm::vec3(other.radius);
		const glm::vec3 min = glm::min(centre - rad1, other.centre - rad2), max = glm::max(centre + rad1, other.centre + rad2);
		const glm::vec3 position = max - min;
		const glm::vec3 halfExtents = max - position;
		const float radius = glm::max(halfExtents.x, glm::max(halfExtents.y, halfExtents.z));
		return Sphere(position, radius);
	}

}
