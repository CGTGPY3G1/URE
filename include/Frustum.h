#ifndef B00289996B00227422_FRUSTUM_H
#define B00289996B00227422_FRUSTUM_H
#include <glm/vec3.hpp>

namespace B00289996 {
	struct OBB;
	struct Sphere;
	class Frustum {
	public:
		Frustum();
		~Frustum() {}
		void DefinePerspective(const float & fov, const float & aspectRatio, const float & near, const float & far);
		void DefineView(const glm::vec3 & position, const glm::vec3 & forward, const glm::vec3 & up);
		const bool CheckSphere(const Sphere & sphere, const bool & fullyEnclosed = false) const;
		const bool CheckOBB(const OBB & obb, const bool & fullyEnclosed = false) const;
	private:
		glm::vec3 position, forward, right, up;
		float horizontalProjectionFactor, verticalProjectionFactor, near, far, radiusScaleX, radiusScaleY;
	};
}

#endif // !B00289996B00227422_FRUSTUM_H