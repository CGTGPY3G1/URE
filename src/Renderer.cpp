#include "Renderer.h"
#include "MessagingSystem.h"
namespace B00289996 {

	Renderer::Renderer(const std::shared_ptr<ContextManager> renderContextManager) : ConnectionMananger(), contextManager(renderContextManager){
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&Renderer::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_RENDERING_SYSTEM));
	}
	Renderer::~Renderer() {
	}
}
