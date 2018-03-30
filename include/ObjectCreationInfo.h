#ifndef UNNAMED_OBJECT_CREATION_INFO_H
#define UNNAMED_OBJECT_CREATION_INFO_H
#include "Enums.h"
#include <string>
namespace B00289996 {
	struct WindowCreationInfo {
		RendererType rendererType = RendererType::OPENGL;
		WindowType windowType = WindowType::WINDOW_TYPE_GLFW;
		std::uint32_t width = 1280, height = 720;
		std::uint32_t positionX = 100, positionY = 100;
		std::string title = "Unnamed Window";
	};
	inline bool operator==(const WindowCreationInfo & l, const WindowCreationInfo & r) { 
		return l.rendererType == r.rendererType
			&& l.windowType == r.windowType
			&& l.width == r.width
			&& l.height == r.height
			&& l.positionX == r.positionY
			&& l.positionY == r.positionY
			&& l.title.compare(r.title) == 0;
	}
	inline bool operator!=(const WindowCreationInfo & l, const WindowCreationInfo & r) { return !(l == r); }
}
#endif // !UNNAMED_OBJECT_CREATION_INFO_H