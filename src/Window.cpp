#include "Window.h"
#include "MessagingSystem.h"
#include "Engine.h"
namespace B00289996 {
	bool Window::maximised = false;
	Window::Window(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType, const std::uint32_t & positionX, const std::uint32_t & positionY)
		: ConnectionMananger(), mName(name), mWidth(width), mHeight(height), mRendererType(rendererType) {
		AddConnection(MessagingSystem::GetInstance().AttachObserver(std::bind(&Window::ProcessEvent, this, std::placeholders::_1), EventTarget::TARGET_WINDOWING_SYSTEM));
	}

	Window::~Window() {
	}
}
