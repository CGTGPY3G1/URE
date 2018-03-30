#ifndef UNNAMED_RENDERER_OPENGL3_H
#define UNNAMED_RENDERER_OPENGL3_H
#include "Renderer.h"
#include <glm/mat4x4.hpp>
#include "DebugDraw.h"
typedef unsigned int GLuint;

namespace B00289996 {
	class Node;
	class ContextManagerOpenGL3;
	struct BoneTransformTree;
	class RendererOpenGL3 : public Renderer {
	public:
		/// <summary>Initializes a new instance of the <see cref="RendererOpenGL3"/> class.</summary>
		RendererOpenGL3();
		/// <summary>Finalizes an instance of the <see cref="RendererOpenGL3"/> class.</summary>
		~RendererOpenGL3();
		void Render(const std::vector<std::shared_ptr<Node>>& objects) override;
		void BeginRendering() override;
		void EndRendering() override;
	protected:
		void ProcessEvent(const Event & toProcess) override;
	private:
		void DebugRenderBoneTree(const std::shared_ptr<BoneTransformTree> & tree, const glm::mat4 & model);
		void SetCameraUniforms(const GLuint & shaderID, const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3 & camPosition);
		void SetLightingUniforms(const GLuint & shaderID, std::vector<std::shared_ptr<Node>> lights);
		void ProcessDeregistrationEvent(const Event & toProcess);
		void ProcessRegistrationEvent(const Event & toProcess);
		DebugDraw debugDraw;
	};
}

#endif // !UNNAMED_RENDERER_OPENGL3_H