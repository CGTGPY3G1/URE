#include "WindowSDL2.h"
#include "MessagingSystem.h"
#include <iostream>
namespace B00289996 {
	WindowSDL2::WindowSDL2(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType) 
		: Window(name, width, height, rendererType), window(nullptr){
	}
	WindowSDL2::~WindowSDL2() {
	}
	void WindowSDL2::CreateDisplay() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cout << "Initialization Failed" << std::endl;
			return;
		}
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8); // Turn on x8 multisampling anti-aliasing (MSAA)
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		window = SDL_CreateWindow(mName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			mWidth, mHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (!window) {
			std::cout << "Window not created" << std::endl;
			return;
		}
		glContext = SDL_GL_CreateContext(window);
		if (!glContext) {
			std::cout << "Rendering context not created" << std::endl;
			return;
		}
		SDL_SetWindowResizable(window, SDL_TRUE);

		SDL_GL_SetSwapInterval(1);
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			std::cout << "glewInit failed." << std::endl;
			return;
		}
	}

	void WindowSDL2::ProcessEvent(const Event & toProcess) {
		switch (toProcess.GetType()) {
		case EventType::EVENT_TYPE_SWAP:
			if (window) {
				SDL_GL_SwapWindow(window);
			}
			else std::cout << "Window is NULL" << std::endl;
			SDL_Event events;
			while (SDL_PollEvent(&events)) {
				if (events.type == SDL_QUIT) {
					return;
				}
				else if (events.type == SDL_WINDOWEVENT) {
					switch (events.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
					case SDL_WINDOWEVENT_SIZE_CHANGED:
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						break;
					case SDL_WINDOWEVENT_CLOSE:
						break;
					default:
						//std::cout << "Unknown SDL event" << std::endl;
						break;
					}
				}
			}
			break;
		case EventType::EVENT_TYPE_NULL:
			std::cout << "Null Event Type Sent To Window" << std::endl;
			break;
		default:
			std::cout << "The SDL2 window couldn't process an event!" << std::endl;
			break;
		}
	
	}
}
