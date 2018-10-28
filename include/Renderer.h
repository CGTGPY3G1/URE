#ifndef UNNAMED_RENDERER_H
#define UNNAMED_RENDERER_H
#include <memory>
#include <vector>
#include <glm/vec4.hpp>
#include "Emitter.h"
namespace B00289996 {
	class Event;
	class Node;
	class ContextManager;
	struct CullingResult;
	class Renderer : public ConnectionMananger {
	public:
		Renderer(const std::shared_ptr<ContextManager> renderContextManager);
		~Renderer();
		virtual void BeginRendering() = 0;
		virtual void Render(const std::vector<CullingResult> & cullingResults) = 0;
		virtual void EndRendering() = 0;
	protected:
		glm::vec4 clearColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		float depthValue = 1.0f;
		std::shared_ptr<ContextManager> contextManager;
	};
}

#endif // !UNNAMED_RENDERER_H