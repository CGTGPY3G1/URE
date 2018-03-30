#ifndef UNNAMED_WINDOW_SDL2_H
#define UNNAMED_WINDOW_SDL2_H
#include "Window.h"
#include <gl/glew.h>
#include <SDL.h>
namespace B00289996 {
	class WindowSDL2 : public Window {
	public:
		/// <summary>Initializes a new instance of the <see cref="MeshRenderer"/> class.</summary>
		WindowSDL2(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType);
		/// <summary>Finalizes an instance of the <see cref="MeshRenderer"/> class.</summary>
		~WindowSDL2();
		void CreateDisplay() override;
	protected:
		void ProcessEvent(const Event & toProcess) override;
		SDL_Window * window;
		SDL_GLContext glContext;
	};
}

#endif // !UNNAMED_WINDOW_SDL2_H