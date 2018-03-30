#include "WindowFactory.h"
#include "WindowGLFW.h"
#include "Window.h"
#include "Enums.h"
namespace B00289996 {
	WindowFactory::WindowFactory() : IFactory<Window, WindowCreationInfo>() {
	}

	std::shared_ptr<Window> WindowFactory::CreateShared(const WindowCreationInfo & params) {
		if (params.windowType == WindowType::WINDOW_TYPE_GLFW) return std::move(std::make_shared<WindowGLFW>(params.title, params.width, params.height, params.rendererType, params.positionX, params.positionY));
		return std::shared_ptr<Window>();
	}

	std::unique_ptr<Window> WindowFactory::CreateUnique(const WindowCreationInfo & params) {
		if (params.windowType == WindowType::WINDOW_TYPE_GLFW) return std::move(std::make_unique<WindowGLFW>(params.title, params.width, params.height, params.rendererType, params.positionX, params.positionY));
		return std::move(std::unique_ptr<Window>());
	}

}
