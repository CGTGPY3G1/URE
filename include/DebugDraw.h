#ifndef B00289996B00227422_DEBUG_DRAW_H
#define B00289996B00227422_DEBUG_DRAW_H
#include "Bounds.h"
#include <glm/vec3.hpp>
#include <vector>
#include <memory>
namespace B00289996 {

	class ShaderProgram;
	/// <summary>
	/// Debug Drawer
	/// </summary>
	/// <seealso cref="btIDebugDraw" />
	class DebugDraw {
	public:
		/// <summary>Initializes a new instance of the <see cref="DebugDraw"/> class. </summary>
		DebugDraw();
		/// <summary>
		/// Finalizes an instance of the <see cref="DebugDraw"/> class.
		/// </summary>
		~DebugDraw();

		void DrawLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &colour = glm::vec3(0.0f, 1.0f, 0.0f));
		void DrawSphere(const Sphere & sphere, const glm::vec3 & colour = glm::vec3(0.0f, 1.0f, 0.0f));
		void DrawOBB(const OBB & obb, const glm::vec3 &colour = glm::vec3(0.0f, 1.0f, 0.0f));
		/// <summary>Renders the cached data</summary>
		void Render(const glm::mat4 & viewProjection);
		/// <summary>Flushes the cached data.</summary>
		void FlushLines();
	private:
		
		void Init();
		unsigned int VAO, positionBuffer, colourBuffer;
	protected:
		int debugMode;
		unsigned int noOfVerts = 0;
		std::vector<glm::vec3> verts, colours;
		unsigned int shaderID;
	};
}
#endif // !B00289996B00227422_DEBUG_DRAW_H