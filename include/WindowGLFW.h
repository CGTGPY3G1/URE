#ifndef UNNAMED_WINDOW_GLFW_H
#define UNNAMED_WINDOW_GLFW_H
#include "Window.h"
#include <gl/glew.h>
#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>
namespace B00289996 {
	enum class KeyboardKey : std::int32_t;
	enum class MouseButton : std::int32_t;
	class WindowGLFW : public Window {
	public:
		WindowGLFW(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType, const std::uint32_t & positionX = 100, const std::uint32_t & positionY = 100);
		~WindowGLFW();
		static void ResizedCallback(GLFWwindow *window, int width, int height);
		static void RepositionedCallback(GLFWwindow* window, int xpos, int ypos);
		void CreateDisplay() override;
	protected:
		std::uint32_t width, height, positionX, positionY;
		std::string title;
		std::int32_t ToGLFWKey(const KeyboardKey & key);
		std::int32_t ToGLFWMouseButton(const MouseButton & button);
		bool KeyPressed(const KeyboardKey & key);
		bool MouseButtonPressed(const MouseButton & button);
		void ProcessEvent(const Event & toProcess) override;
		GLFWwindow * window;
		bool initialized;
	};
}

#endif // !UNNAMED_WINDOW_GLFW_H