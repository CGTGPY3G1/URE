#ifndef UNNAMED_WINDOW_H
#define UNNAMED_WINDOW_H
#include <vector>
#include <memory>
#include "Node.h"
#include "Emitter.h"

namespace B00289996 {
	enum class RendererType : std::uint8_t;
	class Event;
	class Window : public ConnectionMananger {
	public:
		Window(const std::string & name, const std::uint32_t & width, const std::uint32_t & height, const RendererType & rendererType, const std::uint32_t & positionX = 100, const std::uint32_t & positionY = 100);
		virtual ~Window();
		virtual void CreateDisplay() = 0;
	protected:
		virtual void ProcessEvent(const Event & toProcess) = 0;
		std::string mName;
		std::uint32_t mWidth, mHeight;
		RendererType mRendererType;
		static bool maximised;
	};
}

#endif // !UNNAMED_WINDOW_H