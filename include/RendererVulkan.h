#ifndef UNNAMED_RENDERER_VULKAN_H
#define UNNAMED_RENDERER_VULKAN_H
#include "Renderer.h"
#include <glm/mat4x4.hpp>

namespace B00289996 {

	class ContextManagerVulkan;
	class RendererVulkan : public Renderer {
	public:
		RendererVulkan();
		~RendererVulkan();
		void Render(const std::vector<std::shared_ptr<Node>>& objects) override;
		void BeginRendering() override;
		void EndRendering() override;
	protected:
		void ProcessEvent(const Event & toProcess) override;
	private:
		void ProcessDeregistrationEvent(const Event & toProcess);
		void ProcessRegistrationEvent(const Event & toProcess);
		// From LunarGs hologram demo.  transforms projection from OpenGL to Vulkan clip space
		const glm::mat4 vulkanPerspectiveClip = glm::mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 0.5f, 1.0f);
	};
}

#endif // !UNNAMED_RENDERER_VULKAN_H