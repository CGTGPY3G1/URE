#include "Frustum.h"
#include "Bounds.h"

namespace B00289996 {
	Frustum::Frustum() {
	}

	void Frustum::DefinePerspective(const float & fov, const float & aspectRatio, const float & near, const float & far) {
		verticalProjectionFactor = glm::tan(fov * 0.5f);
		horizontalProjectionFactor = verticalProjectionFactor * aspectRatio;
		radiusScaleY = 1.0f / glm::cos(fov * 0.5f);
		radiusScaleX = 1.0f / glm::cos(atan(horizontalProjectionFactor));
		this->near = near;
		this->far = far;
		
	}

	void Frustum::DefineView(const glm::vec3 & position, const glm::vec3 & forward, const glm::vec3 & up) {
		this->position = position;
		this->forward = glm::normalize(forward);
		this->up = glm::normalize(up);
		this->right = glm::normalize(glm::cross(forward, up));
	}

	const bool Frustum::CheckSphere(const Sphere & sphere, const bool & fullyEnclosed) const {
		glm::vec3 toSphere = sphere.centre - position; // spheres position from frustums perspective
		// get the scaler viewprojection of toSphere onto the frustums local forward direction
		const float forwardProjection = glm::dot(forward, toSphere);
		const float radius = sphere.radius;

		// if the projected value - the spheres radius is less than the near plane value or 
		// the projected value + the spheres radius is more than the far plane then the sphere
		// cannot be fully enclosed by the frustum
		if (fullyEnclosed) {
			if (forwardProjection - radius < near || far < forwardProjection + radius) return false;
		}
		// if the projected value + the spheres radius is less than the near plane value or 
		// the projected value - the spheres radius is more than the far plane then the sphere
		// cannot be intersected by the frustum
		else if (forwardProjection + radius < near || far < forwardProjection - radius)  return false;

		// get the scaler viewprojection of toSphere onto the frustums local right direction
		const float rightProjection = glm::dot(right, toSphere);
		// scale the forward viewprojection by the horizontal viewprojection factor
		float limit = horizontalProjectionFactor * forwardProjection; 
		// if the outer projected extents of the sphere are outside of the frusums horizontal extents 
		// then the sphere cannot be fully enclosed by the frustum
		if (fullyEnclosed) {
			// check the outer extents against the left and right projected extent
			if (rightProjection - radius*radiusScaleX < -limit || limit < rightProjection + radius*radiusScaleX) return false;
		}
		// if the inner projected extents of the sphere are outside of the frusums horizontal extents 
		// then the sphere cannot be intersected by the frustum
		else if (rightProjection + radius*radiusScaleX < -limit || limit < rightProjection - radius*radiusScaleX) return false;

		// get the scaler viewprojection of toSphere onto the frustums local up direction
		const float upProjection = glm::dot(up, toSphere); 
		// scale the forward viewprojection by the vertical viewprojection factor
		limit = verticalProjectionFactor * forwardProjection;
		// if the outer projected extents of the sphere are outside of the frusums vertical extents 
		// then the sphere cannot be fully enclosed by the frustum
		if (fullyEnclosed) {
			if (upProjection - radius*radiusScaleY < -limit || limit < upProjection + radius*radiusScaleY) return false;
		}
		// if the inner projected extents of the sphere are outside of the frusums vertical extents 
		// then the sphere cannot be intersected by the frustum
		else if (upProjection + radius*radiusScaleY < -limit || limit < upProjection - radius*radiusScaleY) return false;

		// Sphere is either fully enclosed or intersected by ther frustum
		return true;
	}

	const bool Frustum::CheckOBB(const OBB & obb, const bool & fullyEnclosed) const {
		// used to track which how many time the projected points exceed the bounds of the frustum
		int outOfRangeLeft = 0, outOfRangeRight = 0, outOfRangeFar = 0, outOfRangeNear = 0, outOfRangeTop = 0, outOfRangeBottom = 0;
		bool inRangeX, inRangeY, inRangeZ; // flags to determine if a vertex is inside the frustum

		const glm::mat3 rotationMatrix = glm::mat3_cast(obb.rotation); // create a rotation matrix from the obbs rotation
		// use the rotation matrix to construct the obbs local up, right and forward vectors
		const glm::vec3 lRight = rotationMatrix * glm::vec3(1.0f, 0.0f, 0.0f), lForward = rotationMatrix * glm::vec3(0.0f, 0.0f, 1.0f), lUp = rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f);
		float width = obb.extents.x * 2.0f, height = obb.extents.y * 2.0f, length = obb.extents.z * 2.0f; // get the width height and length of the obb

		// construct all 8 vertices of the obb and store them in an array
		const glm::vec3 leftBottomBack = rotationMatrix * -obb.extents + obb.centre - position, rightBottomBack = leftBottomBack + lRight * width, rightTopBack = rightBottomBack + lUp * height, leftTopBack = rightTopBack + lRight * -width,
			leftBottomFront = leftBottomBack + lForward * length, rightBottomFront = leftBottomFront + lRight * width, rightTopFront = rightBottomFront + lUp * height, leftTopFront = rightTopFront + lRight * -width;
		glm::vec3 corners[8] { leftBottomBack, rightBottomBack, rightTopBack, leftTopBack, leftBottomFront, rightBottomFront, rightTopFront, leftTopFront };

		glm::vec3 point;
		// iterate through the vertices
		for (int i = 0; i < 8; ++i) {
			inRangeX = inRangeY = inRangeZ = false;
			point = corners[i];

			const float rightProjection = glm::dot(point, right); // get the scaler viewprojection of the corner onto the frustums local right direction
			const float upProjection = glm::dot(point, up); // get the scaler viewprojection of the corner onto the frustums local up direction
			const float forwardProjection = glm::dot(point, forward); // get the scaler viewprojection of the corner onto the frustums local forward direction

			// if the right viewprojection value is less than the horizontal extents of the frustum at the projected forward point then the vertex must be outside the left extent of the frustum
			if (rightProjection < -horizontalProjectionFactor * forwardProjection) outOfRangeLeft++;
			// if the right prjection value is greater than the horizontal extents of the frustum at the projected forward point then the vertex must be outside the right extent of the frustum
			else if (rightProjection > horizontalProjectionFactor * forwardProjection) outOfRangeRight++;
			// the vertex is in the frustums horizontal range 
			else inRangeX = true;

			// if the up viewprojection value is less than the vertical extents of the frustum at the projected forward point then the vertex must be outside the bottom extent of the frustum
			if (upProjection < -verticalProjectionFactor * forwardProjection) outOfRangeBottom++;
			// if the up prjection value is greater than the vertical extents of the frustum at the projected forward point then the vertex must be outside the top extent of the frustum
			else if (upProjection > verticalProjectionFactor * forwardProjection) outOfRangeTop++;
			// the vertex is in the frustums vertical range 
			else inRangeY = true;

			// if the forward viewprojection value is less than the frustums near value then the vertex must be behind the frustum
			if (forwardProjection < near) outOfRangeNear++;
			// if the forward prjection value is greater than the frustums far value then the vertex must be beyond the far plane
			else if (forwardProjection > far) outOfRangeFar++;
			// the vertex is in the frustums near-far range 
			else inRangeZ = true;

			// if the vertex is in the frustums range on all axes the bounding box must be intersected by the frustum
			const bool inRange = (inRangeX && inRangeY && inRangeZ);
			if (!fullyEnclosed && inRange) return true;
			// if any vertex is out of range then the bounding box cannot be fully enclosed by the frustum
			else if (fullyEnclosed && !inRange) return false;
		}
		// if all of the vertices are out of range of any side of the frustum the object os not visible
		if (!fullyEnclosed && (outOfRangeLeft == 8 || outOfRangeRight == 8 || outOfRangeBottom == 8 || outOfRangeTop == 8 || outOfRangeNear == 8 || outOfRangeFar == 8)) return false;
		return true;
	}

}
